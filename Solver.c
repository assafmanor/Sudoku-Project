#include <stdio.h>
#include <stdlib.h>
#include "Solver.h"
#include "Stack.h"
#include "ILP_Solver.h"
#define	TRUE	1
#define FALSE	0

/********** Private method declarations **********/
/* Includes *some* of the private methods in this module */

/*int randomSolve(Board*, Board*, unsigned int, unsigned int);*/

void possibleVals(Board* boardPtr, unsigned int row, unsigned int col, unsigned int* possible);

unsigned int chooseRandVal(unsigned int* possible, unsigned int posValsCount);

void calcNextCell(unsigned int N, unsigned int row, unsigned int col, unsigned int* nextRow, unsigned int* nextCol);

void getNextCellCordinates(info** cd, unsigned int N);

void init_cell (Board* original, Board* temp, info** def, unsigned int N);

int exhaustive_backtracking(Board* original, Board* temp);

/******* End of private method declarations ******/



/************************* Public methods *************************/


/*
 * Checks if the current configuration of the game board is solvable.
 * If solvable and some cells' val field != sol_val - update relevant sol_val's.
 * Returns TRUE iff current configuration of game board is solvable.
 *
 *	Pre: board is not erroneous	(Checked in MainAux.c)
 *
 * Board*	boardPtr		-	A pointer a game board.
 */
int validate(Board* boardPtr) {
	unsigned int isSolvable = ilpSolver(boardPtr,getSolutionBoardPtr()); /* try to solve the board and update the sol board if solvable. */
	return isSolvable;
}


/*
 * Returns TRUE if all cells are filled on a game board and are not erroneous.
 * Returns FALSE if all cells are filled but there are erroneous values.
 * Returns -1 if not all cells are filled.
 *
 * Board*	boardPtr	-	A game board pointer.
 */
int isBoardComplete(Board* boardPtr) {
	unsigned int N = boardPtr->m*boardPtr->n;
	if(boardPtr->cellsDisplayed == N*N) { /* all cells are filled */
		return validate(boardPtr); /* return TRUE iff board is solvable, ie - all values are legal. */
	}
	return -1; /* not all cells are filled */
}


/*
 * Fill cells which contain exactly a single legal value
 * pre: assume we are in Solve mode (Checked in MainAux.c)
 *
 * Board*	boardPtr		-	A pointer a game board.
 */
unsigned int autofill(Board* boardPtr){
	Board 				constBoard = {'\0'};		/* This board will be a copy of board, and won't change*/
	unsigned int		m = boardPtr->m, n = boardPtr->n;
	unsigned int 		N = m*n;
	unsigned int 		errounous, row , col, val;
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
	for(row = 0; row < N; row++) {
		for(col = 0; col < N; col++) {
			cell = getCell(&constBoard,row,col);
			if (cell->value!=0)continue; /*cell is not empty-->continue*/
			/* Calculate all the possible values for current cell, and save in possible:*/
			possibleVals(&constBoard,row,col,possible);
			posValsCount = possible[N];  /*Number of possible values*/
			/* if there are a few choices- ignore this cell:*/
			if(posValsCount != 1)continue;
			/*there is only one choice-->find it*/
			for(val=0; val<N; val++){
				if(possible[val]){
					printf("Cell <%d,%d> set to %d\n",col+1,row+1,val+1);
					lastVal = cell->value;
					setCellVal(boardPtr,row,col,val+1);
					singly_addLast(move,row,col,val+1,lastVal);
					break;
				}
			}
		}
	}

	if(move->size > 0) { /* if the number of cells autofilled is greater than zero - add to move list */
		addMove(move);
	}
	else { /* no autofills */
		singly_clear(move);
	}

	free(possible);
	freeBoard(&constBoard);

	return TRUE;
}


/*
 * Calculates the next cell to be checked, and
 * init only the relevant variables, so next while loop iteration
 * will work on the (new) next cell.
 *
 * Board*	boardPtr		-	A pointer a game board.
 */
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



/*
 * Try to generate x cells at gameBoardPtr board.
 * try to solve with ilp(result at "solution_board" so we copy it to gameBoardPtr board).
 * finally deleting cells values until only y values left.
 *
 * pre: we are in EDIT mode			(Checked in MainAux.c)
 * pre: x, y to are int 			(Checked in MainAux.c)
 * pre :x, y have legal coordinates (Checked in MainAux.c)
 *
 */
unsigned int generate(Board* gameBoardPtr,int x, int y ) {
	SinglyLinkedList*	move;
	unsigned int	x_values_successfully, ilp_Successful;
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
	for(try = 0; try < 1000; try++){
		x_values_successfully = TRUE;
		ilp_Successful = FALSE;
		for (i=0; i<x; i++){ /* find x good values */
			/* choose random cell */
			rand_col = rand()%N;
			rand_row = rand()%N;
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
				nullifyBoard(gameBoardPtr);
				x_values_successfully = FALSE;
				break; /* next try */
			}
			else{/* we can fill this cell legally with a value */
				rand_val = chooseRandVal(cur_cell->possible_vals, posValsCount);
				setCellVal(gameBoardPtr,rand_row,rand_col,rand_val);
			}
		}/* finished current board building, maybe with illegal board */

		if(x_values_successfully){ /* Try to solve board */
			ilp_Successful = ilpSolver(gameBoardPtr,getSolutionBoardPtr());
		}

		if(ilp_Successful) break;
		else{
			nullifyBoard(gameBoardPtr);
				continue;
		}

	}/* Outer for loop was ended */

	if(!ilp_Successful){	/* Failed to generate the board */
		printf("Error: puzzle generator failed\n");
		nullifyBoard(gameBoardPtr);
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





/****************** num_solutions() Private methods *******************/

/*
 * Calculates the next cell to be checked, and
 * init only the relevant variables, so next while loop iteration
 * will work on the (new) next cell.
 *
 * info**			cd	-	Current cell's info.
 * unsigned int 	N	-	Number of rows/columns in the board.
 */
void getNextCellCordinates(info** cd, unsigned int N){
	unsigned int nextRow, nextCol;

	/* Calculate and set next cell */
	calcNextCell(N, (*cd)->row, (*cd)->col, &nextRow, &nextCol);
	(*cd)->row 			= nextRow;
	(*cd)->col 			= nextCol;
	(*cd)->first_time 	= TRUE;

}


/*
 * init default cell settings for every new cell we check
 *
 * Board* 			original	-	A pointer a game board.
 * Board* 			temp		-	A pointer a temp board.
 * info** 			def			-	Current cell's info.
 * unsigned int 	N			-	Number of rows/columns in the board.
 */
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


/*
 * Exhaustive backtracking on the board, and returns num of solutions.
 *
 * Board* 			original	-	A pointer a game board.
 * Board* 			temp		-	A pointer a temp board.
 */
int exhaustive_backtracking(Board* original, Board* temp) {
	/* Variables */
	unsigned int	  N         = (original->m * original->n);
	struct StackNode* root 		= NULL;		/* beautiful stack who mimic recursion */
	info* cd = NULL; 						/* data of current cell */
	unsigned int counter;


	cd 	= (info*) malloc(sizeof(info));  /* allocate memory safely (freed after while loop) */
	if(cd == NULL) {printf("Error: malloc has failed\n");exit(1);}

	/* ---adjust 1st cell data --- */
	cd->row	= 0;cd->col	= 0;
	cd->first_time	= TRUE;
	/*--For each cell:--*/
	do {
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
	while(TRUE);
	counter = cd->counter;
	free(cd);
	return counter;
}

