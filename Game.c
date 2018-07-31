#include <stdio.h>
#include <stdlib.h>
#include "Game.h"



Board			gameBoard;			/* a board storing the game values (the ones shown) */
Board			solutionBoard;		/* a board storing the solved values */
unsigned int	cellsDisplayed;		/* number of cells  displayed on the board - used for game over */
unsigned int	gameMode;			/* current game mode (init / solve / edit). */


/*
 * Initializes the game board's parameters for a new game.
 */
void initializeBoard(Board* boardPtr, unsigned int m, unsigned int n) {
	unsigned int i, j, k;
	unsigned int N = m*n;

	boardPtr->m = m;
	boardPtr->n = n;

	/* Allocate space */
    boardPtr->board = (Cell**)malloc(N * sizeof(Cell*)); 											/* rows */
	if(boardPtr->board == NULL) {
		printf("Error: malloc has failed\n");
		exit(1);
	}
    for (i = 0; i < N; i++) {
    	boardPtr->board[i] = (Cell*)malloc(N * sizeof(Cell));	 									/* Cells */
     	if(boardPtr->board[i] == NULL) {
     		printf("Error: malloc has failed\n");
     		exit(1);
     	}
         for(j = 0; j < N; j++) {
        	 boardPtr->board[i][j].possible_vals = (unsigned int*)malloc(N * sizeof(unsigned int));	/* Possible values */
        		if(boardPtr->board[i][j].possible_vals == NULL) {
        			printf("Error: malloc has failed\n");
        			exit(1);
        		}
         }
    }

    /* set empty values */
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			boardPtr->board[i][j].value = 0;
			boardPtr->board[i][j].fixed = FALSE;
			for(k = 0; k < N; k++) {
				boardPtr->board[i][j].possible_vals[k] = TRUE;
			}
		}
	}

	boardPtr->cellsDisplayed = 0;
}


/*
 * Initializes a new empty game board and a solution board.
 */
void newGame(Board* gBoard, Board* sBoard, unsigned int m, unsigned int n) {
	gameBoard = (*gBoard);
	solutionBoard = (*sBoard);

	initializeBoard(&gameBoard,m,n);
	initializeBoard(&solutionBoard,m,n);
	cellsDisplayed = 0;
}


/* Frees allocated space used by a board */
void freeBoard(Board board) {
	unsigned int i, j;
	unsigned int N = board.m*board.n;
	/* free allocated possible_vals for each cell, and free rows */
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
	        free(board.board[i][j].possible_vals);
		}
		/* free allocated row */
		free(board.board[i]);
	}

	/* free the board itself */
	free(board.board);
}


/*
 * Returns a pointer to the specified cell.
 *
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int col		-	Column number (between 0 and N-1).
 */
Cell* getCell(Board* boardPtr, unsigned int row, unsigned int col) {
	return &(boardPtr->board[row][col]);
}


/*
 * Given a column and row of a cell - returns the solution value of the cell.
 *
 * unsigned int	col		-	Column number (between 1 and N).
 * unsigned int row		-	Row number (between 1 and N).
 */
unsigned int getHint(unsigned int col, unsigned int row) {
	return solutionBoard.board[row-1][col-1].value;
}


/*
 * Given a column and row of a cell - return TRUE iff cell is fixed (given as a hint in the beginning of a new game).
 *
 * unsigned int	col		-	Column number (between 1 and N).
 * unsigned int row		-	Row number (between 1 and N).
 */
unsigned int isCellFixed(unsigned int col, unsigned int row) {
	return gameBoard.board[row-1][col-1].fixed;
}


/*
 * Given a column, row, and value - returns TRUE iff value is 0 or it is possible by the constrains of
 * the cells from the same row, column, or block.
 *
 * unsigned int	col		-	Column number (between 1 and N).
 * unsigned int	row		-	Row number (between 1 and N).
 * unsigned int	val		-	The value the user wants to assign to the cell. (Between 0 and N).
 *
 */
unsigned int isSetValid(unsigned int col, unsigned int row, unsigned int val) {
	if(val == 0) {
		return TRUE;
	}
	return gameBoard.board[row-1][col-1].possible_vals[val-1]; /* returns TRUE iff val is a possible value for this cell */
}

/*
 * Returns TRUE iff all cells are filled.
 */
unsigned int isBoardComplete(Board board) {
	unsigned int N = board.m*board.n;
	return board.cellsDisplayed == N*N;
}


/*
 * Given a row a column and a new valueupdates the possible values for the row,
 * column, and block.
 *
 * unsigned int	col		-	Column number (between 0 and N-1).
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int	val		-	The value the user assigned to the cell. (Between 0 and N).
 */
void updatePossibleValues(Board* boardPtr, unsigned int row, unsigned int col, unsigned int val) {
	unsigned int	i, j, count_i, count_j;
	unsigned int	m = boardPtr->m, n = boardPtr->n;
	unsigned int	N = m*n;
	unsigned int	lastVal = boardPtr->board[row][col].value;	/* remember the last value of cell */

	/* Row */
	for(j = 0; j < N; j++) {
		if(val > 0) {
			boardPtr->board[row][j].possible_vals[val-1] = FALSE; /* set new val as invalid for the whole row. */
		}
		if(lastVal > 0) {
			boardPtr->board[row][j].possible_vals[lastVal-1] = TRUE; /* set last val as valid for the whole column. */
		}

	}


	/* Column */
	for(i = 0; i < N; i++) {
		if(val > 0) {
			boardPtr->board[i][col].possible_vals[val-1] = FALSE;
		}
		if(lastVal > 0) {
			boardPtr->board[i][col].possible_vals[lastVal-1] = TRUE;
		}
	}

	/* Block */
	i = m*((row)/m); /* Index of the first row of the block */
	j = n*((col)/n); /* Index of the first column of the block */
	for(count_i = 0; count_i < m; count_i++) {
		for(count_j = 0; count_j < n; count_j++) {
			if(val > 0) {
				boardPtr->board[i+count_i][j+count_j].possible_vals[val-1] = FALSE;
			}
			if(lastVal > 0) {
				boardPtr->board[i+count_i][j+count_j].possible_vals[lastVal-1] = TRUE;
			}
		}
	}

	/* Make 'changing' value to be the same as before possible. */
	if(val > 0) {
		boardPtr->board[row][col].possible_vals[val-1] = TRUE;
	}
}


/*
 * Assigns the value of val to cell[row-1][col-1].value (assuming value is possible),
 * updates the possible values of all the cells in the row, column and block,
 *
 * Cell**		board	-	A game board.
 * unsigned int	col		-	Column number (between 1 and N).
 * unsigned int	row		-	Row number (between 1 and N).
 * unsigned int	val		-	The value being assigned to the cell. (Between 0 and N).
 */
void setCellVal(Board* boardPtr, unsigned int col, unsigned int row, unsigned int val) {
	unsigned int	lastVal = boardPtr->board[row-1][col-1].value;	/* remember the last value */
	if(val == lastVal) { /* if you want to change the value to be the same as before - there's nothing to do */
		return;
	}
	updatePossibleValues(boardPtr, row-1, col-1, val);
	boardPtr->board[row-1][col-1].value = val; 			/* update to new value */

	/* update the number of cells displayed (used for checking if game over) */
	if(val > 0 && lastVal == 0) {
		boardPtr->cellsDisplayed++;
	}
	if(val == 0 && lastVal > 0) {
		boardPtr->cellsDisplayed--;
	}
}

void setGameMode(unsigned int newGameMode) {
	gameMode = newGameMode;
}

unsigned int getGameMode() {
	return gameMode;
}


/*
 * Copies the contents of the original board to the copied board.
 * Assumes both boards are of equal sizes.
 *
 * Cell**	original	-	Original board.
 * Cell**	copied		-	Copied board.
 * char*	N			-	Number of rows/columns in both boards.
 */
void copyBoard(Board* original, Board* copy) {
	unsigned int	m = original->m, n = original->n;
	unsigned int	N = m*n;
	unsigned int i, j, k;
	Cell* copyCell;
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			copyCell = getCell(copy,i,j);
			copyCell->fixed = getCell(original,i,j)->fixed;
			copyCell->value = getCell(original,i,j)->value;
			for(k = 0; k < N; k++) {
				getCell(copy,i,j)->possible_vals[k] = getCell(original,i,j)->possible_vals[k];
			}
		}

	}
	copy->m = m;
	copy->n = n;
	copy->cellsDisplayed = original->cellsDisplayed;
}


Board* getGameBoardPtr() {
	return &gameBoard;
}


Board* getSolutionBoardPtr() {
	return &solutionBoard;
}
