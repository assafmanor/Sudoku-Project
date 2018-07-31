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
 * -Reads user command and returns if valid or not.
 *
 * -updates array command such that:
 *
 * 	command[0] = :
 * 		1 - validate
 * 		2 - restart
 * 		3 - exit
 * 		4 - hint
 * 		5 - set
 *
 *	 if command[0] == 4 (hint):
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
unsigned int	interpretCommand (char* , int*);
