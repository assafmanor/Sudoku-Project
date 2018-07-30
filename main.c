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
int main(int argc, char* argv[]) {
	unsigned int    isValidCommand;
	char            input[1024] = {'\0'};
	unsigned int	command[4] = { 0 };
	unsigned int    seed;

	SP_BUFF_SET()
	if(argc > 1) { /* Get seed entered in command line */
		seed = toInt(argv[1]); 	/* Convert seed to integer - assuming valid input */
		srand(seed);
	}
	else {						/* No seed given. get seed from current time */
		srand(time(NULL));
	}

	printf("Sudoku\n------\n");

	startNewGame(); /* Ask user for the number of filled cells, generate the board and print it. */

	/* Ask user for commands and execute them until user enters "exit". */
	while(command[0] != 15) { /* While command is not "exit". */
		getUserInput(input); /* Wait for user input */
		isValidCommand = interpretCommand(input,command); /* Determine if input is valid, and if it is - store it in command array. */
		if(isValidCommand) {
			if(!executeCommand(command)) { /* Try to execute command. If not executed print error message */
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
