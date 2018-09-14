/*---FileManager.c---
 * This module adds the functionality of working with files.
 * This functionality is being used in "executeSolve", "executeEdit", and "executeSave" user-commands.
 *
 * The module includes 2 functions:
 * 1 - saveBoard() : Saves board to given path.
 * 2-  loadBoard() : Loads board to *boardPtr from path
 */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Parser.h" /* import toInt() function */
#include "FileManager.h"


/*
 * Saves board to given path path.
 *
 * Board		board		-	A game board.
 * char*		path		-	The path to which the file will be saved to (including filename and extension).
 * unsigned int	gameMode	-	The current game mode.
 */
unsigned int saveBoard(Board board, char* path, unsigned int gameMode) {
	unsigned int	row, col;
	unsigned int	N;
	unsigned int	value;
	FILE 			*ofp;

	ofp = fopen(path, "w");
	if(ofp == NULL) {
		return FALSE;
	}

	/* First line: m n (block size) */
	fprintf(ofp, "%d %d\n", board.m, board.n);

	N = board.m * board.n;
	/* The board itself */
	for(row = 0; row < N; row++) {
		for(col = 0; col < N; col++) {
			value = getCell(&board,row,col)->value;
			/* print cell value */
			fprintf(ofp, "%d", value);
			/* Add a dot - '.', if cell is greater than zero and is fixed or we're in edit mode */
			if(value > 0 && (gameMode == EDIT || getCell(&board,row,col)->fixed)) {
				fprintf(ofp, ".");
			}
			/* Add a space character or a new line if we're in the last column */
				fprintf(ofp, "%c", col==N-1?'\n':' ');
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
		return FALSE;
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
	for(row = 0; row < N; row++) {
		for(col = 0; col < N; col++) {
			getCell(boardPtr,row,col)->isErroneous = isErroneous(boardPtr,row,col);
		}
	}
	return TRUE;
}
