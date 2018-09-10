/* declarations */
#include <stdio.h>
#include <stdlib.h>
#include "Solver.h"
#include "FileManager.h"
#include "ILP_Solver.h"



Board			gameBoard;
Board			solutionBoard;



/********** Private method declarations **********/
/* Includes *some* of the private methods in this module */
void printBoard(Board*);

/* 1 */ unsigned int executeSolve(char*);
/* 2 */ unsigned int executeEdit(char*);
/* 3 */ unsigned int executeMarkErrors(int);
/* 4 */ unsigned int executePrintBoard();
/* 5 */ unsigned int executeSet(int,int,int);
/* 6 */ unsigned int executeValidate();
/* 7 */ unsigned int executeGenerate(int,int);
/* 8 */ unsigned int executeUndo();
/* 9 */ unsigned int executeRedo();
/* 10*/ unsigned int executeSave(char*);
/* 11*/ unsigned int executeHint(int,int);
/* 12*/ unsigned int executeNumSolutions();
/* 13*/ unsigned int executeAutofill();
/* 14*/ unsigned int executeReset();
/* 15*/ unsigned int executeExit();

/******* End of private method declarations ******/


/************************* Public methods *************************/

/*
 * Given an interpreted command from the user - executes it and prints the result of the execution.
 * returns TRUE iff command executed successfully.
 *
 * unsigned int*	command		-	The already encoded user command (after interpretation).
 * char*			path		-	A file path (used by edit, solve, and save commands).
 */
unsigned int executeCommand (int* command, char* path){
	switch(command[0]) {
	case 1:		/*  SOLVE   */
		return executeSolve(path);
	case 2:		/* EDIT		*/
		return executeEdit(path);
	case 3:		/* MARK ERRORS */
		return executeMarkErrors(command[1]);
	case 4:		/* PRINT BOARD */
		return executePrintBoard();
	case 5:		/*	SET		*/
		return executeSet(command[2]-1, command[1]-1, command[3]);
	case 6: 	/* VALIDATE	*/
		return executeValidate();
	case 7:		/* GENERATE */
		return executeGenerate(command[1], command[2]);
	case 8:		/* UNDO */
		return executeUndo();
	case 9:		/* REDO */
		return executeRedo();
	case 10:	/* SAVE */
		return executeSave(path);
	case 11: 	/*	HINT	*/
		return executeHint(command[2]-1, command[1]-1);
	case 12:	/* NUM SOLUTIONS */
		return executeNumSolutions();
	case 13:	/* AUTOFILL */
		return executeAutofill();
	case 14:	/* RESET */
		return executeReset();
	case 15: 	/*	EXIT	*/
		return executeExit();

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

	printf("%s",separatorRow);

	for(i = 0; i < n; i++) {
		for(j = 0; j < m; j++) {
			printCellRow(boardPtr, i*m+j);
		}
		printf("%s",separatorRow);
	}

	free(separatorRow);
}


/*
 * Checks if the board is complete and prints a message accordingly.
 * If:		the board is completely filled and does not contain erroneous value - prints "Puzzle solved successfully\n"
 * 			and changes game mode to INIT.
 * Else if:	the board is completely filled but contains erroneous values - prints "Puzzle solution erroneous\n",
 * Otherwise, prints nothing.
 */
void handleBoardCompletion() {
	int boardComplete;
	boardComplete = isBoardComplete(&gameBoard);
	if(boardComplete == TRUE) {
		printf("Puzzle solved successfully\n");
		/* Set game mode to INIT */
		setGameMode(INIT);
	}
	else if(boardComplete == FALSE) { /* board is filled but contains erroneous values */
		printf("Puzzle solution erroneous\n");
	}
	/* else: (boardComplete == -1 -- board is not completely filled) do nothing. */
}

/*
 * Available in all game modes.
 * Tries to load a puzzle from the given path address.
 * If successful, changes game mode to SOLVE, loads the puzzle and prints the board.
 * Otherwise, prints an error message.
 * returns TRUE iff the path string isn't empty (meaning that the command was a valid format).
 *
 * char*	path	-	A file path (might be empty or invalid).
 */
unsigned int executeSolve(char* path) {
	unsigned int loadSuccessful;
	if(path[0] == '\0') { /* No path given */
		return FALSE;
	}
	/* Try to load board from file path, if failed to load - print error. */
	loadSuccessful = loadBoard(&gameBoard, path, SOLVE);
	if(loadSuccessful) {
		setGameMode(SOLVE);
		initializeMoveList();
		initializeBoard(&solutionBoard, gameBoard.m, gameBoard.n);
		printBoard(&gameBoard);
	}
	else {
		printf("Error: File doesn't exist or cannot be opened\n");
	}
	return TRUE;

}


/*
 * Available in all game modes.
 * Tries to either load a game from file (if a path was given), or initializes an empty 9x9 game.
 * If a path was given but an invalid one, prints an error message and returns.
 * Otherwise, changes game mode to EDIT and prints the boards.
 * returns TRUE - always (the command was in the right format - no arguments missing).
 *
 * char*	path	-	A file path (might be empty or invalid).
 *
 */
unsigned int executeEdit(char* path) {
	unsigned int loadSuccessful;
	if(path[0] == '\0') { /* No path given. Generate an empty m=3 n=3 board. */
		setGameMode(EDIT);
		initializeBoard(&gameBoard,3,3);
		initializeBoard(&solutionBoard, gameBoard.m, gameBoard.n);
		initializeMoveList();
	}
	else {
		/* Try to load board from file path, if failed to load - print error. */
		loadSuccessful = loadBoard(&gameBoard, path, EDIT);
		if(loadSuccessful) {
			setGameMode(EDIT);
			initializeBoard(&solutionBoard, gameBoard.m, gameBoard.n);
			initializeMoveList();
		}
		else {
			printf("Error: File cannot be opened\n");
			return TRUE;
		}
	}
	printBoard(&gameBoard);
	return TRUE;
}


/*
 * Available in SOLVE mode only.
 * Changes game mode to either 0 or 1, if given a valid argument. It does not assume the argument is valid though.
 * If the argument is invalid - prints an error message.
 * returns TRUE iff the game mode is SOLVE.
 *
 * int	toMarkErrors	-	The desired value of markErrors - should be 0 or 1. might be invalid.
 */
unsigned int executeMarkErrors(int toMarkErrors) {
	if(getGameMode() != SOLVE) return FALSE;
	if(toMarkErrors != 0 && toMarkErrors != 1) {
		printf("Error: the value should be 0 or 1\n");
		return TRUE;
	}
	setMarkErrors(toMarkErrors);
	return TRUE;
}


/*
 * Available in EDIT and SOLVE modes.
 * Prints the game board.
 * returns TRUE iff the game mode is EDIT or SOLVE.
 */
unsigned int executePrintBoard() {
	if(getGameMode() == INIT) return FALSE;
	printBoard(&gameBoard);
	return TRUE;
}


/*
 * Available in EDIT and SOLVE modes.
 * Tries to set a new value to a cell.
 * The arguments are not guaranteed to be correct, and if they aren't -
 * error messages will be printed and the set will not be executed.
 * If they are correct - Changes cell value, updates erroneous and possible cells, adds the move to the move list,
 * Updates the number of cells displayed, and finally prints the updated board.
 * In SOLVE mode: if the board is completely filled - prints a message accordingly (puzzle solved/erroneous values) and changes game mode to INIT complete.
 * returns TRUE iff the game mode is EDIT or SOLVE.
 *
 * int	row		-	Row number (between 0 and N-1 if valid).
 * int	col		-	Column number (between 0 and N-1 if valid).
 * int	val		-	The value to be set (between 0 and N if valid).
 */
unsigned int executeSet(int row, int col, int val) {
	unsigned int		m = gameBoard.m, n = gameBoard.n;
	int 				N = m*n;
	unsigned int		gameMode = getGameMode();
	unsigned int		lastVal;
	SinglyLinkedList 	*move;

	/* This command is available in Edit or Solve mode only */
	if(gameMode == INIT) return FALSE;

	/* The values of X,Y,Z are not guaranteed to be correct */
	if(row <  0 || col <  0 || val < 0 ||
	   row >= N || col >= N || val > N) {
		printf("Error: value not in range 0-%d\n",N);
	}

	/* Cell is fixed */
	else if(isCellFixed(&gameBoard, row, col)) {
		printf("Error: cell is fixed\n");
	}
	/*  If this is a valid set: */
	else {
		lastVal = getCell(&gameBoard, row, col)->value;
		/* if cell value changes: */
		if(lastVal != (unsigned int)val) {
			/* update cell value and erroneous values */
			setCellVal(&gameBoard, row, col, val);
			updateErroneous(&gameBoard, row, col, lastVal);
			/* Add move to list (for undo/redo) */
			move = createNewSinglyLinkedList();
			singly_addLast(move,row,col,val,lastVal);
			addMove(move);
		}
		/* then print the board either way */
		printBoard(&gameBoard);

		/* Check if the puzzle was solved and print a message and change game mode if needed */
		if(gameMode == SOLVE) {
			handleBoardCompletion();
		}
	}
	return TRUE;
}


/*
 * Available in EDIT and SOLVE modes.
 * Validates game if the game doesn't contain erroneous values.
 * Uses ILP to find out if whether the current game has a solution, and tells it to the player.
 *
 * returns TRUE iff the game mode is EDIT or SOLVE.
 */
unsigned int executeValidate() {
	int isSolvable;
	/* return TRUE iff the command is legal */
	if(getGameMode() == INIT) return FALSE;

	/* The command is legal: */
	if(hasErrors(&gameBoard)) {
		printf("Error: board contains erroneous values\n");
		return TRUE;
	}
	isSolvable = validate(&gameBoard);
	if(isSolvable == -1) { /* Gurobi failure */
		printf("Error: Gurobi failed to validate board. Please try again\n");
	}
	else if(isSolvable) {
		printf("Validation passed: board is solvable\n");
	}
	else {
		printf("Validation failed: board is unsolvable\n");
	}
	return TRUE;
}


/*
 * Available in EDIT mode only.
 * If the arguments are valid (as described below):
 * Generates a puzzle by randomly filling X cells with random legal values,
 * running ILP to solve the resulting board, and then clearing all but Y random cells.
 * Finally prints the board.
 * returns TRUE iff the game mode is EDIT.
 *
 * int	X	-	Number of random legal values (between 0 and N*N if valid).
 * int	Y	-	Number of cells to display on the board (between 0 and N*N if valid).
 *
 */
unsigned int executeGenerate(int X, int Y) {
	int				N = gameBoard.m * gameBoard.n;
	unsigned int	generateSuccessful;
	if(getGameMode() != EDIT) return FALSE;
	/* check x, y to have legal coordinates: */
	if ((X < 0) || (Y < 0) || (X > N*N) || (Y > N*N)){
		printf("Error: value not in range 0-%d\n",N*N);
	    return TRUE;
	}
	/* board must be empty */
	if (!isBoardEmpty(gameBoard)){
		printf("Error: board is not empty\n");
	    return TRUE;
	}
	generateSuccessful = generate(&gameBoard, X, Y);
	if(generateSuccessful) { /* try to generate board. returns TRUE if successful */
		printBoard(&gameBoard);
	}
	else {
		printf("Error: puzzle generator failed\n");
	}
	return TRUE;
}


/*
 * Available in EDIT and SOLVE modes.
 * Undo previous move done by the player.
 * Prints an error message if there are no moves to undo.
 * returns TRUE iff the game mode is EDIT or SOLVE.
 */
unsigned int executeUndo() {
	unsigned int successful;
	if(getGameMode() == INIT) return FALSE;
	successful = undoMove(TRUE);
	if(!successful) {
		printf("Error: no moves to undo\n");
	}
	return TRUE;
}


/*
 * Available in EDIT and SOLVE modes.
 * Redo a move previously done by the player.
 * Prints an error message if there are no moves to redo.
 * returns TRUE iff the game mode is EDIT or SOLVE.
 */
unsigned int executeRedo() {
	unsigned int successful;
	unsigned int gameMode = getGameMode();
	if(gameMode == INIT) return FALSE;
	successful = redoMove();
	if(successful) {
 		/* Check if the board was completely filled and contains erroneous values (other options aren't possible) */
		if(gameMode == SOLVE) {
			handleBoardCompletion();
		}
	}
	else { /* unsuccessful redo */
		printf("Error: no moves to redo\n");
	}
	return TRUE;
}


/*
 * Available in EDIT and SOLVE modes.
 * If given a valid path, saves the current game board to the given path address.
 * In EDIT mode, saves only if the board does not contain erroneous values and the board is solvable, else - prints an error message.
 * Otherwise (invalid path), prints an error message.
 * returns TRUE iff the game mode is SOLVE or EDIT and string path is not empty (a path was given by the player).
 *
 * char*	path	-	A file path (might be empty or invalid).
 */
unsigned int executeSave(char* path) {
	unsigned int gameMode = getGameMode();
	unsigned int solvable;
	unsigned int saveSuccessful;
	if(gameMode == INIT) return FALSE;
	if(path[0] == '\0') { /* no path given */
		return FALSE;
	}
	/* in edit mode - the board has to have a valid solution, thus there cannot be any erroneous cells and also it should pass a validation */
	if(gameMode == EDIT) {
		/* check that the puzzle doesn't have any erroneous cells */
		if(hasErrors(&gameBoard)) {
			printf("Error: board contains erroneous values\n");
			return TRUE;
		}
		/* check that the puzzle has a solution */
		solvable = validate(&gameBoard);
		if(!solvable) {
			printf("Error: board validation failed\n");
			return TRUE;
		}
	}
	/* try to save the puzzle to the given path, and print message indicating the success or failure */
	saveSuccessful = saveBoard(gameBoard, path, gameMode);
	if(saveSuccessful) {
		printf("Saved to: %s\n", path);
	}
	else {
		printf("Error: File cannot be created or modified\n");
	}
	return TRUE;
}


/*
 * Available in SOLVE mode only.
 * Give a hint to the player by showing the solution of a single cell[row,col].
 * A hint will be given only if the arguments are in range, the board doesn't contain any erroneous values,
 * the cell isn't fixed, and doesn't already contain a value. Otherwise - an error message will be printed.
 * returns TRUE iff the game mode is SOLVE.
 *
 * int	row		-	Row number (between 0 and N-1 if valid).
 * int	col		-	Column number (between 0 and N-1 if valid).
 */
unsigned int executeHint(int row, int col) {
	unsigned int	m = gameBoard.m, n = gameBoard.n;
	unsigned int	gameMode 		= getGameMode();
	unsigned int 	N 				= m*n;
	int				isSolvable = TRUE;
	Cell* 			cur_cell;

	/* check gameMode */
	if(gameMode != SOLVE) return FALSE;

	/* check coordinates */
	if(row < 0 || col < 0 ||
	   row >= (int)N || col >= (int)N) {
		printf("Error: value not in range 1-%d\n",N);
		return TRUE;
	}

	/* check errors */
	if(hasErrors(&gameBoard)) {
		printf("Error: board contains erroneous values\n");
		return TRUE;
	}

	/* check if cell is fixed */
	cur_cell = getCell(&gameBoard,row,col);
	if(cur_cell->fixed){
		printf("Error: cell is fixed\n");
		return TRUE;
	}

	/* check if cell already has value */
	if(cur_cell->value != 0){
		printf("Error: cell already contains a value\n");
		return TRUE;
	}

	/* try to solve with ILP */
	isSolvable = ilpSolve(&gameBoard, &solutionBoard);
	if(isSolvable == -1) { /* Gurobi failure */
		printf("Error: Gurobi failure. Please try again\n");
	}
	else if(isSolvable) {
		printf("Hint: set cell to %d\n",getHint(row,col));
	}
	else {
		printf("Error: board is unsolvable\n");
	}
	return TRUE;
}


/*
 * Available in EDIT and SOLVE modes.
 * Prints the number of solutions for the current board, but only if the board does not contain erroneous values,
 * Otherwise, an error message is printed.
 * returns TRUE iff the game mode is EDIT or SOLVE.
 */
unsigned int executeNumSolutions() {
	unsigned int	n; /* number of solutions */
	Board*			boardPtr = &gameBoard;
	if(getGameMode() == INIT) return FALSE;
	/* check if there are erroneous cells*/
	if (hasErrors(boardPtr)){
		printf("Error: board contains erroneous values\n");
		return TRUE;
	}
	/* Calculate the number of solutions using exhaustive backtracking (implemented using a stack) */
	n = numSolutions(boardPtr);
	/* print results */
	printf("Number of solutions: %d\n",n);
	if(n == 1) {
		printf("This is a good board!\n");
	}
	else if(n > 1) {
		printf("The puzzle has more than 1 solution, try to edit it further\n");
	}
	return TRUE;
}


/*
 * Available in SOLVE mode only.
 * Automatically fills "obvious" values -- cells which contain a single legal value.
 * If there are erroneous cells - an error messages will be printed and the autofill will not be executed.
 * If they are correct - Changes cell values, updates erroneous and possible cells, adds the move to the move list,
 * Updates the number of cells displayed, and finally prints the updated board.
 * If the board is completely filled - prints a message accordingly (puzzle solved/erroneous values) and changes game mode to INIT complete.

 * Returns TRUE iff the game mode is SOLVE.
 */
unsigned int executeAutofill() {
	Board*			boardPtr = &gameBoard;
	if(getGameMode() != SOLVE) return FALSE;
	/* check if there are errounous cells*/
	if (hasErrors(boardPtr)){
		printf("Error: board contains erroneous values\n");
		return TRUE;
	}
	/* Execute an autofill and print the board afterwards.  */
	autofill(&gameBoard);
	printBoard(&gameBoard);
	/* Check if the puzzle was solved and print a message and change game mode if needed */
	handleBoardCompletion();
	return TRUE;
}


/*
 * Available in EDIT and SOLVE modes.
 * Undos all modes, reverting the board to its original loaded state.
 * Once the board is reset, the undo/redo list is cleared entirely, and the program prints: "Board reset\n".
 * returns TRUE iff the game mode is EDIT or SOLVE.
 */
unsigned int executeReset() {
	if(getGameMode() == INIT) return FALSE;
	resetGame();
	printf("Board reset\n");
	return TRUE;
}


/*
 * Available in all game modes.
 * Frees all memory resources.
 * This method is followed by a termination of the program in the main module.
 * returns TRUE - always.
 */
unsigned int executeExit() {
	Board* gameBoardPtr = &gameBoard;
	Board* solBoardPtr = &solutionBoard;
	freeBoard(gameBoardPtr);
	freeBoard(solBoardPtr);
	clearMoveList();
	return TRUE;
}


/********************** End of private methods *********************/

