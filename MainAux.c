#include <stdio.h>
#include "MainAux.h"
#include "Game.h"
#include "Parser.h"
#include "Solver.h"


/*
 * Asks user for the number of cells to filled and repeats until user enters a valid input,
 * then generates a new board and prints it.
 */
void startNewGame() {
	int		numOfHints;
	char	numOfHints_str[1024] = {'\0'};

	printf("Please enter the number of cells to fill [0-%d]:\n",N*N-1);
	getUserInput(numOfHints_str);
	numOfHints = toInt(numOfHints_str);
	while(numOfHints < 0  || numOfHints > N*N-1) {
		printf("Error: Invalid number of cells to fill (should be between 0 and %d)\n",N*N-1);
		getUserInput(numOfHints_str); /* wait for user input */
		numOfHints = toInt(numOfHints_str);
	}
	initializeBoard();
	generateBoard(numOfHints);
	printBoard();
}


/*
 * Given an interpreted command from the user - executes it and prints the result of the execution.
 * returns TRUE iff command executed successfully.
 *
 * unsigned int*	command		-	The already encoded user command (after interpretation).
 */
unsigned int executeCommand (unsigned int* command){
	unsigned int	gameOver = isGameOver();

	switch(command[0]) {
	case 1: 	/* VALIDATE	*/
		if(gameOver) return 0;
		if(validate()) {
			printf("Validation passed: board is solvable\n");
		}
		else {
			printf("Validation failed: board is unsolvable\n");
		}
		return TRUE;
	case 2: 	/*	RESTART	*/
		startNewGame();
		return TRUE;
	case 3: 	/*	EXIT	*/
		/* nothing to do here. handled in main */
		return TRUE;
	case 4: 	/*	HINT	*/
		if(gameOver) return FALSE;
		printf("Hint: set cell to %d\n",getHint(command[1],command[2]));
		return TRUE;
	case 5:		/*	SET		*/
		if(gameOver) return FALSE;
		if(isCellFixed(command[1],command[2])) {
			printf("Error: cell [%d,%d] is fixed\n",command[1],command[2]);
		}
		else if(!isSetValid(command[1],command[2],command[3])) {
			printf("Error: value is invalid\n");
		}
		else {
			setCellVal(command[1], command[2], command[3]);
			printBoard();
			if(isGameOver()) {
				printf("Puzzle solved successfully\n");
			}
		}
		return TRUE;
	default: /* shouldn't get here */
		return FALSE;
	}/*switch-end*/
}
