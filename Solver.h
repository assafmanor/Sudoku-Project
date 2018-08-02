#include "Game.h"

/*
 * Solve an empty board, and then update for each cell: val<-sug_val.
 *
 * unsigned int	numOfHints	-	The number of cells with the solution value revealed (given from user input).
 */
void			generateBoard(Board*, Board*, unsigned int);


/*
 * Updates the solutionBoardPtr->board according to gameBoardPtr->board.
 *
 * Board*	gameBoardPtr		-	A pointer to the game board.
 * Board*	solutionBoardPtr	-	A pointer to the solution board.
 */
void updateSolBoard(Board*, Board*);


/*
 * Checks if the current configuration of the game board is solvable.
 * If solvable and some cells' val field != sol_val - update relevant sol_val's.
 * Returns TRUE iff current configuration of game board is solvable.
 *
 * Board*	boardPtr		-	A pointer a game board.
 */
unsigned int	validate(Board*);


void autofill();
