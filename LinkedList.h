#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

/*--------------------- Singly linked list ----------------------*/
/*----- Will be used as the data for the doubly linked list -----*/


/* A structure for a singly linked list node */
typedef struct singly_node_t {
	unsigned int data[4]; /* row, col, val, and lastVal of a set command */
	struct singly_node_t *next;
} SinglyNode;


/* A structure for a singly linked list */
typedef struct singly_linked_list_t {
	unsigned int		size;	/* Number of nodes in list */
	SinglyNode			*head;	/* First node pointer */
	SinglyNode			*tail;	/* Last node pointer */
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
 * Removes the first node from the list and returns its data.
 * if the list is empty, returns a null pointer.
 *
 * SinglyLinkedList*	list	-	Said singly linked list.

 */
unsigned int* singly_removeFirst(SinglyLinkedList*);


/*
 * Removes all nodes from the list.
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
 * Removes the first node and returns a pointer to its data (a singly linked list).
 * if the list is empty, returns a null pointer.
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
 */
SinglyLinkedList* doubly_removeFirst(DoublyLinkedList*);


/*
 * Removes all nodes that appear after DoublyNode node, in list (not including).
 * Assumes node is in the linked list.
 *
 * DoublyLinkedList*	list	-	Said doubly linked list.
 * DoublyNode*			node	-	Said doubly linked list node.
 */
void doubly_removeAfter(DoublyLinkedList* list, DoublyNode *node);


/*
 * Removes all nodes from the list.
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
