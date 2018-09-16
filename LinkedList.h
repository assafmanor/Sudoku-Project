/*---LinkedList.h---
 *  This module adds two data structures: 1. Singly linked list   2. Doubly linked list.
 *  This header file contains the definition of four structs:
 *   1. SinglyNode			(singly_node_t)			:	Used to store a singly cell change. Its data is the cell coordinates, the new value and the last value of the cell.
 *   													Also contains a pointer to the next node.
 *   2. SinglyLinkedList	(singly_linked_list_t)	:	Used to store an entire move (ie. one or move cell changes).
 *   													Stores the size of the list, its head (first node), and its tail (last node).
 *   3. DoublyNode			(doubly_node_t)			:	Used to store An entire move (a singly linked list) as data, and contains a pointer to the next and previous nodes.
 *   4. DoublyLinkedList	(doubly_linked_list_t)	:	Used to store the entire move list of a puzzle.
 *   													Stores the size of the list, its head (first node/move), and its tail (last node/move).
 *
 *  The singly linked list is being used to store player's cell changes for one move.
 *  The doubly linked list is being used to store player's moves. Thus, each node of the doubly linked list is a singly linked list that stores one move.
 *
 *  It is important to emphasize that this module includes only the implementation of the two data structures, and does NOT implement the undo/redo/reset functionalities.
 *  These functionalities are implemented in the Game.c module.
 *
 * Functions:
 * A. Singly linked list:
 * 	1. singly_isEmpty() 			:	Checks if a singly linked list is empty (does not contain any nodes).
 * 	2. singly_addLast()				:	Adds a node at the end of the list.
 *	3. singly_removeFirst()			:	Removes the first node of the and returns its data, also frees allocated memory. If the list is empty - returns a null pointer.
 *	4. singly_clear()				:	Removes all nodes from the list and frees all allocated space that was used by the list.
 *	5. createNewSinglyLinkedList()	:	Creates a new empty singly linked list. Returns a pointer to the list.
 *
 * B. Doubly linked list:
 * 	1. doubly_createNewNode() 		:	Creates a new DoublyLinkedList node (a SinglyLinkedList) and returns a pointer to it.
 * 	2. doubly_isEmpty() 			:	Checks if a doubly linked list is empty (does not contains any nodes).
 * 	3. doubly_addLast()				:	Adds a new node to the end of the list (as tail).
 * 	4. doubly_removeAfter()			:	Removes all nodes that appear after a given node, in a given doubly linked list (not including the given node).
 * 	5. doubly_clear()				:	Removes all nodes from the list, and frees all allocated memory used by the list.
 *	6. doubly_getLastNode()			:	Returns the last node in the doubly linked list, if exists.
 *	7. createNewDoublyLinkedList()	:	Creates a new empty doubly linked list. Returns a pointer to the list.
 */


#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

/*--------------------- Singly linked list ----------------------*/
/*----- Will be used as the data for the doubly linked list -----*/


/* A structure for a singly linked list node */
typedef struct singly_node_t {
	unsigned int	data[4];				/* row, col, val, and lastVal of a set command */
	struct			singly_node_t *next;
} SinglyNode;


/* A structure for a singly linked list */
typedef struct singly_linked_list_t {
	unsigned int		size;				/* Number of nodes in list */
	SinglyNode			*head;				/* First node pointer */
	SinglyNode			*tail;				/* Last node pointer */
} SinglyLinkedList;


/*
 * Returns TRUE iff singly linked list is empty
 *
 * SinglyLinkedList*		list	-	Said singly linked list.
*/
int singly_isEmpty(SinglyLinkedList*);


/*
 * Adds a new node with data corresponding to <row,col,val> to the end of the list (as tail).
 *
 * SinglyLinkedList*	list	-	Said singly linked list.
 * unsigned int			row		-	Row number (between 0 and N-1).
 * unsigned int 		col		-	Column number (between 0 and N-1).
 * unsigned int			val		-	The value being assigned to the cell. (Between 0 and N).
 * unsigned int			lastVal	-	The last value that cell had.
 */
void singly_addLast(SinglyLinkedList*, unsigned int, unsigned int, unsigned int, unsigned int);


/*
 * Removes the first node from the list and returns its data, and frees allocated space.
 * if the list is empty, returns a null pointer.
 *
 * SinglyLinkedList*	list	-	Said singly linked list.
 */
unsigned int* singly_removeFirst(SinglyLinkedList*);


/*
 * Removes all nodes from the list, and frees all allocated space that was used by the list.
 *
 * SinglyLinkedList*	list	-	Said singly linked list.
*/
void singly_clear(SinglyLinkedList* list);


/*
 * Creates a new empty singly linked list.
 * Returns a pointer to the list.
 */
SinglyLinkedList* createNewSinglyLinkedList();

/*------------------ End of singly linked list ------------------*/


/*--------------------- Doubly linked list ----------------------*/

/* A structure for a doubly linked list node */
typedef struct doublyNode_t {
	SinglyLinkedList	*data;
	struct doublyNode_t	*next;
	struct doublyNode_t	*prev;

} DoublyNode;


/* A structure for a singly linked list */
typedef struct doubly_linked_list_t {
	unsigned int		size;	/* Number of nodes in list */
	DoublyNode			*head;	/* First node pointer */
	DoublyNode			*tail;	/* Last node pointer */

} DoublyLinkedList;


/*
 * Returns TRUE iff doubly linked list is empty
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
 *
*/
int doubly_isEmpty(DoublyLinkedList*);


/*
 * Adds a new node to the end of the list (as tail).
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
 * SinglyLinkedList*	data	-	Data of the new node.
 */
void doubly_addLast(DoublyLinkedList*, SinglyLinkedList*);


/*
 * Removes all nodes that appear after DoublyNode node, in list (not including), and frees all allocated memory used by all removed nodes.
 * *** Assumes that the given node is one of the nodes of the linked list. ***
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
 * DoublyNode*			node	-	Said doubly linked list node.
 */
void doubly_removeAfter(DoublyLinkedList* list, DoublyNode *node);


/*
 * Removes all nodes from the list, and frees all allocated memory used by the list.
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
*/
void doubly_clear(DoublyLinkedList*);


/*
 * Returns the last node in the doubly linked list, if exists.
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
 */
DoublyNode* doubly_getLastNode(DoublyLinkedList* list);


/*
 * Creates a new empty doubly linked list.
 * Returns a pointer to the list.
 */
DoublyLinkedList* createNewDoublyLinkedList();

/*------------------ End of doubly linked list ------------------*/

#endif
