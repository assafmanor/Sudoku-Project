#include <stdio.h>
#include <stdlib.h>
#include "MainAux.h"
/*#include "Game.h"*/
#include "Parser.h"
#include "Solver.h"



/*TODO: Change to solveBoard and editBoard such that edit has no fixed cells. */
Board			gameBoard;
Board			solutionBoard;


/*
 * Asks user for the number of cells to filled and repeats until user enters a valid input,
 * then generates a new board and prints it.
 */
void initGame() {
	setGameMode(INIT);
}

void startNewGame(unsigned int m, unsigned int n, unsigned int numOfHints) {

	/* Free gameBoard and solutionBoard (they may have been allocated space before) */
	freeBoard(gameBoard);
	freeBoard(solutionBoard);

	newGame(&gameBoard, &solutionBoard, m, n);
	generateBoard(&gameBoard, &solutionBoard, numOfHints);
	printBoard(gameBoard);

}



/*
 * Given an interpreted command from the user - executes it and prints the result of the execution.
 * returns TRUE iff command executed successfully.
 *
 * unsigned int*	command		-	The already encoded user command (after interpretation).
 */
unsigned int executeCommand (int* command){
	unsigned int	m = gameBoard.m, n = gameBoard.n;
/*	unsigned int	gameOver = isBoardComplete();*/
	unsigned int	gameMode = getGameMode();
	unsigned int 	N = m*n;

	switch(command[0]) {
	case 1:		/*  SOLVE   */
/*		if(gameOver) return FALSE;*/
		setGameMode(SOLVE);
		printf("Solve mode activated\n"); /* TEMPORARY PRINT */
		/* in the future - we should also get file path and open it */
		return TRUE;
	case 2:		/* EDIT		*/
/*		if(gameOver) return FALSE;*/
		setGameMode(EDIT);
		printf("Edit mode activated\n"); /* TEMPORARY PRINT */
		/*  ///////////////////////// TEMPORARY IF ///////////////////////// */
		if(command[1] <= 0 || command[1] > 5 ||
		   command[2] <= 0 || command[2] > 5 ||
		   command[3] < 0 || command[3] > (command[1]*command[2])*(command[1]*command[2])) {
			printf("Invalid board parameters.\nPlease try again.\n\tCommand format: \"edit X Y Z\"; where X = m, Y = n, Z = numOfHints.\n");
		}
		else {
			startNewGame(command[1],command[2],command[3]);
		}
		return TRUE;
	case 3:		/* MARK ERRORS */
		if(gameMode != SOLVE) return FALSE;
		printf("Will mark errors,\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 4:		/* PRINT BOARD */
		if(gameMode == INIT) return FALSE;
		printBoard(gameBoard);
		return TRUE;
	case 5:		/*	SET		*/
		if(gameMode == INIT) return FALSE;
		if(command[1] < 0 || command[2] < 0 || command[3] < 0 ||
		   command[1] > (int)N || command[2] > (int)N || command[3] > (int)N) {
			printf("Error: value not in range 0-%d\n",N);
		}
		else if(isCellFixed(command[1],command[2])) {
			printf("Error: cell is fixed\n");
		}
		else {
			if(!isSetValid(command[1],command[2],command[3])) {
				/* TODO: Mark cell as erroneous */
			}
			setCellVal(&gameBoard, command[1], command[2], command[3]);
			printBoard(gameBoard);
			if(isBoardComplete(gameBoard) && gameMode == SOLVE) {
				printf("Puzzle solved successfully\n");
			}
		}
		return TRUE;
	case 6: 	/* VALIDATE	*/
		if(gameMode == INIT) return FALSE;
		if(validate(&gameBoard)) {
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
		if(command[1] < 1 || command[2] < 1 ||
		   command[1] > (int)N || command[2] > (int)N) {
			printf("Error: value not in range 1-%d\n",N);
			return TRUE;
		}
		printf("Hint: set cell to %d\n",getHint(command[1],command[2]));
		return TRUE;
	case 12:	/* NUM SOLUTIONS */
		if(gameMode == INIT) return FALSE;
		printf("Will display the number of possible Solutions.\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 13:	/* AUTOFILL */
		if(gameMode != SOLVE) return FALSE;
		/*autofill();*/
		printf("You asked for an autofill.\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 14:	/* RESET */
		if(gameMode == INIT) return FALSE;
		printf("You asked to reset game.\n"); /* TEMPORARY PRINT */
		return TRUE;
	case 15: 	/*	EXIT	*/
		freeBoard(gameBoard);
		freeBoard(solutionBoard);
		return TRUE;

	/* /////////// TEMPORARY /////////////// */
	case 16: 	/*	GAME MOD;	*/
		printf("Game mode: %s.\n",getGameMode()==INIT ? "INIT":(getGameMode()==SOLVE ? "SOLVE":"EDIT"));
		return TRUE;
	default: /* shouldn't get here */
		return FALSE;
	}/*switch-end*/
}


/*
 * Repeats the character c n times in string out.
 * Used for making the separator row for printing the board.
 *
 * char	c			-	The character to be repeated.
 * unsigned int n	-	The number of times the character will be repeated.
 * char* out		-	The string that the outcome will be saved to.
 */
void repeatChar(char c, unsigned int n, char* out) {
	unsigned int	i;

	for(i = 0; i < n; i++) {
		out[i] = c;
	}
	out[n] = '\n';
}


void printCellRow(Board board, unsigned int row) {
	unsigned int m = board.m, n = board.n;
	unsigned int col;
	unsigned int block;
	printf("|");
	/* For each block row */
	for(block = 0; block < m; block++) {
		/* For each block column: */
		for(col = block*n; col < block*n + n; col++) {
			printf(" ");
			if(gameBoard.board[row][col].value == 0){
				printf("  ");
			}
			else {
				printf("%2d", gameBoard.board[row][col].value);
			}
			if(gameBoard.board[row][col].fixed) {
				printf(".");
			}
			/* else if(markErrors == TRUE && gameBoard[row][col].error) {
			 * 	printf("*");
			 * } */
			else {
				printf(" ");
			}
		}
		printf("|");
	}
	printf("\n");
}


/*
 * Prints the game board.
 */
void printBoard(Board board) {
	unsigned int		m = board.m, n = board.n;
	unsigned int		N = m*n;
	char*				separatorRow;
	unsigned int		i,j;
	static unsigned int	sepSize;

	sepSize = 4*N + m + 1;
	separatorRow = (char*) malloc((sepSize+2)*sizeof(char)); /* allocate enough space for the number of '-' characters needed + '\n' + '\0'. */
	if(separatorRow == NULL) {
		printf("Error: malloc has failed\n");
		exit(1);
	}
	separatorRow[sepSize+1] = '\0';
	repeatChar('-',sepSize,separatorRow);

	/* NEW BOARD PRINT FORMAT */
	printf("%s",separatorRow);

	for(i = 0; i < n; i++) {
		for(j = 0; j < m; j++) {
			printCellRow(board, i*m+j);
		}
		printf("%s",separatorRow);
	}

	free(separatorRow);
}
