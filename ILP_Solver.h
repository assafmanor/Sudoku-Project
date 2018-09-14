/*---ILP_Solver.h---
 * This module uses the Gurobi Optimizer Integer Linear Programming solver.
 * It is being used in order to solve puzzles and check possible solutions to them.
 * The module includes one public function - ilpSolve().
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
