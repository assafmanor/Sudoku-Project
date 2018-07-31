#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Parser.h"
#include "Game.h"

#define	INVALID	(-1)

/* TRUE,FALSE, N, BLOCK_ROWS, and BLOCK_COLS are defined in Game.h. */


/*
 * Returns TRUE iff str1 and str2 contain exactly the same chars.
 *
 * char*	str1	-	First string to compare.
 * char*	str2	-	Second string to compare.
 */
unsigned int stringsEqual(char* str1, char* str2) {
	return strcmp(str1,str2) == 0;
}


/* Determines whether a string consists only of empty spaces.
 *
 * char*	str		-	Said string.
 */
unsigned int isEmptyInput(char* str) {
	unsigned int	i;

	if(strlen(str) == 0) {
		return TRUE;
	}
	for(i = 0; i < strlen(str); i++) {
		if(str[i] != '\n' && str[i] != '\t' && str[i] != ' ' && str[i] != '\r') {
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * Use this whenever you want to get the user's input and ignore an input of whitespaces.
 *
 * char*	str		-	user input will be stored on this string (assumes memory was already allocated).
 */
void getUserInput(char* str) {
	do {
		if(fgets(str,1024,stdin) == FALSE) { /* Reached EOF */
			printf("Exiting...\n");
			exit(0);
		}
		if(str == NULL) {
			printf("Error: fgets has failed\n");
			exit(1);
		}
	}
	while(isEmptyInput(str));
}


/*
 * Splits each word of user input to a separate cell of an array of strings.
 *
 * char*	input	-	User input.
 * char**	output	-	An empty array of strings. Will store each individual word on separate cells of the array.
 */
void commandToArray(char* input, char** output) {
	unsigned int	i = 0;
	char*			tok;

	tok = strtok(input," \t\r\n");
	while(i < 4 && tok != NULL) {
		output[i] = (char*) calloc(strlen(tok)+1,sizeof(char));
		if(output[i] == NULL) {
			printf("Error: calloc has failed\n");
			exit(1);
		}
		strcpy(output[i],tok); /* copy current word to output[i] */
		tok = strtok(NULL," \t\r\n");  /* go to next word */
		i++;
	}
}

/*
 * Converts a string to the integer it represents if it does represent an integer
 * and returns it (only non-negatives). otherwise - returns INVALID.
 *
 * char*	str	-	Said string.
 */
int toInt(char* str) {
	unsigned int	len;
	int				i;
	unsigned int	result = 0;
	unsigned int	factor = 1;
	char*			str_no_ws; /* Stores the first word appearing in str, ignoring whitespaces. */

	str_no_ws = strtok(str," \t\r\n"); /* Used to ignore unnecessary whitespaces */
	if(str_no_ws == NULL) { /* String is empty */
		return INVALID;
	}
	len = strlen(str_no_ws);
	for(i = len-1; i >= 0; i--) {
		if(str_no_ws[i] == '\n') {
			continue;
		}
		if(str_no_ws[i] < '0' || str_no_ws[i] > '9') { /* If not a decimal number */
			return INVALID;
		}
		result += (str_no_ws[i]-'0')*factor;
		factor *= 10;
	}
	return result;
}


/*
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
unsigned int interpretCommand (char* input, int* command) {
	unsigned int 	i1,i2,i3;
	unsigned int	i;
	char*			strArr[4] = {'\0'};
	unsigned int 	isValidCommand = TRUE;
	commandToArray(input,strArr); /* insert each one of the first four words of input into the array strArr */
	i1 = toInt(strArr[1]);
	i2 = toInt(strArr[2]);
	i3 = toInt(strArr[3]);
	if (stringsEqual(strArr[0],"solve")) {
		command[0] = 1;
		/*command[1] =*/
	}
/*	else if (stringsEqual(strArr[0],"restart")) {
			command[0] = 2;
	}*/
	else if (stringsEqual(strArr[0],"edit")) {
		command[0] = 2;

		/* //// Temporary ////
		 * For convenience only!
		 */
		command[1] = i1;
		command[2] = i2;
		command[3] = i3;
	}
	else if (stringsEqual(strArr[0],"mark_errors")) {
		command[0] = 3;
		command[1] = i1;
	}
	else if (stringsEqual(strArr[0],"print_board")) {
		command[0] = 4;
	}
	else if (stringsEqual(strArr[0],"set")) {
		command[0] = 5;
		command[1] = i1;
		command[2] = i2;
		command[3] = i3;
	}
	else if (stringsEqual(strArr[0],"validate")) {
		command[0] = 6;
	}
	else if (stringsEqual(strArr[0],"generate")) {
		command[0] = 7;
		command[1] = i1;
		command[2] = i2;
	}
	else if (stringsEqual(strArr[0],"undo")) {
		command[0] = 8;
	}
	else if (stringsEqual(strArr[0],"redo")) {
		command[0] = 9;
	}
	else if (stringsEqual(strArr[0],"save")) {
		command[0] = 10;
	}
	else if (stringsEqual(strArr[0],"hint")) {
		command[0] = 11;
		command[1] = i1;
		command[2] = i2;
	}
	else if (stringsEqual(strArr[0],"num_solutions")) {
		command[0] = 12;
	}
	else if (stringsEqual(strArr[0],"autofill")) {
		command[0] = 13;
	}
	else if (stringsEqual(strArr[0],"reset")) {
		command[0] = 14;
	}
	else if (stringsEqual(strArr[0],"exit")) {
		command[0] = 15;
	}
	/* TEMPORARY COMMAND!!!!! FOR TESTING */
	else if (stringsEqual(strArr[0],"game_mode")) {
		command[0] = 16;
	}
	else {
		isValidCommand = FALSE;
	}
	/* Free allocated memory */
	for(i = 0; i < 4; i++) {
		free(strArr[i]);
	}

	return isValidCommand;
}
