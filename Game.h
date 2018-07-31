#define	FALSE		0
#define	TRUE		1

/* Game modes */
#define INIT		0
#define SOLVE		1
#define EDIT		2


/*
 * Cell structure. The Sudoku board consists of N*N Cell's.
 */
typedef struct cell_t {
	unsigned int value;				/* Cell value*/
	unsigned int fixed;				/* TRUE or FALSE */
	unsigned int* possible_vals;	/* the possible values for this cell.
									 * ie. value val is possible iff:
									 * possible_vals[val-1] = 1
									 */
} Cell;

typedef struct board_t {
	Cell**	board;
	unsigned int cellsDisplayed;
	unsigned int m;
	unsigned int n;
} Board;


void initializeBoard(Board*, unsigned int m, unsigned int n);


void newGame(Board*, Board*, unsigned int, unsigned int);


void freeBoard(Board);


/*
 * Returns a pointer to the specified cell.
 *
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
 * Given a column and row of a cell - return TRUE iff cell is fixed (given as a hint in the beginning of a new game).
 *
 * unsigned int	col		-	Column number (between 1 and N).
 * unsigned int row		-	Row number (between 1 and N).
 */
unsigned int	isCellFixed(unsigned int, unsigned int);


/*
 * Given a column, row, and value - returns TRUE iff value is 0 or it is possible by the constrains of
 * the cells from the same row, column, or block.
 *
 * unsigned int	col		-	Column number (between 1 and N).
 * unsigned int	row		-	Row number (between 1 and N).
 * unsigned int	val		-	The value the user wants to assign to the cell. (Between 0 and N).
 *
 */
unsigned int	isSetValid(unsigned int, unsigned int, unsigned int);


/*
 * Returns TRUE iff all cells are filled.
 */
unsigned int	isBoardComplete(Board);


/*
 * Given a row a column and a new value, updates the possible values for the row,
 * column, and block.
 *
 * unsigned int	col		-	Column number (between 0 and N-1).
 * unsigned int	row		-	Row number (between 0 and N-1).
 * unsigned int	val		-	The value the user assigned to the cell. (Between 0 and N).
 */
void			updatePossibleValues(Board*, unsigned int, unsigned int, unsigned int);


/*
 * Assigns the value of val to cell[row-1][col-1].value (assuming value is possible),
 * updates the possible values of all the cells in the row, column and block,
 * and updates the number of cells displayed on the board.
 *
 * unsigned int	col		-	Column number (between 1 and N).
 * unsigned int	row		-	Row number (between 1 and N).
 * unsigned int	val		-	The value being assigned to the cell. (Between 0 and N).
 */
void			setCellVal(Board*, unsigned int, unsigned int, unsigned int);


/*
 * Prints the game board.
 */
void			printBoard(Board);


void			setGameMode(unsigned int);

unsigned int 	getGameMode();

/*
 * Copies the contents of the first board to the latter.
 */
void			copyBoard(Board*, Board*);


Board* getGameBoardPtr();

Board* getSolutionBoardPtr();
