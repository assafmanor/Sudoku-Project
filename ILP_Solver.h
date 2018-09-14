/*---ILP_Solver.h---
 * This module adds the functionality of linear programming.
 * This functionality is being used to solve puzzles and check possible solutions to them.
 * The module includes 1 function - ilpSolve()
 */

/*
 *  Solve a game board and update its solution board,
 *  using Integer Linear Programming (ILP) with the Gurobi library.
 *  return TRUE iff board is solvable.
 *
 *  Board*	boardPtr	-	A pointer a game board.
 *  Board*	solBoardPtr	-	A pointer to the solution board. used to update the solution board.
 */
int ilpSolve(Board*, Board*);
