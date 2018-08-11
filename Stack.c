#include <stdlib.h> /* for malloc */
#include <stdio.h>	/* for error printing */
#include "Game.h"   /* for cell structure */
#include "Stack.h"  /* for stack,info structures */

/*------------------- stack functions-----------------------------*/
int isEmpty(struct StackNode *root)
{
    return !root;
}

/* First, adjust current cell before pushing it.
 * Then, it push the current cell to the stack.*/
void push(struct StackNode** root, info** cd)
{
	unsigned int last_value;
	struct StackNode* stackNode;

	/* Step1: Adjust cell before pushing it*/
	last_value = (*cd)->sug_cell->value-1;
	(*cd)->possible[last_value]  			 = FALSE;	/* cell old value is not possible anymore */
	(*cd)->posValsCount 					-=	1;
	(*cd)->k			 					+=	1;		/* push interrupt the for loop, so we append k manually */
	(*cd)->first_time 						 = FALSE;

	/* Step2: Allocate memory safely for new stack node.
	 * we free this memory at pop() */
    stackNode = (struct StackNode*) malloc(sizeof(struct StackNode));
	if(stackNode == NULL) {printf("Error: malloc has failed\n");exit(1);}

	/* Step3: Push the cell */
	stackNode->cell_info.row			= (*cd)->row;
    stackNode->cell_info.col			= (*cd)->col;
	stackNode->cell_info.k				= (*cd)->k;
	stackNode->cell_info.possible 		= (*cd)->possible;
	stackNode->cell_info.posValsCount	= (*cd)->posValsCount;
	stackNode->cell_info.counter		= (*cd)->counter;
	stackNode->cell_info.orig_cell		= (*cd)->orig_cell;
	stackNode->cell_info.sug_cell		= (*cd)->sug_cell;
	stackNode->cell_info.first_time		= (*cd)->first_time;
    stackNode->next 					= *root;
    /* Update Stack pointer */
    *root 								= stackNode;

}


/* This function takes the values of the popped stackNode, and
 * initialize them as the current cell.
 * this way,the next while loop iteration will
 * handle the popped stack node.
 * pre: we don't pop NULL, but a actual stackNode */
void pop(struct StackNode** root, info** cd)
{
	/* step 1: pop the stack node to 'top' */
	struct StackNode* top;
    top = *root;
    *root = (*root)->next;

    /* if cell was empty originally, clear it before popping */
	if((*cd)->orig_cell->value == 0) {(*cd)->sug_cell->value	= 0;} /*problematic line*/

    /* step 2: copy top's info to current cell */
	(*cd)->row 				= (top->cell_info.row);
	(*cd)->col 				= (top->cell_info.col);
	(*cd)->k				= (top->cell_info.k);
	(*cd)->possible			= (top->cell_info.possible);
	(*cd)->posValsCount		= (top->cell_info.posValsCount);
	(*cd)->counter			+= (top->cell_info.counter);
	(*cd)->orig_cell		= (top->cell_info.orig_cell);
	(*cd)->sug_cell			= (top->cell_info.sug_cell);
	(*cd)->first_time		= (top->cell_info.first_time);

	free(top); /* kill just popped stack node */
    /* we don't free "top->possible" because we use it as: "(*cd)->possible" */
}
