void			initGame();


void startNewGame(unsigned int, unsigned int, unsigned int);

/*
 * Given an interpreted command from the user - executes it and prints the result of the execution.
 * returns TRUE iff command executed successfully.
 *
 * unsigned int*	command		-	The already encoded user command (after interpretation).
 * char*			path		-	An file path (used by edit, solve, and save commands).
 */
unsigned int	executeCommand (int*, char*);
