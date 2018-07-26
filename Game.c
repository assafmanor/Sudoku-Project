#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include "Solver.h"

/* TRUE,FALSE, N, BLOCK_ROWS, and BLOCK_COLS are defined in Game.h. */



Cell			gameBoard[N][N];	/* a Sudoku board with N rows and N columns */
unsigned int	numOfFixedCells;	/* number of fixed cells */
unsigned int	cellsDisplayed;		/* number of cells  displayed on the board - used for game over */



/*
 * Initializes the game board's parameters for a new game.
 */
void initializeBoard() {
	unsigned int	i, j, count;

	cellsDisplayed = 0;
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			gameBoard[i][j].value = 0;
			gameBoard[i][j].fixed = FALSE;
			gameBoard[i][j].sug_val = 0;
			for(count = 0; count < N; count++) {
				gameBoard[i][j].possible_vals[count] = TRUE;
			}
		}
	}
}


/*
 * Returns a pointer to the specified cell.
 *
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int col		-	Column number (between 0 and N-1).
 */
Cell* getCell(unsigned int row, unsigned int col) {
	return &gameBoard[row][col];
}


/*
 * Given a column and row of a cell - returns the solution value of the cell.
 *
 * unsigned int	col		-	Column number (between 1 and N).
 * unsigned int row		-	Row number (between 1 and N).
 */
unsigned int getHint(unsigned int col, unsigned int row) {
	return gameBoard[row-1][col-1].sol_val;
}


/*
 * Given a column and row of a cell - return TRUE iff cell is fixed (given as a hint in the beginning of a new game).
 *
 * unsigned int	col		-	Column number (between 1 and N).
 * unsigned int row		-	Row number (between 1 and N).
 */
unsigned int isCellFixed(unsigned int col, unsigned int row) {
	return gameBoard[row-1][col-1].fixed;
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
	return gameBoard[row-1][col-1].possible_vals[val-1]; /* returns TRUE iff val is a possible value for this cell */
}

/*
 * Returns TRUE iff all cells are filled.
 */
unsigned int isGameOver() {
	return cellsDisplayed == N*N;
}


/*
 * Given a row a column and a new value, updates the possible values for the row,
 * column, and block.
 *
 * unsigned int	col		-	Column number (between 0 and N-1).
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int	val		-	The value the user assigned to the cell. (Between 0 and N).
 */
void updatePossibleValues(unsigned int row, unsigned int col, unsigned int val) {
	unsigned int	i, j, count_i, count_j;
	unsigned int	lastVal = gameBoard[row][col].value;	/* remember the last value of cell */

	/* Row */
	for(j = 0; j < N; j++) {
		if(val > 0) {
			gameBoard[row][j].possible_vals[val-1] = FALSE; /* set new val as invalid for the whole row. */
		}
		if(lastVal > 0) {
			gameBoard[row][j].possible_vals[lastVal-1] = TRUE; /* set last val as valid for the whole column. */
		}

	}


	/* Column */
	for(i = 0; i < N; i++) {
		if(val > 0) {
			gameBoard[i][col].possible_vals[val-1] = FALSE;
		}
		if(lastVal > 0) {
			gameBoard[i][col].possible_vals[lastVal-1] = TRUE;
		}
	}

	/* Block */
	i = BLOCK_ROWS*((row)/BLOCK_ROWS); /* Index of the first row of the block */
	j = BLOCK_COLS*((col)/BLOCK_COLS); /* Index of the first column of the block */
	for(count_i = 0; count_i < BLOCK_ROWS; count_i++) {
		for(count_j = 0; count_j < BLOCK_COLS; count_j++) {
			if(val > 0) {
				gameBoard[i+count_i][j+count_j].possible_vals[val-1] = FALSE;
			}
			if(lastVal > 0) {
				gameBoard[i+count_i][j+count_j].possible_vals[lastVal-1] = TRUE;
			}
		}
	}

	/* Make 'changing' value to be the same as before possible. */
	if(val > 0) {
		gameBoard[row][col].possible_vals[val-1] = TRUE;
	}
}


/*
 * Assigns the value of val to cell[row-1][col-1].value (assuming value is possible),
 * updates the possible values of all the cells in the row, column and block,
 * and updates the number of cells displayed on the board.
 *
 * unsigned int	col		-	Column number (between 1 and N).
 * unsigned int	row		-	Row number (between 1 and N).
 * unsigned int	val		-	The value being assigned to the cell. (Between 0 and N).
 */
void setCellVal(unsigned int col, unsigned int row, unsigned int val) {
	unsigned int	lastVal = gameBoard[row-1][col-1].value;	/* remember the last value */
	if(val == lastVal) { /* if you want to change the value to be the same as before - there's nothing to do */
		return;
	}
	/* update the number of cells displayed (used for checking if game over) */
	if(val > 0 && lastVal == 0) {
		cellsDisplayed++;
	}
	if(val == 0 && lastVal > 0) {
		cellsDisplayed--;
	}

	updatePossibleValues(row-1, col-1, val);
	gameBoard[row-1][col-1].value = val; 			/* update to new value */

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


/*
 * Prints the game board.
 */
void printBoard() {
	char*				separatorRow;
	unsigned int		i, row = 0, col = 0;
	static unsigned int	sepSize;
	unsigned int		temp = N/BLOCK_COLS;

	temp += temp < (double) N/BLOCK_COLS;
	sepSize = 3*N+temp*2+1;
	separatorRow = (char*) malloc((sepSize+2)*sizeof(char)); /* allocate enough space for the number of '-' characters needed + '\n' + '\0'. */
	if(separatorRow == NULL) {
		printf("Error: malloc has failed\n");
		exit(1);
	}
	separatorRow[sepSize+1] = '\0';
	repeatChar('-',sepSize,separatorRow);
	for(i = 0; i < N+(N)/BLOCK_ROWS+1; i++) { /* board printing */
		if(i%(BLOCK_ROWS+1) == 0) {
			if(i < N+(N)/BLOCK_ROWS) {
				printf("%s",separatorRow);
			}
		}
		else {
			for(col = 0; col < N; col++) {
				if(col%(BLOCK_COLS) == 0) {
					printf("| ");
				}
				if(gameBoard[row][col].fixed) { /* print a dot for a fixed cell. a cell with a value of 0 should never be fixed. */
					printf(".");
				}
				else { /* print a space for an unfixed cell */
					printf(" ");
				}
				if(gameBoard[row][col].value == 0) { /* empty cell */
					printf("  ");
				}
				else { /* print cell value */
					printf("%d ",gameBoard[row][col].value);
				}
			}
			printf("|\n");
			row++;
		}
	}
	printf("%s",separatorRow);
	free(separatorRow);
}
