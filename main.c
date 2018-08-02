#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SPBufferset.h"
#include "MainAux.h"
#include "Parser.h"
#include "Game.h"

/* ITS WORKING!!!*/
/*
 * The main function.
 *
 * Gets seed used for pseudo-randomization as an argument from command line.
 */
int main() {
	unsigned int    isValidCommand;
	char            input[MAX_INPUT_LENGTH] = {'\0'};
	int				command[4] = { 0 };
	char			path[MAX_INPUT_LENGTH];

	SP_BUFF_SET()

	srand(time(NULL));

	initGame();

	printf("Sudoku\n------\n");

	/* Ask user for commands and execute them until user enters "exit". */
	while(command[0] != 15) { /* While command is not "exit". */
		getUserInput(input); /* Ask and Wait for user input */
		path[0] = '\0'; /* Nullify path */
		isValidCommand = interpretCommand(input,command,path); /* Determine if input is valid, and if it is - store it in command array. */
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
