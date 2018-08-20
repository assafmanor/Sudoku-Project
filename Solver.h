#include "Game.h"

/*
 * Solve an empty board, and then update for each cell: val<-sug_val.
 *
 * unsigned int	numOfHints	-	The number of cells with the solution value revealed (given from user input).
 */
/*void			generateBoard(Board*, Board*, unsigned int);*/


/*
 * Updates the solutionBoardPtr->board according to gameBoardPtr->board.
 *
 * Board*	gameBoardPtr		-	A pointer to the game board.
 * Board*	solutionBoardPtr	-	A pointer to the solution board.
 */
/*void updateSolBoard(Board*, Board*);*/


/*
 * Checks if the current configuration of the game board is solvable.
 * If solvable and some cells' val field != sol_val - update relevant sol_val's.
 * Returns TRUE iff current configuration of game board is solvable.
 *
 *	Pre: board is not erroneous	(Checked in MainAux.c)
 * Board*	boardPtr		-	A pointer a game board.
 */
unsigned int validate(Board*);


/*
 * Fill cells which contain exactly a single legal value
 * pre: assume we are in Solve mode (Checked in MainAux.c)
 *
 * Board*	boardPtr		-	A pointer a game board.
 */
unsigned int autofill(Board*);


/*
 * Calculates the next cell to be checked, and
 * init only the relevant variables, so next while loop iteration
 * will work on the (new) next cell.
 *
 * Board*	boardPtr		-	A pointer a game board.
 */
unsigned int num_solutions(Board*);


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
unsigned int generate(Board* ,int, int);
