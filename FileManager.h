#include "Game.h"

/*
 * Saves board to given path path.
 *
 * Board		board		-	A game board.
 * char*		path		-	The path to which the file will be saved to (including file name and extension).
 * unsigned int	gameMode	-	The current game mode.
 */
unsigned int	saveBoard(Board, char*, unsigned int);


/*
 * Loads board to *boardPtr from path.
 * Assumes the file contains valid data and is correctly formatted (as instructed).
 *
 * Board		board		-	A game board.
 * char*		path		-	The path to which the file will be saved to (including file name and extension).
 * unsigned int	gameMode	-	The current game mode.
 */
int				loadBoard(Board*, char*, unsigned int);
