/*---Stack.h---
 * An implement of a stack for exhaustive-Backtracking in solver.c
 * Besides stack structure, also contains "info_t" -
 * A structure to represent the info in a stack node
 */

#include <stdio.h>
#include <stdlib.h>
#include "Game.h"

#ifndef STACK_H_
#define STACK_H_


/*------------------- stack structure-----------------------------*/
/* A structure to represent the info in a stack node*/
typedef struct info_t {
	unsigned int	row,col;		/* cell's row and col */
	unsigned int	k;				/* for empty cell only, the current iteration of possible values */
	unsigned int* 	possible;		/* calloc in init_cell(), free() in while loop */
	unsigned int	posValsCount;	/* Decreased by 1 in push() */
	unsigned int	counter;		/* increased in pop() */
	Cell*			orig_cell;
	Cell*			sug_cell;
	unsigned int    first_time;		/* set to TRUE in push(), set to FALSE in push() */
} info;

/* A structure to represent a stack node*/
struct StackNode
{
	info cell_info;
    struct StackNode* next;
};

int isEmpty(struct StackNode *);

void push(struct StackNode** , info** );

void pop(struct StackNode** , info** );





#endif /* STACK_H_ */
