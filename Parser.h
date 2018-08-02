#define MAX_INPUT_LENGTH	256
#define	INVALID				(-1)
/* TRUE and FALSE are defined in Game.h. */

/*
 * Use this whenever you want to get the user's input and ignore an input of whitespaces.
 *
 * char*	str		-	user input will be stored on this string (assumes memory was already allocated).
 */
void			getUserInput(char*);


/*
 * Converts a string to the integer it represents if it does represent an integer
 * and returns it (only non-negatives). otherwise - returns INVALID.
 *
 * char*	str	-	Said string.
 */
int				toInt(char*);


/*
 * @@@@@@@@@@NEEDS MORE DESCRIPTION (WILL BE ADDED LATER)@@@@@@@
 * -Reads user command and returns if valid or not.
 *
 * -updates array command such that:
 *
 * 	command[0] = :
 * 		1 - solve
 * 		2 - edit
 * 		3 - mark_errors
 * 		4 - print_board
 * 		5 - set
 * 		6 - validate
 * 		7 - generate
 * 		8 - undo
 * 		9 - redo
 * 		10 - save
 * 		11 - hint
 * 		12 - num_solutions
 * 		13 - autofill
 * 		14 - reset
 * 		15 - exit
 *
 *
 *	 if command[0] == 11 (hint):
 * 		command[1] = column number (1-9)
 * 		command[2] = row number (1-9)
 *
 *	 if command[0] == 5 (set):
 * 		command[1] = column number (1-9)
 * 		command[2] = row number (1-9)
 * 		command[3] = value (0-9)
 *
 * returns TRUE iff valid command.
 *
 * char*			input		-	User input.
 * unsigned int*	command		-	The encoded command will be stored on this array.
 */
unsigned int	interpretCommand (char* , int*, char*);
