#include <stdio.h>
#include <stdlib.h>
/*#include "Game.h"*/
#include "Solver.h"
#define	TRUE	1
#define FALSE	0


/*
 * Calculate all the legal values for cell[row,col], and update the "possible" array.
 *
 * Cell** 			board		-	A game board.
 * unsigned int		m			-	Number of rows of cells in each block.
 * unsigned int		n			-	Number of columns of cells in each block.
 * unsigned int		row			-	Row number (between 0 and N-1).
 * unsigned int 	col			-	Column number (between 0 and N-1).
 * unsigned int* 	possible	-	A binary array indicating which values are possible to assign to this cell,
 * 									and the number of possible values at the end of the array.
 */
void possibleVals(Board* boardPtr, unsigned int row, unsigned int col, unsigned int* possible) {
	unsigned int	i, j, count_i, count_j;
	unsigned int	m = boardPtr->m, n = boardPtr->n;
	unsigned int	N = m*n;
	int				val;
	for(i = 0; i < N; i++) {
		possible[i] = TRUE;
	}

	/* Row */
	for(j = 0; j < N; j++) {
		val = getCell(boardPtr, row,j)->value;
		if(val > 0) {
			possible[val-1] = FALSE;
		}
	}


	/* Column */
	for(i = 0; i < N; i++) {
		val = getCell(boardPtr, i, col)->value;
		if(val > 0) {
			possible[val-1] = FALSE;
		}
	}

	/* Block */
	i = m*((row)/m); /* Index of the first row of the block */
	j = n*((col)/n); /* Index of the first column of the block */
	for(count_i = 0; count_i < m; count_i++) {
		for(count_j = 0; count_j < n; count_j++) {
			val = getCell(boardPtr, i+count_i, j+count_j)->value;
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
 * unsigned int		N		-	Number of rows/columns in the board.
 * unsigned int		row		-	Row number (between 0 and N-1).
 * unsigned int 	col		-	Column number (between 0 and N-1).
 * unsigned int*	nextRow	-	A pointer to the variable on which the new row will be stored at.
 * unsigned int*	nextCol	-	A pointer to the variable on which the new column will be stored at.
 */
void calcNextCell(unsigned int N, unsigned int row, unsigned int col, unsigned int* nextRow, unsigned int* nextCol) {
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
 * Cell** 			original	-	Original game board.
 * Cell** 			temp		-	Copied game board.
 * unsigned int		m			-	Number of rows of cells in each block.
 * unsigned int		n			-	Number of columns of cells in each block.
 * unsigned int		row			-	Row number (between 0 and N-1).
 * unsigned int 	col			-	Column number (between 0 and N-1).
 */
int detSolve(Board* original, Board* temp, unsigned int row, unsigned int col) {
	unsigned int	m = original->m, n = original->n;
	unsigned int	N = m*n;
	unsigned int* 	possible;
	unsigned int	posValsCount;
	unsigned int	nextRow, nextCol;
	unsigned int	i = 0;
	Cell*			orig_cell;
	Cell*			sug_cell;

	possible = (unsigned int*)calloc(N+1, sizeof(unsigned int));
	if(possible == NULL) {
		printf("Error: calloc has failed\n");
		exit(1);
	}

	possibleVals(temp, row, col, possible);	/* Calculate all the possible values for current cell and save in possible */
	posValsCount = possible[N];		/* Number of possible values */

	orig_cell = getCell(original, row, col);
	sug_cell  = getCell(temp, row, col);

	/* If we're on an empty cell and there are no possible values left - unsolvable */
	if(posValsCount == 0 && orig_cell->value == 0) {
		free(possible);
		return FALSE;
	}

	/* If Last cell */
	if(row == N-1 && col == N-1) {
		if(orig_cell->value != 0 ) {
			free(possible);
			return TRUE;
		}
		for (i = 0; i < N; i++) {
			if(possible[i]) {
				sug_cell->value = i+1;
				free(possible);
				return TRUE;
			}
		}
		free(possible);
		return FALSE;
	}

	/* Not the last cell */

	calcNextCell(N, row, col, &nextRow, &nextCol);	/* Calculate next cell */

	/* Case 1: cell is already filled */
	if(orig_cell->value != 0 ) return detSolve(original, temp, nextRow,nextCol);

	/* ---- Case 2: cell is empty----- */
		for(i = 0; i < N; i++){ 			/* for all N values for this cell:*/
			if(possible[i]) {				/* if a possible value:*/
				sug_cell->value = i+1;							/* Assign next possible value and try to solve board */
				if(detSolve(original, temp, nextRow,nextCol)) { /* Case 2.1: Board is solvable*/
					free(possible);
					return TRUE;
				}
																/* Case 2.2: Board is unsolvable. try next possible value for this cell */
			}
		}
	sug_cell->value = 0;										/* No possible values found */
	free(possible);
	return FALSE;
}


/* Random backtrack board solver.
 * Recursively tries to brute force solve the board using values chosen randomly from the possible values.
 * If possible, updates the field sug_val for each cell on the board, otherwise - leaves it as 0.
 * Returns TRUE iff board is solvable.
 *
 * Cell** 			original	-	Original game board.
 * Cell** 			temp		-	Copied game board.
 * unsigned int		m			-	Number of rows of cells in each block.
 * unsigned int		n			-	Number of columns of cells in each block.
 * unsigned int		row			-	Row number (between 0 and N-1).
 * unsigned int 	col			-	Column number (between 0 and N-1).
 */
int randomSolve(Board* original, Board* temp, unsigned int row, unsigned int col){
	unsigned int	m = original->m, n = original->n;
	const unsigned int N = m*n;
	Cell*			orig_cell;
	Cell*			sug_cell;
	unsigned int* 	possible;
	unsigned int	nextRow, nextCol;
	unsigned int	val, posValsCount;
	int result;


	possible = (unsigned int*)calloc(N+1, sizeof(unsigned int));
	if(possible == NULL) {
		printf("Error: calloc has failed\n");
		exit(1);
	}

	orig_cell = getCell(original, row, col);
	sug_cell = getCell(temp, row, col);

	possibleVals(temp, row, col, possible);	/* Calculate all the possible values for current cell and save in possible */
	posValsCount = possible[N];		/* Number of possible values */

	/* If we're on an empty cell and there are no possible values left - unsolvable */
	if(posValsCount == 0 && orig_cell->value == 0) {
		free(possible);
		return FALSE;
	}

	/* If Last cell */
	if(row == N-1 && col == N-1) {
		if(sug_cell->value !=0 ) {
			free(possible);
			return TRUE;
		}
		val = chooseRandVal(possible, posValsCount);
		sug_cell->value = val;
		free(possible);
		return TRUE;
	}

	/* Not the last cell */

	calcNextCell(N, row, col, &nextRow, &nextCol);	/* Calculate next cell to check */

	/* ---- Case 1: cell is already filled ---- */
	if(sug_cell->value != 0 ) {
		result = randomSolve(original, temp, nextRow,nextCol);
		free(possible);
		return result;
	}

	/* ---- Case 2: cell is empty ---- */
	while(posValsCount > 0) {
		val = chooseRandVal(possible, posValsCount);
		sug_cell->value = val;				/* Assign next random possible value and try to solve board  */
		possible[val-1] = FALSE;	/* Make this value no longer possible so it won't be used next time */
		posValsCount--;
		if(randomSolve(original, temp, nextRow,nextCol)) {	/* Case 2.1: Board is solvable */
			free(possible);
			return TRUE;
		}
											/* Case 2.2: Board is unsolvable. try next random possible value for this cell */
	}
	sug_cell->value = 0;						/* No possible values found */
	free(possible);
	return FALSE;
}

/*
 * Copy board state and try to solve it using the deterministic backtracking solver.
 * If solvable - update solution board, restore sug_val to zero for all cells on the board.
 * Returns TRUE iff board is solvable.
 */
unsigned int detBacktracking(Board* boardPtr){
	unsigned int	solvable;
	Board 			tempBoard = {'\0'};		/* This board will be a copy of board, and will be solved instead of it. */
	initializeBoard(&tempBoard, boardPtr->m, boardPtr->n);
	copyBoard(boardPtr, &tempBoard);


	/*try to solve the board */
	solvable =  detSolve(boardPtr, &tempBoard, 0, 0);

	/* step 3 - update the solution board */
	if(solvable) {
		copyBoard(&tempBoard, getSolutionBoardPtr());
	}

	/* Free allocated temporary board */
	freeBoard(&tempBoard);
	return solvable;
}




/*
 * Solve an empty board, and reveal numOfHints cells.
 *
 * unsigned int	numOfHints	-	The number of cells with the solution value revealed (given from user input).
 */
void generateBoard(Board* gameBoardPtr, Board* solutionBoardPtr, unsigned int numOfHints) {
	Cell*			cell;
	Board 			tempBoard = {'\0'};		/* This board will be a copy of board, and will be solved instead of it. */
	unsigned int	row, col;
	unsigned int	m = gameBoardPtr->m, n = gameBoardPtr->n;
	unsigned int 	N = m*n;
	unsigned int	value;
	initializeBoard(&tempBoard,m,n);

	copyBoard(gameBoardPtr, &tempBoard);

	/* Solve game board using randomly chosen values (random backtracking) */
	randomSolve(gameBoardPtr, &tempBoard, 0, 0);

	/* Update solution board */
	copyBoard(&tempBoard, solutionBoardPtr);

	/* Randomly choose which cells to reveal to the user and set as fixed */
	while(numOfHints > 0){
		col = rand()%N;
		row = rand()%N;
		cell = getCell(gameBoardPtr,row,col);
		value = getCell(&tempBoard, row, col)->value;
		if(cell->fixed){
			continue;
		}
		setCellVal(gameBoardPtr, col+1,row+1,value);
		cell->fixed = TRUE;
		numOfHints--;
	}

}


void updateSolBoard(Board* gameBoardPtr, Board* solutionBoardPtr) {
	Board 			tempBoard = {'\0'};		/* This board will be a copy of board, and will be solved instead of it. */
	unsigned int	m = gameBoardPtr->m, n = gameBoardPtr->n;
	initializeBoard(&tempBoard,m,n);

	copyBoard(gameBoardPtr, &tempBoard);

	/* Solve game board using randomly chosen values (random backtracking) */
	randomSolve(gameBoardPtr, &tempBoard, 0, 0);

	/* Update solution board */
	copyBoard(&tempBoard, solutionBoardPtr);
}


/*
 * Checks if the current configuration of the game board is solvable.
 * If solvable and some cells' val field != sol_val - update relevant sol_val's.
 * Returns TRUE iff current configuration of game board is solvable.
 */
unsigned int validate(Board* boardPtr) {
	return 	detBacktracking(boardPtr);
}
/*---------------------------- ron update ---------------------------*/
/*unsigned int checkerrounous(){
	 to be implemented
	return TRUE;
}


 fill cells which contain a single legal value
 * pre: assume we are in Solve mode
void autofill(){
	unsigned int 	errounous, i , j, k;
	unsigned int 	possible[N+1] = {0};
	unsigned int	posValsCount;
	Cell* 			cell;

	 c - check if there is errounous cells
	errounous = checkerrounous(); if there are errors --> errounous = TRUE
	if (errounous){
		printf("Error:board contains erroneous values\n");
	}

	 d1 - copy:
	 * all the suggested values by autofill
	 * are the values which already on the board:
	initSugValues();

	 d2 - update the board with new values.
	 * cell->sug_value is not changing , while cell->value
	 * can be changed in the nested for loop

	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			cell = getCell(i,j);
			 Calculate all the possible values for current cell and save in possible
			 * this calculation refers to cell->sug_val which is not change
			possibleVals(i,j,possible);
			posValsCount = possible[N];  Number of possible values
			 if there are a few choices- ignore this cell
			if(posValsCount != 1){
				continue;
			}
			 there is only one choice:
			for(k=0; k<N; k++){
				if(possible[k]){
					setCellVal(i+1,j+1,k+1);
					break;
			}}
	}}

	 d4 - nullify suggested values for future use
	fixSuggestedVals();

	 g - print the new board
	printBoard();

}*/
