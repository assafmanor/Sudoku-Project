/* declarations */
#include <stdio.h>
#include <stdlib.h>
#include "MainAux.h"
/*#include "Game.h"*/
#include "Parser.h"
#include "Solver.h"
#include "FileManager.h"
#include "LinkedList.h"



Board			gameBoard;
Board			solutionBoard;



/********** Private method declarations **********/
/* Includes *some* of the private methods in this module */
void printBoard(Board*);

/* 1 */ unsigned int executeSolve(char*);
/* 2 */ unsigned int executeEdit(char*);
/* 3 */ unsigned int executeMarkErrors(int*);
/* 4 */ unsigned int executePrintBoard();
/* 5 */ unsigned int executeSet(int*);
/* 6 */ unsigned int executeValidate();
/* 7 */ unsigned int executeGenerate();
/* 8 */ unsigned int executeUndo();
/* 9 */ unsigned int executeRedo();
/* 10*/ unsigned int executeSave(char*);
/* 11*/ unsigned int executeHint(int*);
/* 12*/ unsigned int executeNumSolutions();
/* 13*/ unsigned int executeAutofill();
/* 14*/ unsigned int executeReset();
/* 15*/ unsigned int executeExit();

/*//////////// TEMPORARY ///////////*/
/*16*/ unsigned int executeCreate(int*);

/******* End of private method declarations ******/


/************************* Public methods *************************/

/*////////////////////// TEMPORARY //////////////////////*/
void startNewGame(unsigned int m, unsigned int n, unsigned int numOfHints) {

	/* initialize gameBoard and solutionBoard */
	initializeGame(&gameBoard, &solutionBoard, m, n);
	initializeMoveList();
	generateBoard(&gameBoard, &solutionBoard, numOfHints);
	printBoard(&gameBoard);
}


/*
 * Given an interpreted command from the user - executes it and prints the result of the execution.
 * returns TRUE iff command executed successfully.
 *
 * unsigned int*	command		-	The already encoded user command (after interpretation).
 * char*			path		-	An file path (used by edit, solve, and save commands).
 */
unsigned int executeCommand (int* command, char* path){
	switch(command[0]) {
	case 1:		/*  SOLVE   */
		return executeSolve(path);
	case 2:		/* EDIT		*/
		return executeEdit(path);
	case 3:		/* MARK ERRORS */
		return executeMarkErrors(command);
	case 4:		/* PRINT BOARD */
		return executePrintBoard();
	case 5:		/*	SET		*/
		return executeSet(command);
	case 6: 	/* VALIDATE	*/
		return executeValidate();
	case 7:		/* GENERATE */
		return executeGenerate();
	case 8:		/* UNDO */
		return executeUndo();
	case 9:		/* REDO */
		return executeRedo();
	case 10:	/* SAVE */
		return executeSave(path);
	case 11: 	/*	HINT	*/
		return executeHint(command);
	case 12:	/* NUM SOLUTIONS */
		return executeNumSolutions();
	case 13:	/* AUTOFILL */
		return executeAutofill();
	case 14:	/* RESET */
		return executeReset();
	case 15: 	/*	EXIT	*/
		return executeExit();

	/* /////////// TEMPORARY /////////////// */
	case 16:		/* CREATE		*/
		return executeCreate(command);
	default: /* shouldn't get here */
		return FALSE;
	}/*switch-end*/
}

/********************** End of public methods *********************/



/************************* Private methods *************************/

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


/*
 * Prints a specific row of boardPtr->board.
 *
 * Board*		boardPtr	-	A pointer to a game board.
 * unsigned int row			-	The row to be printed.
 */
void printCellRow(Board* boardPtr, unsigned int row) {
	unsigned int	m = boardPtr->m, n = boardPtr->n;
	unsigned int	col;
	unsigned int	block;
	Cell*			cell;
	printf("|");
	/* For each block row */
	for(block = 0; block < m; block++) {
		/* For each block column: */
		for(col = block*n; col < block*n + n; col++) {
			cell = getCell(boardPtr,row,col);
			printf(" ");
			if(cell->value == 0){
				printf("  ");
			}
			else {
				printf("%2d", cell->value);
			}
			if(cell->fixed) {
				printf(".");
			}
			else if((getGameMode() == EDIT || getMarkErrors()) && cell->isErroneous) {
				printf("*");
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
 *
 * Board*	boardPtr	-	A pointer to a game board.
 */
void printBoard(Board* boardPtr) {
	unsigned int		m = boardPtr->m, n = boardPtr->n;
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
			printCellRow(boardPtr, i*m+j);
		}
		printf("%s",separatorRow);
	}

	free(separatorRow);
}



unsigned int executeSolve(char* path) {
	setGameMode(SOLVE);
	if(path[0] == '\0') { /* No path given */
		return FALSE;
	}

	/* Try to load board from file path, if failed to load - print error. */
	if(loadBoard(&gameBoard, path, SOLVE)) {
		initializeMoveList();
		initializeBoard(&solutionBoard, gameBoard.m, gameBoard.n);
		if(!hasErrors(&gameBoard)) { /* If board has erroneous values, then the board has no valid solution */
			updateSolBoard(&gameBoard, &solutionBoard);
		}
		printBoard(&gameBoard);
	}
	else {
		printf("Error: File doesn't exist or cannot be opened\n");
	}
	return TRUE;

}


unsigned int executeEdit(char* path) {
	setGameMode(EDIT);
	if(path[0] == '\0') { /* No path given. Generate an empty m=3 n=3 board. */
		startNewGame(3,3,0);
		initializeMoveList();
	}
	else {
		if(loadBoard(&gameBoard, path, EDIT)) {
			printBoard(&gameBoard);
			initializeBoard(&solutionBoard, gameBoard.m, gameBoard.n);
			initializeMoveList();
		}
		else {
			printf("Error: File cannot be opened\n");
		}
	}
	return TRUE;
}


unsigned int executeMarkErrors(int* command) {
	unsigned int	gameMode = getGameMode();
	if(gameMode != SOLVE) return FALSE;
	if(command[1] != 0 && command[1] != 1) {
		printf("Error: the value should be 0 or 1\n");
		return TRUE;
	}
	setMarkErrors(command[1]);
	return TRUE;
}


unsigned int executePrintBoard() {

	if(getGameMode() == INIT) return FALSE;
	printBoard(&gameBoard);
	return TRUE;
}


unsigned int executeSet(int* command) {
	unsigned int		m = gameBoard.m, n = gameBoard.n;
	unsigned int		gameMode = getGameMode();
	unsigned int 		N = m*n;
	unsigned int		lastVal;
	int					row, col, val;

	SinglyLinkedList 	*move;

	col = command[1]-1;
	row = command[2]-1;
	val = command[3];

	if(gameMode == INIT) return FALSE;
	if(row < 0 || col < 0 || val < 0 ||
	   row >= (int)N || col >= (int)N || val > (int)N) {
		printf("Error: value not in range 0-%d\n",N);
	}
	else if(isCellFixed(&gameBoard, row, col)) {
		printf("Error: cell is fixed\n");
	}
	else {
		lastVal = getCell(&gameBoard, row, col)->value;

		/* Add move to list (for undo/redo) */
		move = createNewSinglyLinkedList();
		singly_addLast(move,row,col,val,lastVal);
		addMove(move);

		setCellVal(&gameBoard, row, col, val);
		updateErroneous(&gameBoard, row, col, lastVal);
		printBoard(&gameBoard);
		if(!hasErrors(&gameBoard) && isBoardComplete(gameBoard) && gameMode == SOLVE) {
			printf("Puzzle solved successfully\n");
			/* Set game mode to INIT */
			setGameMode(INIT);
		}
	}
	return TRUE;
}


unsigned int executeValidate() {
	if(getGameMode() == INIT) return FALSE;
	if(hasErrors(&gameBoard)) {
		printf("Error: board contains erroneous values\n");
		return TRUE;
	}
	if(validate(&gameBoard)) {
		printf("Validation passed: board is solvable\n");
	}
	else {
		printf("Validation failed: board is unsolvable\n");
	}
	return TRUE;
}


unsigned int executeGenerate() {
	if(getGameMode() != EDIT) return FALSE;
	printf("Will generate a new board.\n"); /* TEMPORARY PRINT */
	return TRUE;
}


unsigned int executeUndo() {
	unsigned int successful;
	if(getGameMode() == INIT) return FALSE;
	successful = undoMove(TRUE);
	if(!successful) {
		printf("Error: no moves to undo\n");
	}
	return TRUE;
}


unsigned int executeRedo() {
	unsigned int successful;
	if(getGameMode() == INIT) return FALSE;
	successful = redoMove();
	if(!successful) {
		printf("Error: no moves to redo\n");
	}
	return TRUE;
}


unsigned int executeSave(char* path) {
	unsigned int gameMode = getGameMode();
	if(gameMode == INIT) return FALSE;
	if(gameMode == EDIT && hasErrors(&gameBoard)) {
		printf("Error: board contains erroneous values\n");
		return TRUE;
	}
	if(path[0] == '\0') { /* No path given */
		return FALSE;
	}
	if(gameMode == EDIT && !validate(&gameBoard)) {
		printf("Error: board validation failed\n");
		return TRUE;
	}
	saveBoard(gameBoard, path, gameMode);
	printf("Saved to: %s\n", path);
	return TRUE;
}


unsigned int executeHint(int* command) {
	unsigned int	m = gameBoard.m, n = gameBoard.n;
	unsigned int	gameMode = getGameMode();
	unsigned int 	N = m*n;
	int				row, col;

	col = command[1]-1;
	row = command[2]-1;

	if(gameMode != SOLVE) return FALSE;
	if(row < 0 || col < 0 ||
	   row >= (int)N || col >= (int)N) {
		printf("Error: value not in range 1-%d\n",N);
		return TRUE;
	}
	if(hasErrors(&gameBoard)) {
		printf("Error: board contains erroneous values\n");
		return TRUE;
	}
	if(getHint(row,col) == 0) { /* solutionBoard not solved */
		updateSolBoard(&gameBoard, &solutionBoard);
	}
	printf("Hint: set cell to %d\n",getHint(row,col));
	return TRUE;

}


unsigned int executeNumSolutions() {
	if(getGameMode() == INIT) return FALSE;
	num_solutions(&gameBoard);
	return TRUE;
}


unsigned int executeAutofill() {
	if(getGameMode() != SOLVE) return FALSE;
	autofill(&gameBoard);
	printBoard(&gameBoard);
	if(isBoardComplete(gameBoard)) {
		printf("Puzzle solved successfully\n");
		/* Set game mode to INIT */
		setGameMode(INIT);
	}
	return TRUE;
}


unsigned int executeReset() {
	if(getGameMode() == INIT) return FALSE;
	resetGame();
	printf("Board reset\n");
	return TRUE;
}


unsigned int executeExit() {
	freeBoard(&gameBoard);
	freeBoard(&solutionBoard);
	clearMoveList();
	return TRUE;
}


unsigned int executeCreate(int* command) {
	setGameMode(EDIT);
	/*  ///////////////////////// TEMPORARY IF ///////////////////////// */
	if(command[1] <= 0 || command[1] > 5 ||
	   command[2] <= 0 || command[2] > 5 ||
	   command[3] < 0 || command[3] > (command[1]*command[2])*(command[1]*command[2])) {
		printf("Invalid board parameters.\nPlease try again.\n\tCommand format: \"create X Y Z\"; where X = m, Y = n, Z = numOfHints.\n");
		printf("\tcommand[1]=%d\n\tcommand[2]=%d\n\tcommand[3]=%d\n",command[1],command[2],command[3]);
	}
	else {
		startNewGame(command[1],command[2],command[3]);
	}
	return TRUE;
}


/********************** End of private methods *********************/

