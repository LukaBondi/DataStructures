/* matrilineal
 * 
 * 	Build a simple, non-sorted tree of mother-daughter relationships
 *  Template for Fundamental Data Structures Lab 2
 * 
 *  Created by Pasin Manurangsi, 2025-01-08
 * 	Modified by Luka Bond, 2025-02-13
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

/* --------------- Linked list-based stack implementation --------------- */

/* 
push: Add data (of any type) to the top of the stack.
@param _data: Pointer to data (of any type) to be stored
@param _stack: Pointer to the stack
*/
void push(void* _data, linkedlist_t* _stack)
{
	llnode_t* newNode = calloc(1, sizeof(llnode_t));
	if(newNode == NULL)
	{
		printf("Failed to allocate new node\n");
		exit(1);
	}

	/* Filling node attributes */
	newNode->data = _data; /* Fill data */
	newNode->nextNode = _stack->head; /* Next node is the current head */
	
	/* Stack is empty */
	if(_stack->tail == NULL)
	{
		_stack->tail = newNode;
	}
	_stack->head = newNode; /* Reassigning the head*/
}

/* 
pop: Remove data (of any type) from the top of stack and return it. 
@param _stack: Pointer to the stack
@return: Pointer to the allocated data
*/
void* pop(linkedlist_t* _stack)
{	
	if(_stack->head == NULL) return NULL;

	llnode_t* prevHead = _stack->head;
	void* item = (_stack->head)->data;

	/* Reassign new head */
	_stack->head = (_stack->head)->nextNode;
	if(_stack->head == NULL)
	{
		/* If the new head is null, tail must also be null */
		_stack->tail = NULL;
	}

	/* Free and set old pointer to NULL*/
	free(prevHead);
	prevHead = NULL;

	return item; /* Extract pointer to the data */
}

/* 
top: Return data (of any type) from the top of stack. 
@param _stack: Pointer to the stack
@return: Pointer to the allocated data
*/
void* top(linkedlist_t* _stack)
{	
	if(_stack->head == NULL) return NULL;
	return (_stack->head)->data; /* Extract pointer to the data */
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
DFSearch: Search for the node name in every node in the tree/subtree using Depth-First Traversal
@param _root: Pointer to the root node of the tree/subtree
@param _name: The name to search for in the tree/subtree
@return: Pointer to the first matching node (Leftest node in the tree)
*/
tnode_t* DFSearch(tnode_t* _root, char* _name)
{
	/* Non-existent node */
	if(_root == NULL) return NULL; 

	/* If node name matches, return it and stop exploration. */
	if(!strcmp(_root->name, _name)) 
	{
		return _root;
	} 

	/* Exits loop if unable to explore deeper than the depth of all child nodes */
	for (int i = 0; i < _root->numChildren; ++i)
	{
		tnode_t* searchedNode = DFSearch(&_root->children[i], _name);
		if(searchedNode != NULL)
		{
			return searchedNode;
		}
	}

	/* No matching node found */
	return NULL;
}

/* 
DFPrint: Print the string of every node in the tree/subtree using Depth-First Traversal
@param _root: Pointer to the root node of the tree/subtree
*/
void DFPrint(tnode_t* _root)
{
	/* Non-existent node */
	if(_root == NULL) return;

	/* Exits loop if unable to explore deeper than the depth of all child nodes */
	for (int i = 0; i < _root->numChildren; ++i)
	{
		DFPrint(&_root->children[i]);
	}

	/* Print the string contained in the node (Post-order) */
	printf("%s ", _root->name);
}

/* 
freeTree: Free all of the nodes from the tree/subtree using Depth-First Traversal (Recursion)
@param _root: Pointer to the root node of the tree/subtree
*/
void freeTree(tnode_t* _root)
{
	/* Non-existent node */
	if(_root == NULL) return;

	/* Exits loop if unable to explore deeper than the depth of all child nodes */
	for (int i = 0; i < _root->numChildren; ++i)
	{
		freeTree(&_root->children[i]);
	}

	/* Free the pointer to the array of children of the current subtree */
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
	tnode_t* motherNode = DFSearch(&treeRoot, motherName);
	if(motherNode == NULL)
	{
		printf("Unsuccessful\n");
		return;
	}
	printf("Added\n");
	insertChild(motherNode, name);
}

/* 
queryDescendant: Prints every daughter's name of the queried mother node.
@param name: The name of the mother node which will be quried.
*/
void queryDescendant(char* name)
{
	tnode_t* motherNode = DFSearch(&treeRoot, name);
	if(motherNode == NULL || motherNode->numChildren == 0)
	{
		printf("Not Found\n");
		return;
	}

	/* DFPrint prints the inputted node too, so we loop through the chlidren instead */
	for (int i = 0; i < motherNode->numChildren; ++i)
	{
		DFPrint(&motherNode->children[i]);
	}
	printf("\n");
}

/* 
DFDeepest: Search for the deepest node name in the tree/subtree using Depth-First Traversal
@param node: Pointer to the root node of the tree/subtree
@param depth: The current explored depth
@param maxDepth: The maximum depth explored within the tree/subtree
@param deepestNode: The pointer to the pointer to the deepest node (Node pointer updated constantly)
*/
void DFDeepest(tnode_t* node, int depth, int* maxDepth, tnode_t** deepestNode)
{
	/* Non-existent node */
	if(node == NULL) return;

	if(depth > *maxDepth)
	{
		*maxDepth = depth;
		*deepestNode = node;
	}

	/* Exits loop if unable to explore deeper than the depth of all child nodes */
	for (int i = 0; i < node->numChildren; ++i)
	{
		DFDeepest(&node->children[i], depth + 1, maxDepth, deepestNode);
	}
}


/* queryLongestDescendantChain: Prints the longest descendant chain in the tree from top to bottom. */
void queryLongestDescendantChain()
{
	int maxDepth = -1; /* Ensures that the tree root starts at depth 0*/
	tnode_t* deepestNode = NULL; /* Pointer to the deepest tree node */

	/* Finds the deepest node in the tree */
	DFDeepest(&treeRoot, 0, &maxDepth, &deepestNode);

	/* Create a stack of names */
	linkedlist_t chainStack = { NULL, NULL };
	
	/* Climb the chain of descendants until the tree root is reached */
	while(deepestNode != NULL)
	{
		push(&deepestNode->name, &chainStack); /* Push name string into the stack */
		deepestNode = deepestNode->parent; /* Climb up the chain */
	}
	
	/* Prints the descendant chain from top to bottom */
	while(chainStack.head != NULL)
	{
		char* name = pop(&chainStack);
		printf("%s ", name);
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
		scanf("\n%s", input);
		if (strcmp(input, "Add") == 0)
		{
			scanf(" %s %s", name, motherName);
			addNonRoot(name, motherName);
		}
		else if (strcmp(input, "Descendant") == 0)
		{
			scanf(" %s", name);
			queryDescendant(name);
		}
		else
		{
			queryLongestDescendantChain();
		}
	}	
	freeTree(&treeRoot); /* Free all nodes from the tree */
}