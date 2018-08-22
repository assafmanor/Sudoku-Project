#include <stdio.h>
#include <stdlib.h>
#include "Game.h"



Board				gameBoard;					/* a board storing the game values (the ones shown) */
Board				solutionBoard;				/* a board storing the solved values */
DoublyLinkedList	*moveList;					/* a doubly linked list containing all moves done by the user that are still relevant for undos and redos */
DoublyNode			*curMove;					/* a pointer to the current move the user is at */
unsigned int		gameMode;					/* current game mode (init / solve / edit). */
unsigned int		markErrors = TRUE;			/* a binary variable indicating that the player wants to mark erroneous cells (with an asterisk). */



/* --------------- Cell functions --------------- */


/*
 * Returns a pointer to the specified cell.
 *
 * Board*	boardPtr	-	A pointer to a game board.
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int col		-	Column number (between 0 and N-1).
 */
Cell* getCell(Board* boardPtr, unsigned int row, unsigned int col) {
	return &(boardPtr->board[row][col]);
}


/*
 * Given a column and row of a cell - returns the solution value of the cell.
 *
 * unsigned int	col		-	Column number (between 0 and N-1).
 * unsigned int row		-	Row number (between 0 and N-1).
 */
unsigned int getHint(unsigned int row, unsigned int col) {
	return solutionBoard.board[row][col].value;
}


/*
 * Given a column and row of a cell - return TRUE iff cell is fixed in boardPtr->board (given as a hint in the beginning of a new game in solve mode).
 *
 * Board*	boardPtr	-	A pointer to a game board.
 * unsigned int	col		-	Column number (between 0 and N-1).
 * unsigned int row		-	Row number (between 0 and N-1).
 */
unsigned int isCellFixed(Board* boardPtr, unsigned int row, unsigned int col) {
	return getCell(boardPtr,row,col)->fixed;
}


/*
 * Checks if cell[row][col] of boardPtr->board contains an erroneous value.
 *
 * Board*	boardPtr	-	A pointer to a game board.
 * unsigned int	col		-	Column number (between 0 and N-1).
 * unsigned int	row		-	Row number (between 0 and N-1).
 */
unsigned int isErroneous(Board* boardPtr, unsigned int row, unsigned int col) {
	unsigned int i,j, count_i, count_j;
	unsigned int m = boardPtr->m, n = boardPtr->n;
	unsigned int N = m*n;
	unsigned int val = getCell(boardPtr,row,col)->value;
	Cell* cell;

	if(val == 0) {
		return FALSE;
	}

	/* Row */
	for(j = 0; j < N; j++) {
		if(j == col) continue;
		cell = getCell(boardPtr,row,j);
		if(cell->value == val) {
			return TRUE;
		}
	}

	/* Column */
	for(i = 0; i < N; i++) {
		if(i == row) continue;
		cell = getCell(boardPtr,i,col);
		if(cell->value == val) {
			return TRUE;
		}
	}

	/* Block */
	i = m*((row)/m); /* Index of the first row of the block */
	j = n*((col)/n); /* Index of the first column of the block */
	for(count_i = 0; count_i < m; count_i++) {
		for(count_j = 0; count_j < n; count_j++) {
			if(i+count_i == row && j+count_j == col) continue;
			cell = getCell(boardPtr,i+count_i,j+count_j);
			if(cell->value == val) {
				return TRUE;
			}
		}
	}
	return FALSE;
}


/*
 * Given a row a column and a new value, updates the possible values for the row,
 * column, and block. also marks as erroneous if necessary.
 *
 * Board*	boardPtr	-	A pointer to a game board.
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
		if(j == col) continue;
		if(val > 0) {
			getCell(boardPtr,row,j)->possible_vals[val-1] = FALSE; /* set new val as invalid for the whole row. */

		}
		if(lastVal > 0) {
			getCell(boardPtr,row,j)->possible_vals[lastVal-1] = TRUE; /* set last val as valid for the whole column. */
		}

	}

	/* Column */
	for(i = 0; i < N; i++) {
		if(i == row) continue;
		if(val > 0) {
			getCell(boardPtr,i,col)->possible_vals[val-1] = FALSE;
		}
		if(lastVal > 0) {
			getCell(boardPtr,i,col)->possible_vals[lastVal-1] = TRUE;
		}
	}

	/* Block */
	i = m*((row)/m); /* Index of the first row of the block */
	j = n*((col)/n); /* Index of the first column of the block */
	for(count_i = 0; count_i < m; count_i++) {
		for(count_j = 0; count_j < n; count_j++) {
			if(i+count_i == row && j+count_j == col) continue;
			if(val > 0) {
				getCell(boardPtr,i+count_i,j+count_j)->possible_vals[val-1] = FALSE;
			}
			if(lastVal > 0) {
				getCell(boardPtr,i+count_i,j+count_j)->possible_vals[lastVal-1] = TRUE;
			}
		}
	}
}


/*
 * Assigns the value of val to cell[row][col]->value (assuming value is possible),
 * updates the possible values of all the cells in the row, column and block,
 *
 * Board*	boardPtr	-	A pointer to a game board.
 * unsigned int	col		-	Column number (between 0 and N-1).
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int	val		-	The value being assigned to the cell. (Between 0 and N).
 */
void setCellVal(Board* boardPtr, unsigned int row, unsigned int col, unsigned int val) {
	unsigned int	lastVal;
	lastVal = boardPtr->board[row][col].value;	/* remember the last value */
	if(val == lastVal) { /* if you want to change the value to be the same as before - there's nothing to do */
		return;
	}
	updatePossibleValues(boardPtr, row, col, val);
	getCell(boardPtr,row,col)->value = val;
	/* update the number of cells displayed (used for checking if game over) */
	if(val > 0 && lastVal == 0) {
		boardPtr->cellsDisplayed++;
	}
	if(val == 0 && lastVal > 0) {
		boardPtr->cellsDisplayed--;
	}
}


/* --------------- Board functions --------------- */


/*
 * Frees all allocated space used by board
 *
 * Board*	boardPtr	-	A pointer to a game board.
 */
void freeBoard(Board* boardPtr) {
	unsigned int i, j;
	unsigned int N;
	Cell* temp_cell;

	N = boardPtr->m*boardPtr->n;
	/* free allocated possible_vals for each cell, and free rows */
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			temp_cell = getCell(boardPtr,i,j);
			free(temp_cell->possible_vals);
		}
		/* free allocated row */
		free(boardPtr->board[i]);
	}
	/* free the board itself */
	free(boardPtr->board);
}


/*
 * Initializes the game board's parameters for a new game
 * with the same m and n, assuming memory has already been allocated.
 *
 * Board*		boardPtr	-	A pointer to a game board.
 *
 */
void nullifyBoard(Board* boardPtr) {
	unsigned int	N = boardPtr->m * boardPtr->n;
	unsigned int	i,j,k;
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			setCellVal(boardPtr,i,j,0);
			for(k = 0; k < N; k++) {
				getCell(boardPtr,i,j)->possible_vals[k] = TRUE;
			}
		}
	}
}


/*
 * Initializes the game board's parameters for a new game.
 *
 * Board*		boardPtr	-	A pointer to a game board.
 * unsigned int	m			-	Number of rows in each block on the board.
 * unsigned int	m			-	Number of columns in each block on the board.
 *
 */
void initializeBoard(Board* boardPtr, unsigned int m, unsigned int n) {
	unsigned int i, j, k;
	unsigned int N = m*n;

	/* Free previously allocated space */
	if(boardPtr != NULL && boardPtr->m == m && boardPtr->n == n) {
		nullifyBoard(boardPtr);
		return; /* memory already allocated accordingly */
	}
	freeBoard(boardPtr);
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
        	 boardPtr->board[i][j].possible_vals = (unsigned int*)malloc((N+1) * sizeof(unsigned int));	/* Possible values */
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
			boardPtr->board[i][j].isErroneous = FALSE;
			for(k = 0; k < N; k++) {
				boardPtr->board[i][j].possible_vals[k] = TRUE;
			}
		}
	}

	boardPtr->cellsDisplayed = 0;
}


/*
 * Returns TRUE iff all cells are filled on a game board.
 *
 * Board	board	-	A game board.
 */
unsigned int isBoardComplete(Board board) {
	unsigned int N = board.m*board.n;
	return board.cellsDisplayed == N*N;
}


/*
 * Returns TRUE iff all cells are Empty on a game board.
 *
 * Board	board	-	A game board.
 */
unsigned int	isBoardEmpty(Board board){
	return board.cellsDisplayed == 0;
}


/*
 * Copies the contents of the original board to the copied board.
 * Assumes both boards are of equal sizes.
 *
 * Board*	original	-	Original board pointer.
 * Board*	copied		-	Copied board pointer.
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


/*
 * Returns a pointer to gameBoard.
 */
Board* getGameBoardPtr() {
	return &gameBoard;
}


/*
 * Returns a pointer to solutionBoard.
 */
Board* getSolutionBoardPtr() {
	return &solutionBoard;
}


/*
 * Checks if boardPtr->board has any erroneous cells.
 *
 * Board*	boardPtr	-	A pointer to a game board.
 */
unsigned int hasErrors(Board* boardPtr) {
	unsigned int N = boardPtr->m*boardPtr->n;
	unsigned int row, col;
	for(row = 0; row < N; row++) {
		for(col=0; col < N; col++) {
			if(getCell(boardPtr,row,col)->isErroneous) { /* found erroneous cell */
				return TRUE;
			}
		}
	}
	/* no erroneous cells found */
	return FALSE;
}


/* --------------- Game functions --------------- */


/*
 * Initializes a new empty game board and a solution board, frees previously allocated space used by these boards.
 *
 * Board*		gBoard	-	A pointer to the game board.
 * Board*		sBoard	-	A pointer to the solution board.
 * unsigned int	m		-	number of rows in each block on the board.
 * unsigned int	n		-	number of columns in each block on the board.
 */
void initializeGame(Board* gBoard, Board* sBoard, unsigned int m, unsigned int n) {
	gameBoard = (*gBoard);					/* copy gameBoard to Game.c */
	solutionBoard = (*sBoard);				/* copy solutionBoard to Game.c */
	/* initialize the two boards */
	initializeBoard(&gameBoard,m,n);
	initializeBoard(&solutionBoard,m,n);
}


/*
 * Updates all cells that may have changed from erroneous to not erroneous or the other way around,
 * given that cell[row][col] of boardPtr->board has changed its value.
 *
 * Board*	boardPtr	-	A pointer to a game board.
 * unsigned int	col		-	Column number (between 0 and N-1).
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int	lastVal	-	The previous value of cell[row][col] of boardPtr->board (before its value has changed)
 */
void updateErroneous(Board* boardPtr, unsigned int row, unsigned int col, unsigned int lastVal) {
	unsigned int i,j, count_i, count_j;
	unsigned int m = boardPtr->m, n = boardPtr->n;
	unsigned int N = m*n;
	Cell* cell;

	/* Row */
	for(j = 0; j < N; j++) {
		cell = getCell(boardPtr,row,j);
		cell->isErroneous = isErroneous(boardPtr, row, j);
		if(cell->value == lastVal) { /* Might have been erroneous and but now isn't now. check if erroneous. */
			cell->isErroneous = isErroneous(boardPtr,row,j);
		}
	}

	/* Column */
	for(i = 0; i < N; i++) {
		cell = getCell(boardPtr,i,col);
		cell->isErroneous = isErroneous(boardPtr,i,col);
		if(cell->value == lastVal) { /* Might have been erroneous and but now isn't now. check if erroneous. */
			cell->isErroneous = isErroneous(boardPtr,i,col);
		}
	}

	/* Block */
	i = m*((row)/m); /* Index of the first row of the block */
	j = n*((col)/n); /* Index of the first column of the block */
	for(count_i = 0; count_i < m; count_i++) {
		for(count_j = 0; count_j < n; count_j++) {
			cell = getCell(boardPtr,i+count_i,j+count_j);
			cell->isErroneous = isErroneous(boardPtr,i+count_i,j+count_j);
			if(cell->value == lastVal) {
				cell->isErroneous = isErroneous(boardPtr,i+count_i,j+count_j);
			}
		}
	}
}


/*
 * Sets the value of gameMode to newGameMode.
 *
 * unsigned int	newGameMode	-	The desired game mode.
 */
void setGameMode(unsigned int newGameMode) {
	gameMode = newGameMode;
}


/*
 * Returns the value of gameMode.
 */
unsigned int getGameMode() {
	return gameMode;
}


/*
 * Sets the value of markErrors to mark (assumes mark is 0 or 1).
 *
 * unsigned int	mark	-	the desired value of markErrors (assumed to be 0 or 1).
 */
void setMarkErrors(unsigned int mark) {
	markErrors = mark;
}


/*
 * Returns the value of markErrors.
 */
unsigned int getMarkErrors() {
	return markErrors;
}


/* --------------- Move-list functions --------------- */


/*
 * Clears all allocated memory used by the field moveList.
 */
void clearMoveList() {
	doubly_clear(moveList);
}


/*
 * Initializes a new empty move list (sets and autofills).
 */
void initializeMoveList() {
	if(moveList != NULL) {
		clearMoveList();
	}
	moveList = createNewDoublyLinkedList();
	curMove = NULL;
}


/*
 * Adds move to the move list.
 *
 * SinglyLinkedList		*move	-	Said move (a list of set(s)).
 */
void addMove(SinglyLinkedList* move) {
	doubly_removeAfter(moveList,curMove);
	doubly_addLast(moveList,move);
	curMove = doubly_getLastNode(moveList);
}


/*
 * Set the current move pointer to the previous move and update the board accordingly.
 * returns TRUE iff successful.
 *
 * unsigned int	toPrint	-	if TRUE, prints "Undo: X,Y: from Z1 to Z2\n".
 */
unsigned int undoMove(unsigned int toPrint) {
	unsigned int row, col, val, lastVal;
	SinglyLinkedList	*move;
	SinglyNode			*node;
	if(curMove == NULL) { /* No moves to undo */
		return FALSE;
	}
	move = curMove->data;
	curMove = curMove->prev;

	node = move->head;
	while(node != NULL) {
		row = node->data[0];
		col = node->data[1];
		val = node->data[2];
		lastVal = node->data[3];
		if(toPrint) {
			printf("Undo %d,%d: from %c to %c\n",col+1,row+1,val==0? '_':val+'0',lastVal==0? '_':lastVal+'0');
		}
		setCellVal(&gameBoard,row,col,lastVal);
		updateErroneous(&gameBoard, row, col, lastVal);
		node = node->next;
	}
	return TRUE;
}


/*
 * Set the current move pointer to the next move and update the board accordingly.
 * returns TRUE iff successful.
 */
unsigned int redoMove() {
	unsigned int row, col, val, lastVal;
	SinglyLinkedList	*move;
	SinglyNode			*node;
	if(curMove == NULL) {
		if(moveList != NULL && moveList->head != NULL) {
			curMove = moveList->head;
		}
		else { /* no moves to redo */
			return FALSE;
		}
	}
	else if(curMove->next == NULL) { /* no moves to redo */
		return FALSE;
	}
	else {
		curMove = curMove->next;
	}
	move = curMove->data;

	node = move->head;
	while(node != NULL) {
		row = node->data[0];
		col = node->data[1];
		val = node->data[2];
		lastVal = node->data[3];
		printf("Redo %d,%d: from %c to %c\n",col+1,row+1,lastVal==0? '_':lastVal+'0',val==0? '_':val+'0');
		setCellVal(&gameBoard,row,col,val);
		updateErroneous(&gameBoard, row, col, val);
		node = node->next;
	}
	return TRUE;
}


/*
 * Undo all moves, reverting the board to its original loaded state.
 */
void resetGame() {
	/* undo while undos are available */
	while(undoMove(FALSE));
	/* clear the move list */
	doubly_clear(moveList);
	/* create a new empty move list */
	moveList = createNewDoublyLinkedList();
	/* and set current move to NULL, as there were any moves on the cleared board yet */
	curMove = NULL;
}
