#include <stdio.h>
#include <stdlib.h>
#include "Solver.h"
#include "Stack.h"
#define	TRUE	1
#define FALSE	0

/********** Private method declarations **********/
/* Includes *some* of the private methods in this module */

int randomSolve(Board*, Board*, unsigned int, unsigned int);

/******* End of private method declarations ******/



/************************* Public methods *************************/

/*
 * Solve an empty board, and reveal numOfHints cells.
 *
 * Board*	gameBoardPtr		-	A pointer to the game board.
 * Board*	solutionBoardPtr	-	A pointer to the solution board.
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
		setCellVal(gameBoardPtr, row, col, value);
		cell->fixed = TRUE;
		numOfHints--;
	}
	if(getGameMode() == EDIT) { /* Un-fix all cells */
		for(row = 0; row < N; row++) {
			for(col = 0; col < N; col++) {
				getCell(gameBoardPtr,row,col)->fixed = FALSE;
			}
		}
	}
	/* Free allocated space used by tempBoard */
	freeBoard(&tempBoard);
}


/*
 * Updates the solutionBoardPtr->board according to gameBoardPtr->board.
 *
 * Board*	gameBoardPtr		-	A pointer to the game board.
 * Board*	solutionBoardPtr	-	A pointer to the solution board.
 */
void updateSolBoard(Board* gameBoardPtr, Board* solutionBoardPtr) {
	Board 			tempBoard = {'\0'};		/* This board will be a copy of board, and will be solved instead of it. */
	unsigned int	m = gameBoardPtr->m, n = gameBoardPtr->n;
	initializeBoard(&tempBoard,m,n);

	/* First, copy the game board to a temporary board */
	copyBoard(gameBoardPtr, &tempBoard);

	/* Then solve the game board using randomly chosen values (random backtracking) */
	randomSolve(gameBoardPtr, &tempBoard, 0, 0);

	/*update solution board */
	copyBoard(&tempBoard, solutionBoardPtr);

	/* Free tempBoard */
	freeBoard(&tempBoard);
}

/********************** End of public methods *********************/



/************************* Private methods *************************/

/*
 * Calculate all the legal values for cell[row,col], and update the "possible" array.
 *
 * Board* 			board		-	A pointer to a game board.
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
	possible[N] = 0;

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
 * Board* 			original	-	A pointer to the original game board.
 * Board* 			copy		-	A pointer to the copied game board.
 * unsigned int		m			-	Number of rows of cells in each block.
 * unsigned int		n			-	Number of columns of cells in each block.
 * unsigned int		row			-	Row number (between 0 and N-1).
 * unsigned int 	col			-	Column number (between 0 and N-1).
 */
int detSolve(Board* original, Board* copy, unsigned int row, unsigned int col) {
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

	possibleVals(copy, row, col, possible);	/* Calculate all the possible values for current cell and save in possible */
	posValsCount = possible[N];		/* Number of possible values */

	orig_cell = getCell(original, row, col);
	sug_cell  = getCell(copy, row, col);

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
	if(orig_cell->value != 0 ) return detSolve(original, copy, nextRow,nextCol);

	/* ---- Case 2: cell is empty----- */
		for(i = 0; i < N; i++){ 			/* for all N values for this cell:*/
			if(possible[i]) {				/* if a possible value:*/
				sug_cell->value = i+1;							/* Assign next possible value and try to solve board */
				if(detSolve(original, copy, nextRow,nextCol)) { /* Case 2.1: Board is solvable*/
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
 * Board* 			original	-	A pointer to the original game board.
 * Cell** 			copy		-	A pointer to the copied game board.
 * unsigned int		m			-	Number of rows of cells in each block.
 * unsigned int		n			-	Number of columns of cells in each block.
 * unsigned int		row			-	Row number (between 0 and N-1).
 * unsigned int 	col			-	Column number (between 0 and N-1).
 */
int randomSolve(Board* original, Board* copy, unsigned int row, unsigned int col){
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
	sug_cell = getCell(copy, row, col);

	possibleVals(copy, row, col, possible);	/* Calculate all the possible values for current cell and save in possible */
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
		result = randomSolve(original, copy, nextRow,nextCol);
		free(possible);
		return result;
	}

	/* ---- Case 2: cell is empty ---- */
	while(posValsCount > 0) {
		val = chooseRandVal(possible, posValsCount);
		sug_cell->value = val;				/* Assign next random possible value and try to solve board  */
		possible[val-1] = FALSE;	/* Make this value no longer possible so it won't be used next time */
		posValsCount--;
		if(randomSolve(original, copy, nextRow,nextCol)) {	/* Case 2.1: Board is solvable */
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
 *
 * Board*	boardPtr	-	A pointer to a game board.
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
 * Checks if the current configuration of the game board is solvable.
 * If solvable and some cells' val field != sol_val - update relevant sol_val's.
 * Returns TRUE iff current configuration of game board is solvable.
 *
 * Board*	boardPtr		-	A pointer a game board.
 */
unsigned int validate(Board* boardPtr) {
	return 	detBacktracking(boardPtr);
}


 /*fill cells which contain a single legal value
  * pre: assume we are in Solve mode*/
unsigned int autofill(Board* boardPtr){
	Board 				constBoard = {'\0'};		/* This board will be a copy of board, and won't change*/
	unsigned int		m = boardPtr->m, n = boardPtr->n;
	unsigned int 		N = m*n;
	unsigned int 		errounous, i , j, k;
	unsigned int* 		possible;
	unsigned int		posValsCount;
	unsigned int		lastVal;
	Cell* 				cell;
	SinglyLinkedList*	move;


	/* c - check if there are errounous cells*/
	errounous = hasErrors(boardPtr); /*if there are errors --> errounous = TRUE*/
	if (errounous){
		printf("Error: board contains erroneous values\n");
		return FALSE;
	}

	/* allocate array of possibilities */
	possible = (unsigned int*)calloc(N+1, sizeof(unsigned int));
	if(possible == NULL) {
		printf("Error: calloc has failed\n");
		exit(1);
	}

	/* d1 - copy:
	 * all the suggested values by autofill
	 * are the values which already on the board:*/
	initializeBoard(&constBoard,m,n);
	copyBoard(boardPtr, &constBoard);

	/* d2 - update the board with new values.
	 * cell->sug_value is not changing , while cell->value
	 * can be changed in the nested for loop*/
	move = createNewSinglyLinkedList();
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			cell = getCell(&constBoard,i,j);
			if (cell->value!=0)continue; /*cell is not empty-->continue*/
			/* Calculate all the possible values for current cell, and save in possible:*/
			possibleVals(&constBoard,i,j,possible);
			posValsCount = possible[N];  /*Number of possible values*/
			/* if there are a few choices- ignore this cell:*/
			if(posValsCount != 1)continue;
			/*there is only one choice-->find it*/
			for(k=0; k<N; k++){
				if(possible[k]){
					printf("Cell<%d,%d> set to %d\n",j+1,i+1,k+1);
					lastVal = cell->value;
					setCellVal(boardPtr,i,j,k+1);
					singly_addLast(move,i,j,k+1,lastVal);
					break;
				}
			}
		}
	}

	if(move->size > 0) {
		addMove(move);
	}
	else { /* No moves to add */
		singly_clear(move);
	}

	free(possible);
	freeBoard(&constBoard);
	return TRUE;
}




/*------------------- num_solutions() helpful functions-----------------------------*/


/* calculates the next cell to be checked, and
 * init only the relevant variables, so next while loop iteration
 * will work on the (new) next cell. */
void getNextCellCordinates(info** cd, unsigned int N){
	unsigned int nextRow, nextCol;

	/* Calculate and set next cell */
	calcNextCell(N, (*cd)->row, (*cd)->col, &nextRow, &nextCol);
	(*cd)->row 			= nextRow;
	(*cd)->col 			= nextCol;
	(*cd)->first_time 	= TRUE;

}


/* init default cell settings for every new cell we check*/
void init_cell (Board* original, Board* temp, info** def, unsigned int N){
	/* Allocate memory for possible values.
	 * we free this memory at the while loop */
	(*def)->possible  = (unsigned int*)calloc(N+1, sizeof(unsigned int));
	if((*def)->possible == NULL)  {printf("Error: calloc has failed\n");exit(1);}

	/* Adjustments*/
	(*def)->k			 = 0;
	(*def)->counter 	 = 0;
	(*def)->orig_cell 	 = getCell(original, (*def)->row, (*def)->col);
	(*def)->sug_cell     = getCell(temp    , (*def)->row, (*def)->col);

	possibleVals(temp, (*def)->row, (*def)->col, (*def)->possible);	/* Calculate all the possible values for current cell and save in possible */
	(*def)->posValsCount = (*def)->possible[N];					    /* Number of possible values */
	if((*def)->orig_cell->value !=0 ) {(*def)->posValsCount = 1;}   /* fixed cell has only 1 legal value */
}



int exhaustive_backtracking(Board* original, Board* temp) {
	/* Variables */
	unsigned int	  N         = (original->m * original->n);
	unsigned int	  res_counter;
	struct StackNode* root 		= NULL;		/* beautiful stack who mimic recursion */
	info* cd = NULL; 						/* data of current cell */
	unsigned int counter;


	cd 	= (info*) malloc(sizeof(info));  /* allocate memory safely (freed after while loop) */
	if(cd == NULL) {printf("Error: malloc has failed\n");exit(1);}

	/* ---adjust 1st cell data --- */
	cd->row	= 0;cd->col	= 0;
	cd->first_time	= TRUE;
	/*--For each cell:--*/
	do{
			if(cd->first_time){  /* first time we meet a cell-->init cell */
				init_cell (original,temp, &cd, N);
			}

			/* If Last cell */
			if(cd-> row == N-1 && cd->col == N-1) {
				/* If cell is already filled- any board who lead
				 * to this cell is solvable with one solution.
				 * Else, cell is empty. any board who lead
				 * to this cell is solvable with posValsCount solutions.
				 * (the special case of unsolvable board will return 0) */
				if(cd->orig_cell->value != 0 )
					cd->counter = 1;
				else
					cd->counter = cd->posValsCount;
				/* return to cell's father by popping */
				free(cd->possible);
				cd->possible = NULL;
				if(isEmpty(root)) break;
				pop(&root,&cd);
				continue;
			}

			/* Not the last cell- 2 cases: */

			/* Case 1: if cell is already filled  - for each legal board that will be
			 * Accepted by filling the next cells, there will be 1 solution */
			if(cd->orig_cell->value != 0 ) {
				if(cd->first_time){ /*1st time we meet this cell--> push it */
					push(&root,&cd);
					getNextCellCordinates(&cd,N);
				}
				else{				/* 2st time we meet the cell--> pop it */
					free(cd->possible);
					cd->possible = NULL;
					if(isEmpty(root)) break;
					pop(&root,&cd);
				}
				continue;
			}

			/* Case 2: cell is empty */
			for(; cd->k < N; (cd->k)++){ 			/* for all N values for this cell:*/
				if(cd->possible[cd->k]) {			/* if a possible value:  */
					cd->sug_cell->value = (cd->k)+1;/* assign it */
					push(&root,&cd);				/* Try to solve the rest of the board by calling next cell*/
					getNextCellCordinates(&cd,N);
					goto nextCell; 					/* label who lead to the next cell*/
				}
			}
			free(cd->possible);
			cd->possible = NULL;
			if(isEmpty(root)) break;
			pop(&root,&cd);
			nextCell: continue;
	}
	while(1);
	counter = cd->counter;
	free(cd);
	return counter;
	res_counter = cd->counter;
	free (cd);
	return res_counter;
}


/*return the number of solutions.
 * pre: assume we are in Edit or Solve mode*/
unsigned int num_solutions(Board* boardPtr){
	unsigned int	 errounous;
	unsigned int	 counter = 0;
	Board 			 tempBoard = {'\0'};

	/* c - check if there are errounous cells*/
	errounous = hasErrors(boardPtr); /*if there are errors --> errounous = TRUE*/
	if (errounous){
		printf("Error: board contains erroneous values\n");
		return TRUE;
	}

	/* preapere temp board -
	 * This board will be a copy of board, and will be solved instead of it.*/
	initializeBoard(&tempBoard, boardPtr->m, boardPtr->n);
	copyBoard(boardPtr, &tempBoard);

	/*try to solve the board */
	counter =  exhaustive_backtracking (boardPtr, &tempBoard);

	/* Free allocated temporary board */
	freeBoard(&tempBoard);

	/* print result*/
	printf("Number of solutions: %d\n",counter);
	if(counter==1){ printf("This is a good board!\n");}
	if(counter> 1){ printf("The puzzle has more than 1 solution, try to edit it further\n");}
	return counter;
}

/*-------------------------generate() helpful functions--------------------------------*/

/* ------to be replaced------ */
unsigned int ilpBacktracking(Board* boardPtr){
	return detBacktracking(boardPtr);/* to be replaced by ILP */
}


/* try to generate x cells at gameBoardPtr board.
 * try to solve with ilp(result at "solution_board" so we copy it to gameBoardPtr board).
 * finally deleting cells values until only y values left

 * pre: we are in EDIT mode			(Checked in MainAux.c)
 * pre: x, y to are int 			(Checked in MainAux.c)
 * pre :x, y have legal coordinates (Checked in MainAux.c) */
unsigned int generate(Board* gameBoardPtr,int x, int y ) {
	SinglyLinkedList*	move;
	unsigned int	x_values_successfully, ilp_Successfully;
	unsigned int	rand_row, rand_col, rand_val, posValsCount;
	unsigned int	m = gameBoardPtr->m, n = gameBoardPtr->n;
	int	N = m*n;
	int i,j,try;
	Cell* cur_cell;

	/* board must be empty */
	if (!isBoardEmpty(*gameBoardPtr)){
		printf("Error: board is not empty\n");
	    return FALSE;
	}

	/* try 1000 times(max) to fill x cells */
	for(try=0; try<1000;try++){
		x_values_successfully = TRUE;
		ilp_Successfully = FALSE;
		for (i=0; i<x; i++){ /* find x good values */
			/* choose random cell */
			rand_col = rand()%N;/* +1?? */
			rand_row = rand()%N;/* +1?? */
			cur_cell = getCell(gameBoardPtr, rand_row, rand_col);
			/* if cell already has value-> choose another cell instead */
			if(cur_cell->value != 0){
				i--;
				continue;
			}
			/* Calculate all the possible values for current cell. */
			possibleVals(gameBoardPtr, rand_row, rand_col, cur_cell->possible_vals);
			posValsCount = cur_cell->possible_vals[N];		/* Number of possible values */
			/* Choose a random value for the cell*/
			if(posValsCount == 0){ /* Each value for this cell will be illegal */
				initializeBoard(gameBoardPtr,m,n);
				x_values_successfully = FALSE;
				break; /* next try */
			}
			else{/* we can fill this cell legally with a value */
				rand_val =  chooseRandVal(cur_cell->possible_vals, posValsCount);
				setCellVal(gameBoardPtr,rand_row,rand_col,rand_val);
			}
		}/* finished current board building, maybe with illegal board */

		if(x_values_successfully){
			ilp_Successfully = ilpBacktracking(gameBoardPtr);
		}

		if(ilp_Successfully) break;
		else{
				initializeBoard(gameBoardPtr,m,n);
				continue;
		}

	}/* Outer for loop was ended */

	if(!ilp_Successfully){	/* Failed to generate the board */
		printf("Error: puzzle generator failed\n");
		initializeBoard(gameBoardPtr,m,n);
	    return FALSE;
	}
	/* board generated successfully. */
	/* now, copy the board that ILP solved to be our board */
	copyBoard(getSolutionBoardPtr(), gameBoardPtr);

	/* keep only y cells */
	gameBoardPtr->cellsDisplayed = N*N;
	while(gameBoardPtr->cellsDisplayed > (unsigned int)y  ){
		/* choose random cell */
		rand_col = rand()%N ;
		rand_row = rand()%N ;
		cur_cell = getCell(gameBoardPtr,    rand_row, rand_col);

		/* delete it's content */
		if(cur_cell->value != 0)
			setCellVal(gameBoardPtr,rand_row,rand_col,0);
	}

	/* Add move to undo/redo list:
	 * scan the board for changes: */
	move = createNewSinglyLinkedList();
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			cur_cell = getCell(gameBoardPtr, i, j);
			if(cur_cell->value != 0) /* if cell has changed --> save move in list */
				singly_addLast(move, i, j, cur_cell->value, 0);
		}
	}
	if(move->size > 0) {
		addMove(move);
	}
	else { /* No moves to add */
		singly_clear(move);
	}

	return TRUE;
	/* Board printing in MainAux.c */
}

