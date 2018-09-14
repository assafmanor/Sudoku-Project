/*	---Main.c---
 * This module contains one function - the Main function of the project:
 * The main function set the game mode to "INIT" (by using "Game.h").
 * Then, it waits for user input. The input is parsed (by using "Parser.h) as a command.
 * If it is a legal command, it is executed (by using "MainAux.h").
 * The function ends when exit command entered as an input
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SPBufferset.h" /* for srand command */
#include "MainAux.h"
#include "Parser.h"
#include "Game.h"

int main() {
	unsigned int    isValidCommand;
	unsigned int	exitProgram = FALSE;
	unsigned int	executedSuccessfully;
	char            input[MAX_INPUT_LENGTH] = {'\0'};
	int				command[4] = { 0 };
	char			path[MAX_INPUT_LENGTH];
	SP_BUFF_SET()
	srand(time(NULL));
	setGameMode(INIT);
	printf("Sudoku\n------\n");
	/* Ask user for commands and execute them until user enters "exit". */
	while(command[0] != 15) { /* While command is not "exit". */
		if(exitProgram) {
			if(command[0] != INVALID) printf("Enter your command:\n"); /* Print added so it would work exactly like an exit command */
			command[0] = 15; /* exit command code */
			isValidCommand = TRUE;
		}
		else {
			getUserInput(input, &exitProgram); /* Ask and Wait for user input */
			path[0] = '\0'; /* Nullify path */
			isValidCommand = interpretCommand(input,command,path); /* Determine if input is valid, and if it is - store it in command array. */
		}
		if(isValidCommand) {
			/* Try to execute command. If not executed print error message */
			executedSuccessfully = executeCommand(command,path);
			if(!executedSuccessfully) {
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
