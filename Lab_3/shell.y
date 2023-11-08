%union	{
		char   *string_val;
	}

%token	<string_val> WORD

%token 	NOTOKEN GREAT NEWLINE LESS DOUBLE_GREAT ANPERSAND PIPE

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdio.h>
#include "command.h"
%}

%%

goal:	
	commands
	;

commands: 
	command
	| commands command
	;

command: 
	simple_command
    ;

simple_command:	
	pipeline iomodifiers background NEWLINE {
		printf("   Yacc: Execute command\n");		
		Command::_currentCommand.execute();
	}
	| NEWLINE 
	| error NEWLINE { yyerrok; }
	;

pipeline:
	pipeline PIPE command_and_args
	| command_and_args
	;

command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
           printf("   Yacc: insert argument \"%s\"\n", $1);
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

command_word:
	WORD {
		   printf("   Yacc: insert command \"%s\"\n", $1);
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

iomodifiers:
	iomodifiers iomodifier
	| iomodifier
	|
	;

iomodifier:
	iomodifier_ipt
	| iomodifier_opt
	|iomodifier_opt_append
	;


iomodifier_opt:
	GREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	}
	;

iomodifier_opt_append:
	DOUBLE_GREAT WORD {
		printf("   Yacc: insert output in append mode \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._append = 1;
	}
	;

iomodifier_ipt:
	LESS WORD {
		printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
	}
	;

background:
	ANPERSAND {
		printf("   Yacc: background enabled");
		Command::_currentCommand._background = 1;
	}
	|	
	;
%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif
