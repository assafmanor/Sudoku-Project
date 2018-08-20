#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SPBufferset.h" /* for srand command */
#include "MainAux.h"
#include "Parser.h"
#include "Game.h"
/*
 * The main function.
 */
int main() {
	unsigned int    isValidCommand;
	char            input[MAX_INPUT_LENGTH] = {'\0'};
	int				command[4] = { 0 };
	char			path[MAX_INPUT_LENGTH];

	SP_BUFF_SET()

	srand(time(NULL));

	setGameMode(INIT);

	printf("Sudoku\n------\n");

	/* Ask user for commands and execute them until user enters "exit". */
	while(command[0] != 15) { /* While command is not "exit". */
		getUserInput(input); /* Ask and Wait for user input */
		path[0] = '\0'; /* Nullify path */
		if(input[0]== '\0') {
			command[0] = 15;
			isValidCommand = TRUE;
		}
		else {
			isValidCommand = interpretCommand(input,command,path); /* Determine if input is valid, and if it is - store it in command array. */
		}
		if(isValidCommand) {
			if(!executeCommand(command,path)) { /* Try to execute command. If not executed print error message */
				printf("ERROR: invalid command\n");
			}
		}
		else {
			printf("ERROR: invalid command\n");
		}
	}
	printf("Exiting...\n");

	return 0;
}
