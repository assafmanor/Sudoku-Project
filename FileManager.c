/*---FileManager.c---
 * This module adds the functionality of working with files, ie. saving and loading puzzles to/from files.
 * This functionality is being used in "executeSolve", "executeEdit", and "executeSave" user-commands.
 *
 * A. Private functions:
 *  1. fprintfFailureErrorHandling()	:	Prints an error message and closes the given file.
 *  2. markAllErroneousCells()			:	Iterates over all of the given game board and marks all erroneous cells.
 *
 * B. Public functions:
 *  1 - saveBoard()						:	Saves board to given path.
 *  2-  loadBoard()						:	Loads board to *boardPtr from path
 */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Parser.h" /* import toInt() function */
#include "FileManager.h"


/********** Private method declarations **********/

void fprintfFailureErrorHandling(FILE*);
void markAllErroneousCells(Board*);

/******* End of private method declarations ******/


/************************* Public methods *************************/

/*
 * Saves board to given path path.
 * Returns: TRUE (1) - a successful save, FALSE (0) - Unable to open file on given path, INVALID (-1) - fprintf() error.
 *
 * Board		board		-	A game board.
 * char*		path		-	The path to which the file will be saved to (including filename and extension).
 * unsigned int	gameMode	-	The current game mode.
 */
int saveBoard(Board board, char* path, unsigned int gameMode) {
	unsigned int	row, col;
	unsigned int	N;
	unsigned int	value;
	FILE 			*ofp;

	ofp = fopen(path, "w");
	if(ofp == NULL) {
		return FALSE; /* no need to call fclose(), as fopen() failed */
	}

	/* First line: m n (block size) */
	if(fprintf(ofp, "%d %d\n", board.m, board.n) < 0) {
		fprintfFailureErrorHandling(ofp);
		return INVALID;
	}

	N = board.m * board.n;
	/* The board itself */
	for(row = 0; row < N; row++) {
		for(col = 0; col < N; col++) {
			value = getCell(&board,row,col)->value;
			/* print cell value */
			if(fprintf(ofp, "%d", value) < 0) {
				fprintfFailureErrorHandling(ofp);
				return INVALID;
			}
			/* Add a dot - '.', if cell is greater than zero and is fixed or we're in edit mode */
			if(value > 0 && (gameMode == EDIT || getCell(&board,row,col)->fixed)) {
				if(fprintf(ofp, ".") < 0) {
					fprintfFailureErrorHandling(ofp);
					return INVALID;
				}
			}
			/* Add a space character or a new line if we're in the last column */
				if(fprintf(ofp, "%c", col==N-1?'\n':' ') < 0){
					fprintfFailureErrorHandling(ofp);
					return INVALID;
				}
		}
	}
	/* Done writing to the file - close it */
	fclose(ofp);
	return TRUE;
}



/*
 * Loads board to *boardPtr from path.
 * Assumes the file contains valid data and is correctly formatted (as instructed).
 *
 * Board		board		-	A game board.
 * char*		path		-	The path to which the file will be saved to (including file name and extension).
 * unsigned int	gameMode	-	The current game mode.
 */
int loadBoard(Board* boardPtr, char* path, unsigned int gameMode) {
	unsigned int	row, col;
	int				m,n;
	unsigned int	N;
	unsigned int	value;
	char			ch;
	char			str_val[3];
	FILE			*ifp;

	str_val[2] = '\0';

	ifp = fopen(path, "r");
	if(ifp == NULL) {
		return FALSE; /* no need to call fclose(), as fopen() failed */
	}

	/* First, get m and n */
	if(!fscanf(ifp,"%d",&m)) {
		printf("Error: fscanf has failed\n");
		exit(1);
	}
	if(!fscanf(ifp,"%d",&n)) {
		printf("Error: fscanf has failed\n");
		exit(1);
	}
	/* Then initialize board (space allocation etc.) */
	initializeBoard(boardPtr,m,n);
	/* Scan through the rest of the file to get values of all cells */
	N = m*n;
	for(row = 0; row < N; row++) {
		for(col = 0; col < N; col++) {
			while((ch = fgetc(ifp)) != EOF && !isdigit(ch)); /* find first digit of a cell's value */
			if(ferror(ifp)) {
				printf("Error: fgetc has failed\n");
				exit(1);
			}
			if(ch != EOF) {
				str_val[0] = ch;
				if((ch = fgetc(ifp)) != EOF && isdigit(ch)) { /* has another digit */
					str_val[1] = ch;
					ch = fgetc(ifp); /* Scan the next digit (look for a '.') */
				}
				else { /* only one digit */
					str_val[1] = '\0';
				}
				if(ferror(ifp)) {
					printf("Error: fgetc has failed\n");
					exit(1);
				}

				value = toInt(str_val);
				getCell(boardPtr,row,col)->value = value;
				if(ch == '.' && gameMode == SOLVE) { /* check if fixed */
					getCell(boardPtr,row,col)->fixed = TRUE;
				}
				if(value > 0) {
					boardPtr->cellsDisplayed++;
				}
			}
		}
	}
	/* Done reading from the file - close it */
	fclose(ifp);
	/* Check for erroneous cells */
	markAllErroneousCells(boardPtr);
	return TRUE;
}

/********************** End of public methods *********************/



/************************* Private methods *************************/

/*
 * Prints an error message and closes the given file.
 * Assumes fprintf failure.
 *
 * FILE*	file	-	The given file needed to be closed.
 */
void fprintfFailureErrorHandling(FILE* file) {
	printf("Error: fprintf has failed\n");
	fclose(file);
}



/*
 * Iterates over all of the given game board and marks all erroneous cells.
 *
 * Board*	boardPtr	-	The given board pointer.
 */
void markAllErroneousCells(Board* boardPtr) {
	unsigned int	row, col;
	unsigned int	i,j, count_i, count_j;
	unsigned int	m = boardPtr->m, n = boardPtr->n;
	unsigned int	N = m*n;
	unsigned int	val;
	Cell			*curCell, *otherCell;

	for(row = 0; row < N; row++) {
		for(col = 0; col < N; col++) {
			curCell = getCell(boardPtr,row,col);
			val = curCell->value; /* current cell's value */
			if(val == 0 || curCell->isErroneous) { /* cell can't be erroneous if empty */
				continue; /* continue to next cell */
			}
			/* Row */
			for(j = 0; j < N; j++) {
				if(j == col) continue;
				otherCell = getCell(boardPtr,row,j);
				if(otherCell->value == val) { /* both cells are erroneous (whether the cells are fixed or not is irrelevant) */
					curCell->isErroneous 	= TRUE;
					otherCell->isErroneous 	= TRUE;
				}
			}

			/* Column */
			for(i = 0; i < N; i++) {
				if(i == row) continue;
				otherCell = getCell(boardPtr,i,col);
				if(otherCell->value == val) { /* both cells are erroneous (whether the cells are fixed or not is irrelevant) */
					curCell->isErroneous 	= TRUE;
					otherCell->isErroneous 	= TRUE;
				}
			}

			/* Block */
			i = m*((row)/m); /* Index of the first row of the block */
			j = n*((col)/n); /* Index of the first column of the block */
			for(count_i = 0; count_i < m; count_i++) {
				for(count_j = 0; count_j < n; count_j++) {
					if(i+count_i == row && j+count_j == col) continue;
					otherCell = getCell(boardPtr,i+count_i,j+count_j);
					if(otherCell->value == val) { /* both cells are erroneous (whether the cells are fixed or not is irrelevant) */
						curCell->isErroneous 	= TRUE;
						otherCell->isErroneous 	= TRUE;
					}
				}
			}
		}
	}
	return;
}

/********************** End of private methods *********************/
