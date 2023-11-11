#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>

#include "command.h"

char *currentDirectory = get_current_dir_name();

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **)malloc(_numberOfAvailableArguments * sizeof(char *));
}

void SimpleCommand::insertArgument(char *argument)
{
	if (_numberOfAvailableArguments == _numberOfArguments + 1)
	{
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **)realloc(_arguments,
									  _numberOfAvailableArguments * sizeof(char *));
	}

	_arguments[_numberOfArguments] = argument;

	// Add NULL argument at the end
	_arguments[_numberOfArguments + 1] = NULL;

	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc(_numberOfSimpleCommands * sizeof(SimpleCommand *));

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append = 0;
}

void Command::insertSimpleCommand(SimpleCommand *simpleCommand)
{
	if (_numberOfAvailableSimpleCommands == _numberOfSimpleCommands)
	{
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **)realloc(_simpleCommands,
													_numberOfAvailableSimpleCommands * sizeof(SimpleCommand *));
	}

	_simpleCommands[_numberOfSimpleCommands] = simpleCommand;
	_numberOfSimpleCommands++;
}

void Command::clear()
{
	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{
		for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
		{
			free(_simpleCommands[i]->_arguments[j]);
		}

		free(_simpleCommands[i]->_arguments);
		free(_simpleCommands[i]);
	}

	if (_outFile)
	{
		free(_outFile);
	}

	if (_inputFile)
	{
		free(_inputFile);
	}

	if (_errFile)
	{
		free(_errFile);
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append = 0;
}

void Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");

	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{
		printf("  %-3d ", i);
		for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
		{
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[j]);
		}
		printf("\n");
	}

	printf("\n\n");
	printf("  Output       Input        Error        Background\n");
	printf("  ------------ ------------ ------------ ------------\n");
	printf("  %-12s %-12s %-12s %-12s\n", _outFile ? _outFile : "default",
		   _inputFile ? _inputFile : "default", _errFile ? _errFile : "default",
		   _background ? "YES" : "NO");
	printf("\n\n");
}

void logMessage(char *message)
{
	FILE *logFile = fopen("log.txt", "a");

	time_t rawTime;
	time(&rawTime);
	char *TIMESTAMP = ctime(&rawTime);

	fprintf(logFile, "%s- %s\n\n", TIMESTAMP, message);

	fclose(logFile);
}

void handleSigchld(int signum)
{
	logMessage("Child process terminated");
}

void exitCommand(SimpleCommand *currentSimpleCommand)
{
	// EXIT
	if (strcmp(currentSimpleCommand->_arguments[0], "exit") == 0)
	{
		printf("Good bye!!\n");
		logMessage("Exit shell");
		exit(1);
	}

	return;
}

int Command::cdCommand(int i)
{

	// CHANGE DIRECTORY
	if (strcmp(_simpleCommands[i]->_arguments[0], "cd") == 0)
	{
		if (_simpleCommands[i]->_numberOfArguments == 1)
		{
			chdir(getenv("HOME"));
		}
		else if (_simpleCommands[i]->_numberOfArguments == 2)
		{
			if (chdir(_simpleCommands[i]->_arguments[1]) == -1)
			{
				printf("cd: %s: No such file or directory\n", _simpleCommands[i]->_arguments[1]);
			}
		}
		else
		{
			printf("cd: Too many arguments\n");
		}

		char cwd[4096];

		// Get the current working directory
		if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
			currentDirectory = cwd;
		}

		return 1;
	}

	return 0;
}

void Command::execute()
{
	// Don't do anything if there are no simple commands
	if (_numberOfSimpleCommands == 0)
	{
		prompt();
		return;
	}

	// Print contents of Command data structure
	print();

	// Redirect the input/output/error files if necessary.
	int defaultIn = dup(0);
	int defaultOut = dup(1);
	// int defaultErr = dup(2);
	int fdIn = dup(defaultIn);
	int fdOut = dup(defaultOut);
	// int fdErr;

	pid_t childProcess;

	if (_inputFile)
	{
		fdIn = open(_inputFile, O_RDONLY, 0666);
	}

	int fdPipe[_numberOfSimpleCommands - 1][2];
	for (int i = 0; i < _numberOfSimpleCommands - 1; i++)
	{
		pipe(fdPipe[i]);
	}

	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{
		// checks if exit or cd command before forking a child process
		exitCommand(_simpleCommands[i]);
		if (cdCommand(i))
		{
			continue;
		}

		// Handle input redirection
		if (i == 0 && _inputFile)
		{
			int fdIn = open(_inputFile, O_RDONLY, 0666);
			dup2(fdIn, 0);
			close(fdIn);
		}

		// Handle output redirection
		if (i == _numberOfSimpleCommands - 1 && _outFile)
		{
			int fdOut;
			if (_append)
			{
				fdOut = open(_outFile, O_WRONLY | O_APPEND | O_CREAT, 0666);
			}
			else
			{
				fdOut = open(_outFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
			}
			dup2(fdOut, 1);
			close(fdOut);
		}

		if (i > 0)
		{
			// Close the read end of the previous pipe
			close(fdPipe[i - 1][0]);
			// Set the input file descriptor to the write end of the pipe
			dup2(fdPipe[i - 1][1], 1);
			close(fdPipe[i - 1][1]);
		}

		if (i < _numberOfSimpleCommands - 1)
		{
			// Close the write end of the current pipe
			close(fdPipe[i][1]);
			// Set the input file descriptor to the read end of the pipe
			dup2(fdPipe[i][0], 0);
			close(fdPipe[i][0]);
		}

		childProcess = fork();

		if (childProcess == 0)
		{
			if (i == _numberOfSimpleCommands - 1)
			{
				if (_outFile)
				{
					if (_append)
					{
						fdOut = open(_outFile, O_WRONLY | O_APPEND | O_CREAT, 0666);
					}
					else
					{
						fdOut = open(_outFile, O_WRONLY | O_CREAT, 0666);
					}
				}
			}

			execvp(_simpleCommands[i]->_arguments[0], _simpleCommands[i]->_arguments);
			exit(2);
		}
	}

	// Restore default input/output
	dup2(defaultIn, 0);
	dup2(defaultOut, 1);

	for (int i = 0; i < _numberOfSimpleCommands - 1; i++)
	{
		close(fdPipe[i][0]);
		close(fdPipe[i][1]);
	}

	if (!_background)
	{
		waitpid(childProcess, NULL, 0);
	}

	// Clear to prepare for next command
	clear();

	// Print new prompt
	prompt();
}

// Shell implementation
void Command::prompt()
{
	printf("\n%s>", currentDirectory);
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand *Command::_currentSimpleCommand;

int yyparse(void);

int main()
{
	// Disable CTRL+C in terminal
	signal(SIGINT, SIG_IGN);
	// For logging child termination
	signal(SIGCHLD, handleSigchld);

	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}
