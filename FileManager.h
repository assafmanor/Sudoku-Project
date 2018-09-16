/*---FileManager.h---
 * This module adds the functionality of working with files, ie. saving and loading puzzles to/from files.
 * This functionality is being used in "executeSolve", "executeEdit", and "executeSave" user-commands.
 *
 * The module includes 2 functions:
 * 1 - saveBoard()	:	Saves board to given path.
 * 2-  loadBoard()	:	Loads board to *boardPtr from path
 */

#include "Game.h"

/*
 * Saves board to given path path.
 * Returns: TRUE (1) - a successful save, FALSE (0) - Unable to open file on given path, INVALID (-1) - fprintf() error.
 *
 * Board		board		-	A game board.
 * char*		path		-	The path to which the file will be saved to (including filename and extension).
 * unsigned int	gameMode	-	The current game mode.
 */
int	saveBoard(Board, char*, unsigned int);


/*
 * Loads board to *boardPtr from path.
 * Assumes the file contains valid data and is correctly formatted (as instructed).
 *
 * Board		board		-	A game board.
 * char*		path		-	The path to which the file will be saved to (including file name and extension).
 * unsigned int	gameMode	-	The current game mode.
 */
int				loadBoard(Board*, char*, unsigned int);
