#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>

#include "command.h"

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

void catCommand(char **arguments)
{
	execlp("cat", "cat", arguments[1], (char *)0);
}

void redirectSimpleCommand(char *currentCommandWord, char **currentCommandArguments)
{
	if (strcmp(currentCommandWord, "cat") == 0)
	{
		catCommand(currentCommandArguments);
	}
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

	// Ctrl-C signal handler
	signal(SIGINT, SIG_IGN);

	//EXIT
	if (strcmp(_simpleCommands[0]->_arguments[0], "exit") == 0)
	{
		printf("Good bye!!\n");
		exit(1);
	}

	//CHANGE DIRECTORY
	if (strcmp(_simpleCommands[0]->_arguments[0], "cd") == 0)
	{
		if (_simpleCommands[0]->_numberOfArguments == 1)
		{
			chdir(getenv("HOME"));
		}
		else
		{
			chdir(_simpleCommands[0]->_arguments[1]);
		}
		clear();
		prompt();
		return;
	}
	TODO:// Log file
	

	// Redirect the input/output/error files if necessary.
	int defaultIn = dup(0);
	int defaultOut = dup(1);
	int defaultErr = dup(2);
	int fdIn;
	int fdOut;
	int fdErr;
	pid_t childProcess;
	


	if (_inputFile)
	{
		fdIn = open(_inputFile, O_RDONLY, 0666);
	}
	else
	{
		fdIn = dup(defaultIn);
	}

	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{
		dup2(fdIn, 0);
		close(fdIn);

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
			else
			{
				fdOut = dup(defaultOut);
			}
		}
		else
		{
			int fdPipe[2];
			pipe(fdPipe);
			fdOut = fdPipe[1];
			fdIn = fdPipe[0];
		}

		dup2(fdOut, 1);
		close(fdOut);

		childProcess = fork();

		// error creating a child process
		if (childProcess == -1)
		{
			perror("fork");
			exit(2);
		}

		// check if the process is the parent or the child process forked
		if (childProcess == 0)
		{
			// Execute commands
			int _execution = execvp(_simpleCommands[0]->_arguments[0], _simpleCommands[0]->_arguments);

			// if execvp returns -1 the command is not a system call and must be redirected to implementation
			if (_execution == -1)
			{
				// redirect to the commands implementation
				redirectSimpleCommand(_simpleCommands[0]->_arguments[0], _simpleCommands[0]->_arguments);
			}
		}
	}

	dup2(defaultIn, 0);
	dup2(defaultOut, 1);
	dup2(defaultErr, 2);
	// close(defaultIn);
	// close(defaultOut);
	// close(defaultErr);
	// close(fdIn);
	// close(fdOut);

	if (!_background)
	{
		waitpid(childProcess, NULL, 0);
	}

	if (childProcess == 0)
	{
		exit(2);
	}

	// Clear to prepare for next command
	clear();

	// Print new prompt
	prompt();
}

// Shell implementation

void Command::prompt()
{
	printf("\nmyshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand *Command::_currentSimpleCommand;

int yyparse(void);

int main()
{
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}
