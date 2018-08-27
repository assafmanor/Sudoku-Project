#include "LinkedList.h"

#define	FALSE		0
#define	TRUE		1

/* Game modes */
#define INIT		0
#define SOLVE		1
#define EDIT		2

/* --------------- Structures --------------- */
/*
 * Cell structure. The Sudoku board consists of N*N Cells.
 */
#ifndef CELL_H
#define CELL_H
typedef struct cell_t {
	unsigned int	value;				/* Cell value*/
	unsigned int	fixed;				/* TRUE or FALSE */
	unsigned int	isErroneous;		/* an indicator to "is the value of this cell erroneous?". */
	unsigned int*	possible_vals;		/* the possible values for this cell.
									 	 * ie. value val is possible iff:
									 	 * possible_vals[val-1] = 1
									 	 */
} Cell;
#endif


/*
 * Board structure.
 */
#ifndef BOARD_H
#define BOARD_H
typedef struct board_t {
	Cell**			board;						/* A two-dimentional array of cells */
	unsigned int	cellsDisplayed;				/* Number of cells with a value greater than zero */
	unsigned int	m;							/* number of rows in each block on the board.*/
	unsigned int	n;							/* number of columns in each block on the board. */
} Board;
#endif


/* --------------- Cell functions --------------- */


/*
 * Returns a pointer to the specified cell.
 *
 * Board*	boardPtr	-	A pointer to a game board.
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int col		-	Column number (between 0 and N-1).
 */
Cell*			getCell(Board*, unsigned int, unsigned int);


/*
 * Given a column and row of a cell - returns the solution value of the cell.
 *
 * unsigned int	col		-	Column number (between 1 and N).
 * unsigned int row		-	Row number (between 1 and N).
 */
unsigned int	getHint(unsigned int, unsigned int);


/*
 * Given a column and row of a cell - return TRUE iff cell is fixed in boardPtr->board (given as a hint in the beginning of a new game in solve mode).
 *
 * Board*	boardPtr	-	A pointer to a game board.
 * unsigned int	col		-	Column number (between 1 and N).
 * unsigned int row		-	Row number (between 1 and N).
 */
unsigned int	isCellFixed(Board*, unsigned int, unsigned int);


/*
 * Checks if cell[row][col] of boardPtr->board contains an erroneous value.
 *
 * Board*	boardPtr	-	A pointer to a game board.
 * unsigned int	col		-	Column number (between 0 and N-1).
 * unsigned int	row		-	Row number (between 0 and N-1).
 */
unsigned int	isErroneous(Board*, unsigned int, unsigned int);


/*
 * Given a row a column and a new value, updates the possible values for the row,
 * column, and block in boardPtr->board. Also marks as erroneous if necessary.
 *
 * Board*	boardPtr	-	A pointer to a game board.
 * unsigned int	col		-	Column number (between 0 and N-1).
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int	val		-	The value the user assigned to the cell. (Between 0 and N).
 */
void			updatePossibleValues(Board*, unsigned int, unsigned int, unsigned int);

/*
 * 1 - updates the erroneous values of all the cells in the row, column and block
 * 2 - updates the possible values of all the cells in the row, column and block
 * 3 - Assigns the value of val to cell[row][col]->value (assuming value is possible),
 * 4 - update the number of cells displayed (used for checking if game over)
 *
 * Board*	boardPtr	-	A pointer to a game board.
 * unsigned int	col		-	Column number (between 0 and N-1).
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int	val		-	The value being assigned to the cell. (Between 0 and N).
 */
void			setCellVal(Board*, unsigned int, unsigned int, unsigned int);


/* --------------- Board functions --------------- */


/*
 * Initializes the game board's parameters for a new game
 * with the same m and n, assuming memory has already been allocated.
 * we are not using setCellVal() because the current application is faster,
 *
 * Board*		boardPtr	-	A pointer to a game board.
 *
 */
void			nullifyBoard(Board*);

/*
 * Initializes the game board's parameters for a new game.
 *

 * Board*		boardPtr	-	A pointer to a game board.
 * unsigned int	m			-	Number of rows in each block on the board.
 * unsigned int	m			-	Number of columns in each block on the board.
 *
 */
void			initializeBoard(Board*, unsigned int m, unsigned int n);

/*
 * Frees all allocated space used by board
 *
 * Board*	boardPtr	-	A pointer to a game board.
 */
void			freeBoard(Board*);


/*
 * Returns TRUE iff all cells are filled on a game board.
 *
 * Board	board	-	A game board.
 */
unsigned int	isBoardComplete(Board);


/*
 * Returns TRUE iff all cells are empty on a game board.
 *
 * Board	board	-	A game board.
 */
unsigned int	isBoardEmpty(Board);


/*
 * Copies the contents of the original board to the copied board.
 * Assumes both boards are of equal sizes.
 *
 * Board*	original	-	Original board pointer.
 * Board*	copied		-	Copied board pointer.
 */
void			copyBoard(Board*, Board*);


/*
 * Returns a pointer to gameBoard.
 */
Board*			getGameBoardPtr();


/*
 * Returns a pointer to solutionBoard.
 */
Board*			getSolutionBoardPtr();

/*
 * Checks if boardPtr->board has any erroneous cells.
 *
 * Board*	boardPtr	-	A pointer to a game board.
 */
unsigned int	hasErrors(Board*);


/* --------------- Game functions --------------- */


/*
 * Initializes a new empty game board and a solution board, frees previously allocated space used by these boards.
 *
 * Board*		gBoard	-	A pointer to the game board.
 * Board*		sBoard	-	A pointer to the solution board.
 * unsigned int	m		-	number of rows in each block on the board.
 * unsigned int	m		-	number of columns in each block on the board.
 */
void 			initializeGame(Board*, Board*, unsigned int, unsigned int);

/*
 * Updates all cells that may have changed from erroneous to not erroneous or the other way around,
 * given that cell[row][col] of boardPtr->board has changed its value.
 *
 * Board*	boardPtr	-	A pointer to a game board.
 * unsigned int	col		-	Column number (between 0 and N-1).
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int	lastVal	-	The previous value of cell[row][col] of boardPtr->board (before its value has changed)
 */
void 			updateErroneous(Board*, unsigned int, unsigned int, unsigned int);

/*
 * Sets the value of gameMode to newGameMode.
 *
 * unsigned int	newGameMode	-	The desired game mode.
 */
void			setGameMode(unsigned int);

/*
 * Returns the value of gameMode.
 */
unsigned int 	getGameMode();

/*
 * Sets the value of markErrors to mark (assumes mark is 0 or 1).
 *
 * unsigned int	mark	-	the desired value of markErrors (assumed to be 0 or 1).
 */
void			setMarkErrors(unsigned int);

/*
 * Returns the value of markErrors.
 */
unsigned int	getMarkErrors();


/* --------------- Move-list functions --------------- */


/*
 * Clears all allocated memory used by the field moveList.
 */
void			clearMoveList();


/*
 * Initializes a new empty move list (sets and autofills).
 */
void			initializeMoveList();


/*
 * Adds move to the move list.
 *
 * SinglyLinkedList		*move	-	Said move (a list of set(s)).
 */
void			addMove(SinglyLinkedList*);


/*
 * Set the current move pointer to the previous move and update the board accordingly.
 * returns TRUE iff successful.
 */
unsigned int	undoMove(unsigned int);


/*
 * Set the current move pointer to the next move and update the board accordingly.
 * returns TRUE iff successful.
 */
unsigned int	redoMove();


/*
 * Undo all moves, reverting the board to its original loaded state.
 */
void			resetGame();
