/*---Solver.h---
 *  This module adds the functionality of solving a Sudoku board.
 *  It includes validating a board, auto-fill a board, check number of solution etc.
 *
 * Functions:
 * 	1.validate() 		:	Checks if the current configuration of the game board is solvable
 * 	2.isBoardComplete() :	Returns TRUE if all cells are filled on a game board and are not erroneous.
 * 	3.autofill() 		:	Fill cells which contain exactly a single legal value
 * 	4.numSolutions()   :	Calculates the number of solutions for the current board.
 * 	5.generate() 		:	Try to generate x cells at gameBoardPtr board.
 * 						   	try to solve with ilp(result at "solution_board" so we copy it to gameBoardPtr board).
 * 						   	finally deleting cells values until only y values left.
 *
 */

#include "Game.h"



/*
 * Checks if the current configuration of the game board is solvable.
 * If solvable and some cells' val field != sol_val - update relevant sol_val's.
 * Returns TRUE iff current configuration of game board is solvable.
 *
 *	Pre: board is not erroneous	(Checked in MainAux.c)
 * Board*	boardPtr		-	A pointer a game board.
 */
int				validate(Board*);


/*
 * Returns TRUE if all cells are filled on a game board and are not erroneous.
 * Returns FALSE if all cells are filled but there are erroneous values.
 * Returns -1 if not all cells are filled.
 *
 * Board*	boardPtr	-	A game board pointer.
 */
int				isBoardComplete(Board*);


/*
 * Fill cells which contain exactly a single legal value
 * pre: assume we are in Solve mode (Checked in MainAux.c)
 *
 * Board*	boardPtr		-	A pointer a game board.
 */
void			autofill(Board*);


/*
 * Calculates the number of solutions for the current board
 *
 * Board*	boardPtr		-	A pointer a game board.
 */
unsigned int	numSolutions(Board*);


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
unsigned int	generate(Board* ,int, int);
