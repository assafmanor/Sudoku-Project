#include <stdio.h>
#include <stdlib.h>
#include "Solver.h"
#include "Game.h"
#define	TRUE	1
#define FALSE	0


/*
 * Calculate all the legal values for cell[row,col], and update the "possible" array.
 *
 * unsigned int		row			-	Row number (between 0 and N-1).
 * unsigned int 	col			-	Column number (between 0 and N-1).
 * unsigned int* 	possible	-	A binary array indicating which values are possible to assign to this cell,
 * 									and the number of possible values at the end of the array.
 */
void possibleVals(unsigned int row, unsigned int col, unsigned int* possible) {
	unsigned int	i, j, count_i, count_j;
	int				val;
	for(i = 0; i < N; i++) {
		possible[i] = TRUE;
	}

	/* Row */
	for(j = 0; j < N; j++) {
		val = getCell(row,j)->sug_val;
		if(val > 0) {
			possible[val-1] = FALSE;
		}
	}

	/* Column */
	for(i = 0; i < N; i++) {
		val = getCell(i,col)->sug_val;
		if(val > 0) {
			possible[val-1] = FALSE;
		}
	}

	/* Block */
	i = N_ROWS*((row)/N_ROWS); /* Index of the first row of the block */
	j = M_COLS*((col)/M_COLS); /* Index of the first column of the block */
	for(count_i = 0; count_i < N_ROWS; count_i++) {
		for(count_j = 0; count_j < M_COLS; count_j++) {
			val = getCell(i+count_i,j+count_j)->sug_val;
			if(val > 0) {
				possible[val-1] = FALSE;
			}
		}
	}

	/* count the number of possible values,
	 * and put it in the last index of possible */
	for(i = 0; i < N; i++) {
		if(possible[i]) {
			possible[N] ++;
		}
	}

}



/*
 * Randomly chooses and returns a value from all possible values.
 *
 * unsigned int*	possible 	-	A binary array indicating if value is possible or not, for each value from 1 to N.
 * unsigned int		posValCount	-	The number of possible values to assign on a specific cell. ie. the number of 1's in possible.
 *
 */
unsigned int chooseRandVal(unsigned int* possible, unsigned int posValsCount) {
	unsigned int	randPos = 0;
	unsigned int	count = 0, i = 0;

	i = 0;
	if(posValsCount > 1) { 	/* More than one possible value to choose from. Choose index randomly (between 1 and posValsCount). */
		randPos = rand()%posValsCount + 1;
	}
	else {					/* Only one possible value - no need to randomize */
		randPos = 1;
	}
	while(count != randPos) {	/* Stop when reaching the randPos'th possible value. */
		if(possible[i]) {	/* Found possible value */
			count++;
		}
		i++;
	}
	return i;
}


/*
 * Given the coordinates of a cell (row and column), calculates the coordinates of the next cell,
 * ordered left to right, top to bottom.
 * Updates the values of nextRow and nextCol to the coordinates of the next cell.
 *
 * unsigned int		row		-	Row number (between 0 and N-1).
 * unsigned int 	col		-	Column number (between 0 and N-1).
 * unsigned int*	nextRow	-	A pointer to the variable on which the new row will be stored at.
 * unsigned int*	nextCol	-	A pointer to the variable on which the new column will be stored at.
 */
void calcNextCell(unsigned int row, unsigned int col, unsigned int* nextRow, unsigned int* nextCol) {
	if (col == N-1) {
		*nextCol = 0;
		*nextRow = row+1;
	}
	else {
		*nextCol = col+1;
		*nextRow = row;
	}
}


/* Deterministic backtrack board solver.
 * Recursively tries to brute force solve the board using values chosen in order from the possible values.
 * If possible, updates the field sug_val for each cell on the board, otherwise - leaves it as 0.
 * Returns TRUE iff board is solvable.
 *
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int col		-	Column number (between 0 and N-1).
 */
int detSolve(unsigned int row, unsigned int col) {
	Cell*			cell = getCell(row,col);
	unsigned int 	possible[N+1] = {0};
	unsigned int	posValsCount;
	unsigned int	nextRow, nextCol;
	unsigned int	i = 0;

	possibleVals(row,col,possible);	/* Calculate all the possible values for current cell and save in possible */
	posValsCount = possible[N];		/* Number of possible values */

	/* If we're on an empty cell and there are no possible values left - unsolvable */
	if(posValsCount == 0 && cell->value == 0) return FALSE;

	/* If Last cell */
	if(row == N-1 && col == N-1) {
		if(cell->value != 0 ) return TRUE;
		for (i = 0; i < N; i++) {
			if(possible[i]) {
				cell->sug_val = i+1;
				return TRUE;
			}
		}
		return FALSE;
	}

	/* Not the last cell */

	calcNextCell(row, col, &nextRow, &nextCol);	/* Calculate next cell */

	/* Case 1: cell is already filled */
	if(cell->value != 0 ) return detSolve(nextRow,nextCol);

	/* ---- Case 2: cell is empty----- */
		for(i = 0; i < N; i++){ 			/* for all N values for this cell:*/
			if(possible[i]) {				/* if a possible value:*/
				cell->sug_val = i+1;							/* Assign next possible value and try to solve board */
				if(detSolve(nextRow,nextCol)) return TRUE;		/* Case 2.1: Board is solvable*/
																/* Case 2.2: Board is unsolvable. try next possible value for this cell */
			}
		}
	cell->sug_val = 0;											/* No possible values found */
	return FALSE;
}


/* Random backtrack board solver.
 * Recursively tries to brute force solve the board using values chosen randomly from the possible values.
 * If possible, updates the field sug_val for each cell on the board, otherwise - leaves it as 0.
 * Returns TRUE iff board is solvable.
 *
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int col		-	Column number (between 0 and N-1).
 */
int randomSolve(unsigned int row, unsigned int col){
	Cell* 			cell = getCell(row,col);
	unsigned int 	possible[N+1] = {0};
	unsigned int	nextRow, nextCol;
	unsigned int	val, posValsCount;

	possibleVals(row,col,possible);	/* Calculate all the possible values for current cell and save in possible */
	posValsCount = possible[N];		/* Number of possible values */

	/* If we're on an empty cell and there are no possible values left - unsolvable */
	if(posValsCount == 0 && cell->value == 0) return FALSE;

	/* If Last cell */
	if(row == N-1 && col == N-1) {
		if(cell->sug_val !=0 ) return TRUE;
		val = chooseRandVal(possible, posValsCount);
		cell->sug_val = val;
		return TRUE;
	}

	/* Not the last cell */

	calcNextCell(row, col, &nextRow, &nextCol);	/* Calculate next cell to check */

	/* ---- Case 1: cell is already filled ---- */
	if(cell->sug_val != 0 ) return randomSolve(nextRow,nextCol);

	/* ---- Case 2: cell is empty ---- */
	while(posValsCount > 0) {
		val = chooseRandVal(possible, posValsCount);
		cell->sug_val = val;				/* Assign next random possible value and try to solve board  */
		possible[val-1] = FALSE;	/* Make this value no longer possible so it won't be used next time */
		posValsCount--;
		if(randomSolve(nextRow,nextCol)) {	/* Case 2.1: Board is solvable */
			return TRUE;
		}
											/* Case 2.2: Board is unsolvable. try next random possible value for this cell */
	}
	cell->sug_val = 0;						/* No possible values found */
	return FALSE;
}


/*
 * Copy values to suggested values.
 */
void initSugValues() {
	int i,j;
	Cell* cell;
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			cell = getCell(i,j);
			cell->sug_val = cell->value;
		}
	}
}


/*
 * Update the solution board.
 * Copies the values from sug_val (used after board was solved using random or deterministic backtracking).
 */
void updateSolutionBoard() {
	int i,j;
	Cell* cell;
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			cell = getCell(i,j);
			cell->sol_val = cell->sug_val;
		}
	}
}


/*
 * Changes the state of possible_val to be as before.
 */
void fixSuggestedVals() {
	int i,j;
	Cell* cell;
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			cell = getCell(i,j);
			cell->sug_val = 0;
		}
	}
}


/*
 * Copy board state and try to solve it using the deterministic backtracking solver.
 * If solvable - update solution board, restore sug_val to zero for all cells on the board.
 * Returns TRUE iff board is solvable.
 */
unsigned int detBacktracking(){
	unsigned int	solvable;

	/* step 1 - copy
	 * all the suggested values by detBacktracking
	 * are the values which already on the board: */
	initSugValues();

	/* step 2 - try to solve the board */
	solvable =  detSolve(0,0);

	/* step 3 - update the solution board (all cell->sol_val) */
	if(solvable) {
		updateSolutionBoard();
	}

	/* step 4 - nullify suggested values for future backtracking */
	fixSuggestedVals();

	return solvable;
}




/*
 * Solve an empty board, and then update for each cell: val<-sug_val.
 *
 * unsigned int	numOfHints	-	The number of cells with the solution value revealed (given from user input).
 */
void generateBoard(unsigned int numOfHints) {
	unsigned int	row,col;
	Cell*			cell;

	/* Solve game board using randomly chosen values (random backtracking) */
	initSugValues();
	randomSolve(0,0);
	updateSolutionBoard();
	fixSuggestedVals();

	/* Randomly choose which cells to reveal to the user and set as fixed */
	while(numOfHints > 0){
		col = rand()%(N);
		row = rand()%(N);
		cell = getCell(row,col);
		if(cell->fixed){
			continue;
		}
		setCellVal(col+1,row+1,cell->sol_val);
		cell->fixed = TRUE;
		numOfHints--;
	}

}


/*
 * Checks if the current configuration of the game board is solvable.
 * If solvable and some cells' val field != sol_val - update relevant sol_val's.
 * Returns TRUE iff current configuration of game board is solvable.
 */
unsigned int validate() {
	return 	detBacktracking();
}
