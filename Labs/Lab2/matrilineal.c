/* matrilineal
 * 
 * Build a simple, non-sorted tree of mother-daughter relationships
 *  Template for Fundamental Data Structures Lab 2
 * 
 *  Created  by Pasin Manurangsi, 2025-01-08
 *  Modified by Luka Bond, 2025-02-03
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CHILDREN 10

/* --------------- Data structures --------------- */

/* Singly linked list node */
typedef struct _llnode
{
	void* data;
	struct _llnode* nextNode;
} llnode_t;

/* Linked List */
typedef struct _linkedlist
{
	llnode_t* head;
	llnode_t* tail;
} linkedlist_t;

/* Custom tree node */
typedef struct _tnode
{
	char name[25];
	struct _tnode* parent; /* The parent of the node */
	struct _tnode* children; /* Pointer to an array of children */
	int numChildren;
	int maxChildren; 
} tnode_t;

/* --------------- Global variables --------------- */

tnode_t treeRoot = { "", NULL, NULL, 0, MAX_CHILDREN };

/* --------------- Linked list-based Queue implementation --------------- */

/* 
Enqueue: Add data (of any type) to a specified queue.
@param _data: Pointer to data (of any type) to be stored
@param _queue: Pointer to the queue
*/
void enqueue(void* _data, linkedlist_t* _queue)
{
	llnode_t* newNode = calloc(1, sizeof(llnode_t));
	if(newNode == NULL)
	{
		printf("Failed to allocate new node\n");
		exit(1);
	}

	/* Filling node attributes */
	newNode->data = _data; /* Fill data */
	newNode->nextNode = NULL; /* No next node (new tail) */
	
	if(_queue->tail != NULL)
	{
		/* Next node of current tail is the new node */
		(_queue->tail)->nextNode = newNode;
	}

	/* Queue is empty */
	if(_queue->head == NULL)
	{
		_queue->head = newNode;
	}
	_queue->tail = newNode; /* Reassigning the tail*/
}

/* 
Dequeue: Remove data (of any type) from a specified queue.
@param _queue: Pointer to the queue
@return: Pointer to the allocated data
*/
void* dequeue(linkedlist_t* _queue)
{	
	if(_queue->head == NULL)
	{
		return NULL;
	}

	llnode_t* prevHead = _queue->head;
	void* item = (_queue->head)->data;
	

	/* Reassign new head */
	_queue->head = (_queue->head)->nextNode;

	/* Free and set old pointer to NULL*/
	free(prevHead);
	prevHead = NULL;

	return item;
}

/* 
front: Return data (of any type) from the front of the queue.
@param _queue: Pointer to the queue
@return: Pointer to the allocated data
*/
void* front(linkedlist_t* _queue)
{	
	if(_queue->head == NULL)
	{
		return NULL;
	}
	return (_queue->head)->data;
}

/* --------------- Custom array-based Tree implementation --------------- */

/* 
insertChild: Add a new child node to the specified parent node in the tree
@param _parent: Pointer to the parent node
@param _name: The string to be sotred in the child node
*/
void insertChild(tnode_t* _parent, char* _name)
{
	/* There can be no more than the max amount of children */
	if(_parent->numChildren == _parent->maxChildren)
	{
		printf("Cannot add more children to the current parent node\n");
		return;
	}

	/* If the parent is pointing to no children (NULL), create a new array of children */
	if(_parent->children == NULL)
	{
		/* Number of children will follow the max amount children from root */
		tnode_t* _children = calloc(_parent->maxChildren, sizeof(tnode_t));
		if(_children == NULL)
		{
			printf("Failed to allocate new array of tree nodes\n");
			exit(1);
		}
		_parent->children = _children; /* Reassign children pointer */
	}
	++_parent->numChildren; 

	/* Access the child at the back of the array */
	tnode_t* _child = &_parent->children[_parent->numChildren - 1];\
	strncpy(_child->name, _name, 20);
	_child->parent = _parent;
	_child->children = NULL; /* New node = no children */
	_child->numChildren = 0; 
	_child->maxChildren = _parent->maxChildren; /* Inherit from parent */
}

/* 
BFPrint: Print the string in every node in the tree/subtree 
		using Breadth-First Traversal
@param _root: Pointer to the root node of the tree/subtree
*/
void BFPrint(tnode_t* _root)
{
	linkedlist_t _queue = { NULL, NULL };
	enqueue(_root, &_queue); /* Store root node in queue */

	/* While queue is not empty */
	while(front(&_queue) != NULL)
	{
		tnode_t* frontNode = dequeue(&_queue);
		printf("%s ", frontNode->name);
		for (int i = 0; i < frontNode->numChildren; ++i)
		{
			/* Store children node in queue */
			enqueue(&frontNode->children[i], &_queue); 
		}
	}
	printf("\n");
}

/* 
BFSearch: Print the string in every node in the tree/subtree 
		using Breadth-First Traversal
@param _root: Pointer to the root node of the tree/subtree
@param _name: The name to search for in the tree/subtree
@return: Pointer to the first matching node
*/
tnode_t* BFSearch(tnode_t* _root, char* _name)
{
	linkedlist_t _queue = { NULL, NULL };
	enqueue(_root, &_queue); /* Store root node in queue */

	/* While queue is not empty */
	while(front(&_queue) != NULL)
	{
		tnode_t* frontNode = dequeue(&_queue);
		if(!strcmp(frontNode->name, _name))
		{
			/* Return node with the matching name */
			return frontNode; 
		}

		for (int i = 0; i < frontNode->numChildren; ++i)
		{
			/* Store children node in queue */
			enqueue(&frontNode->children[i], &_queue); 
		}
	}
	return NULL;
}

/* 
freeTree: Free all of the nodes from the tree/subtree 
		using Depth-First Traversal (Recursion)
@param _root: Pointer to the root node of the tree/subtree
*/
void freeTree(tnode_t* _root)
{
	/* No child encountered in the current node */
	if(_root == NULL)
	{
		return;
	}

	/* Exits loop if unable to explore deeper than the depth of all child nodes */
	for (int i = 0; i < _root->numChildren; ++i)
	{
		freeTree(&_root->children[i]);
	}

	/* Free the pointer to the array of children of the current subtree*/
	free(_root->children);
	_root->children = NULL;
}

/* --------------- Mother-daughter tree helper functions ---- ----------- */

/* 
addRoot: Adds person with a given name at the root.
@param name: The name of the person at the root
*/
void addRoot(char* name)
{
	printf("Added\n");
	strncpy(treeRoot.name, name, 20);
}


/* 
addNonRoot: Adds person with a given name and mother's name to the tree.
@param motherName: The name of the mother node which the child will be added to
*/
void addNonRoot(char* name, char* motherName)
{
	tnode_t* motherNode = BFSearch(&treeRoot, motherName);
	if(motherNode == NULL)
	{
		printf("Unsuccessful\n");
		return;
	}
	printf("Added\n");
	insertChild(motherNode, name);
}

/*
queryMother: Prints the Mother's name of the queried daughter node.
@param name: The name of the daughter node which will be quried.
*/
void queryMother(char* name)
{
	tnode_t* daughterNode = BFSearch(&treeRoot, name);
	if(daughterNode != NULL) /* Check if daughter exists */
	{
		if(daughterNode->parent != NULL) /* Check if the mother exists */
		{
			printf("%s\n", (daughterNode->parent)->name);
		}
	}
	printf("Not Found\n");
	return;
}

/*
queryMother: Prints every daughter's name of the queried mother node.
@param name: The name of the mother node which will be quried.
*/
void queryDaughters(char* name)
{
	tnode_t* motherNode = BFSearch(&treeRoot, name);
	if(motherNode == NULL)
	{
		printf("Not Found\n");
		return;
	}

	for (int i = 0; i < motherNode->numChildren; ++i)
	{
		printf("%s ", (motherNode->children[i]).name);
	}
	printf("\n");
}

/*
queryMother: Prints every sister's name of the queried daughter node.
@param name: The name of the daughter node which will be quried.
*/
void querySisters(char* name)
{
	tnode_t* daughterNode = BFSearch(&treeRoot, name);
	if(daughterNode == NULL)
	{
		printf("Not Found\n");
		return;
	}

	tnode_t* motherNode = daughterNode->parent;
	for (int i = 0; i < motherNode->numChildren; ++i)
	{
		char* sisterName = (motherNode->children[i]).name;
		if(!strcmp(sisterName, name))
		{
			continue; /* Skip the queried daughter */
		}
		printf("%s ", sisterName);
	}
	printf("\n");
}


int main()
{
	char input[64]; // Input operation name
	char name[25]; // The name of the woman
	char motherName[25]; // The number of woman's mother
	int numOperations; // The number of operations.
	scanf("%d", &numOperations);
	scanf("\n%s", name);
	addRoot(name);
	for (int i = 1 ; i < numOperations ; i ++)
	{
		scanf("\n%s ", input);
		if (strcmp(input, "Add") == 0)
		{
			scanf("%s %s", name, motherName);
			addNonRoot(name, motherName);
		}
		else if (strcmp(input, "Mother") == 0)
		{
			scanf("%s", name);
			queryMother(name);
		}
		else if (strcmp(input, "Daughters") == 0)
		{
			scanf("%s", name);
			queryDaughters(name);
		}
		else
		{
			scanf("%s", name);
			querySisters(name);
		}
	}
	freeTree(&treeRoot); /* Free all nodes from the tree */
}