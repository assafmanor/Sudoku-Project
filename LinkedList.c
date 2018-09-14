/*---LinkedList.c---
 *
 *
 */


#include "LinkedList.h"
#include <stdio.h>
#include <stdlib.h>

/*--------------------- Singly linked list ----------------------*/



/*
 * ********************** PRIVATE METHOD *************************
 * Creates a new SinglyLinkedList node and returns a pointer to it
 *
 * unsigned int		row		-	Row number (between 0 and N-1).
 * unsigned int 	col		-	Column number (between 0 and N-1).
 * unsigned int		val		-	The value being assigned to the cell. (Between 0 and N).
 * unsigned int		lastVal	-	The last value that cell had.
*/
SinglyNode* singly_createNewNode(unsigned int row, unsigned int col, unsigned int val, unsigned int lastVal) {
	SinglyNode *newNode = (SinglyNode*)malloc(sizeof(SinglyNode));
	if(newNode == NULL) {
		printf("Error: malloc has failed\n");
		exit(1);
	}
	newNode->data[0] = row;
	newNode->data[1] = col;
	newNode->data[2] = val;
	newNode->data[3] = lastVal;
	newNode->next = NULL;
	return newNode;
}


/*
 * Returns TRUE iff singly linked list is empty
 *
 * SinglyLinkedList*		list	-	Said singly linked list.
*/
int singly_isEmpty(SinglyLinkedList *list) {
	return list->size == 0; /* returns TRUE iff head == NULL */
}


/*
 * Adds a new node with data corresponding to <row,col,val> to the end of the list (as tail).
 *
 * SinglyLinkedList*	list	-	Said singly linked list.
 * unsigned int			row		-	Row number (between 0 and N-1).
 * unsigned int 		col		-	Column number (between 0 and N-1).
 * unsigned int			val		-	The value being assigned to the cell. (Between 0 and N).
 * unsigned int			lastVal	-	The last value that cell had.
 */
void singly_addLast(SinglyLinkedList *list,
			 	 	unsigned int row, unsigned int col, unsigned int val, unsigned int lastVal) {
	SinglyNode *newNode = singly_createNewNode(row,col,val,lastVal);
	SinglyNode *head = list->head;
	SinglyNode *tail = list->tail;
	SinglyNode *lastNode;
	if(singly_isEmpty(list)) {
		list->head = newNode;
	}
	else {
		lastNode = tail;
		if(tail == NULL) { /* Only head node */
			lastNode = head;
		}
		lastNode->next = newNode;	/* Add newNode after lastNode */
		list->tail = newNode;		/* Make newNode the new tail */
	}
	list->size++;	/* increment list size by one */
	return;
}


/*
 * Removes the first node from the list and returns its data.
 * if the list is empty, returns a null pointer.
 *
 * SinglyLinkedList*	list	-	Said singly linked list.
 */
unsigned int* singly_removeFirst(SinglyLinkedList* list) {
    SinglyNode *head = list->head;
    SinglyNode *next;
    unsigned int *data;

	if(singly_isEmpty(list)) {
		return NULL;
	}
	next = head->next;
	data = list->head->data;
	list->head = next;
	free(head);
	list->size--;	/* decrement list size by one */
	if(list->size <= 1) { /* list is empty or only head node */
		list->tail = NULL;
	}
	return data;
}

/*
 * Removes all nodes from the list.
 *
 * SinglyLinkedList*	list	-	Said singly linked list.
*/
void singly_clear(SinglyLinkedList* list) {
	while(!singly_isEmpty(list)) {
		singly_removeFirst(list);
	}
	free(list);
}

/*
 * Creates a new empty singly linked list.
 * Returns a pointer to the list.
 */
SinglyLinkedList* createNewSinglyLinkedList() {
	SinglyLinkedList *newNode = (SinglyLinkedList*)calloc(1,sizeof(SinglyLinkedList));
	if(newNode == NULL) {
		printf("Error: calloc has failed\n");
		exit(1);
	}
	return newNode;
}

/*------------------ End of singly linked list ------------------*/


/*--------------------- Doubly linked list ----------------------*/

/*
 * ********************** PRIVATE METHOD *************************
 * Creates a new DoublyLinkedList node (a SinglyLinkedList) and returns a pointer to it.
 *
 * SinglyLinkedList*	data	-	A singly linked list that would be added as data to the new doublyNode.
*/
DoublyNode* doubly_createNewNode(SinglyLinkedList *data) {
	DoublyNode *newNode = (DoublyNode*)calloc(1,sizeof(DoublyNode));
	if(newNode == NULL) {
		printf("Error: calloc has failed\n");
		exit(1);
	}
	newNode->data = data;
	return newNode;
}


/*
 * Returns TRUE iff doubly linked list is empty
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
*/
int doubly_isEmpty(DoublyLinkedList *list) {
	if(list == NULL) {
		return 1;
	}
	return list->size == 0; /* returns TRUE iff head == NULL */
}


/*
 * Adds a new node to the end of the list (as tail).
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
 * SinglyLinkedList*	data	-	Data of the new node.
 */
void doubly_addLast(DoublyLinkedList *list, SinglyLinkedList *data) {
	DoublyNode *newNode = doubly_createNewNode(data);
	DoublyNode *head = list->head;
	DoublyNode *tail = list->tail;
	DoublyNode *lastNode;
	if(doubly_isEmpty(list)) {
		list->head = newNode;
	}
	else {
		lastNode = tail;
		if(tail == NULL) { /* Only head node */
			lastNode = head;
		}
		lastNode->next = newNode;	/* Add newNode after lastNode */
		newNode->prev = lastNode;	/* Make lastNode - newNode's prev */
		list->tail = newNode;		/* Make newNode the new tail */
	}
	list->size++;	/* increment list size by one */
	return;
}


/*
 * Removes the last node in a doubly linked list.
 * if the list is empty, does nothing.
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
 */
void doubly_removeLast(DoublyLinkedList *list) {
    DoublyNode *last = doubly_getLastNode(list);
    DoublyNode *prevNode;

	if(doubly_isEmpty(list)) {
		return;
	}
	prevNode = last->prev;
	list->tail = prevNode;
	if(prevNode != NULL) { /* has next */
		prevNode->next = NULL;
	}

	/* Free allocated memory */
	singly_clear(last->data);
	free(last);

	list->size--;	/* decrement list size by one */
	if(list->size <= 1) { /* list is empty or only head node */
		list->tail = NULL;
	}
	if(list->size == 0) { /* list is empty */
		list->head = NULL;
	}
	return;
}


/*
 * Removes all nodes that appear after DoublyNode node, in list (not including).
 * *** Assumes that the given node is one of the nodes of the linked list. ***
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
 * DoublyNode*			node	-	Said doubly linked list node.
 */
void doubly_removeAfter(DoublyLinkedList* list, DoublyNode *node) {
	DoublyNode *tempNode = doubly_getLastNode(list);
	if(tempNode == NULL) { /* list is empty */
		return;
	}
	while(tempNode != node) {
		doubly_removeLast(list);
		tempNode = doubly_getLastNode(list);
	}
}


/*
 * Removes all nodes from the list.
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
*/
void doubly_clear(DoublyLinkedList* list) {
	while(!doubly_isEmpty(list)) {
		doubly_removeLast(list);

	}
	if(list != NULL) {
		free(list);
	}
}


/*
 * Returns the last node in the doubly linked list, if exists.
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
 */
DoublyNode* doubly_getLastNode(DoublyLinkedList* list) {
	if(doubly_isEmpty(list)) { /* no nodes */
		return NULL;
	}
	if(list->tail == NULL) { /* no tail */
		return list->head;
	}
	return list->tail; /* otherwise, tail is the last node */
}


/*
 * Creates a new empty doubly linked list.
 * Returns a pointer to the list.
 */
DoublyLinkedList* createNewDoublyLinkedList() {
	DoublyLinkedList *newNode = (DoublyLinkedList*)calloc(1,sizeof(DoublyLinkedList));
	if(newNode == NULL) {
		printf("Error: calloc has failed\n");
		exit(1);
	}
	return newNode;
}

/*------------------ End of doubly linked list ------------------*/
