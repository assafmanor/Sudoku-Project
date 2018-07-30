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

	setGameMode(INIT);
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
	unsigned int	gameMode = getGameMode();

	switch(command[0]) {
	case 1:		/*  SOLVE   */
		setGameMode(SOLVE);
		printf("Solve mode activated\n"); /* TEMPORARY PRINT */
		/* in the future - we should also get file path and open it */
		return TRUE;
	case 2:		/* EDIT		*/
		setGameMode(EDIT);
		printf("Edit mode activated\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 3:		/* MARK ERRORS */
		if(gameMode != SOLVE) return FALSE;
		printf("Will mark errors,\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 4:		/* PRINT BOARD */
		if(gameMode == INIT) return FALSE;
		printBoard();
		return TRUE;
	case 5:		/*	SET		*/
		if(gameMode == INIT) return FALSE;
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
	case 6: 	/* VALIDATE	*/
		if(gameMode == INIT) return FALSE;
		if(gameOver) return 0;
		if(validate()) {
			printf("Validation passed: board is solvable\n");
		}
		else {
			printf("Validation failed: board is unsolvable\n");
		}
		return TRUE;
	case 7:		/* GENERATE */
		if(gameMode != EDIT) return FALSE;
		printf("Will generate a new board.\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 8:		/* UNDO */
		if(gameMode == INIT) return FALSE;
		printf("Will undo move.\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 9:		/* REDO */
		if(gameMode == INIT) return FALSE;
		printf("Will redo move.\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 10:	/* SAVE */
		if(gameMode == INIT) return FALSE;
		printf("Will save current game.\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 11: 	/*	HINT	*/
		if(gameMode != SOLVE) return FALSE;
		if(gameOver) return FALSE;
		printf("Hint: set cell to %d\n",getHint(command[1],command[2]));
		return TRUE;
	case 12:	/* NUM SOLUTIONS */
		if(gameMode == INIT) return FALSE;
		printf("Will display the number of possible Solutions.\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 13:	/* AUTOFILL */
		if(gameMode != SOLVE) return FALSE;
		printf("You asked for an autofill.\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 14:	/* RESET */
		if(gameMode == INIT) return FALSE;
		printf("You asked to reset game.\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 15: 	/*	EXIT	*/
		/* nothing to do here. handled in main */
		return TRUE;
	case 16: 	/*	GAME MOD;	*/
		printf("Game mode: %s.\n",getGameMode()==INIT ? "INIT":(getGameMode()==SOLVE ? "SOLVE":"EDIT"));
		return TRUE;
	default: /* shouldn't get here */
		return FALSE;
	}/*switch-end*/
}
