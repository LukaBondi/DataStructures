/*  AICE Tree
 * 
 *  Build a sorted binary tree for competency information
 *
 *  Template for Fundamental Data Structures Lab 3
 *  Created  by Chavakorn Arunkunarax and Phasit Thanitkul, 2025-01-27
 *  Modified by Luka Bond, 2025-02-26
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --------------- Data structures --------------- */

/* Binary tree node */
typedef struct _tnode
{
	void* data;
	struct _tnode* parent; /* The parent of the node */
	struct _tnode* leftChild; /* Pointer to an array of children */
	struct _tnode* rightChild; /* Pointer to an array of children */
} tnode_t;

/* Competency info structure */
typedef struct _info
{
    char code[8]; 
    char title[64];
    unsigned int credit;
    unsigned int year;
    int semester; /* Spring = 0, Fall = 1 */
} info_t;

/* --------------- Global variables --------------- */

tnode_t treeRoot = { NULL, NULL, NULL, NULL };

/* --------------- Binary Search Tree implementation --------------- */

/* 
insertChild: Add a new child node to the parent node in the binary tree.
@param _data: Pointer to the node data
@param _parent: Pointer to the parent node
@param childSide: The side in which the child will be added (-1 for left, 1 for right)
*/
void insertNode(void* _data, tnode_t* _parent, int childSide)
{
    if((childSide != -1) && (childSide != 1)) return; /* left or right only */

    tnode_t* newNode = calloc(1, sizeof(tnode_t));
    if(newNode == NULL)
    {
        printf("Failed to allocate new tree node\n");
        exit(1);
    }

    /* A pointer to the pointer which points to the child node of the correct side */
    tnode_t** childPtr = (childSide == -1) ? &_parent->leftChild : &_parent->rightChild;
    if(*childPtr != NULL)
    {
        /* Another child already exists in that position */
        printf("There is already a child node in that position\n");
        return;
    }

    /* Fill in node attributes */
    newNode->data = _data;
    newNode->parent = _parent;
    newNode->leftChild = NULL;
    newNode->rightChild = NULL;

    /* Assign the parent's pointer to the child (of the correct side) */
    *childPtr = newNode; 
}

/* --------------- Helper functions ---- ----------- */

/* 
compareCompetency: Compares two competencies and determine which has a higher order. 
@param info1: Pointer to the competency info (1st)
@param info2: Pointer to the competency info (2nd)
@return: Returns -1/0/1 if the 1st info has a lower/equal/higher order respectively
*/
int compareCompetency(info_t* info1, info_t* info2)
{
    /* Compares by year */
    if(info1->year < info2->year) return -1;
    if(info1->year > info2->year) return 1;

    /* Compares by semester */
    if(info1->semester < info2->semester) return -1;
    if(info1->semester > info2->semester) return 1;

    /* Compares by alphabetical order */
    return strcmp(info1->code, info2->code);
}


/* 
createNode: Creates the info struct containing compentency's information.
@param competency_code: Pointer to the competency code
@param competency_title: Pointer to the competency title
@param credit: The amount of credits the competency offers
@param year: The year when the competency was offered
@param semester: The semester when the competency was offered within that year
@return: Returns a pointer to the allocated competency info struct.
*/
info_t* createNode(char* competency_code, char* competency_title, int credit, int year, int semester)
{
    info_t* newInfo = calloc(1, sizeof(info_t));
    if(newInfo == NULL)
    {
        printf("Failed to allocate new competency info\n");
        exit(1);
    }

    /* Fill in info attributes */
    strncpy(newInfo->code, competency_code, strlen(competency_code));
    strncpy(newInfo->title, competency_title, strlen(competency_title));
    newInfo->code[strlen(competency_code)] = '\0';
    newInfo->title[strlen(competency_title)] = '\0';
    newInfo->credit = credit;
    newInfo->year = year;
    newInfo->semester = semester;

    return newInfo;
}


/* 
insert: Insert a new node into the binary tree.
@param competency: Pointer to the competency info
*/
void insert(info_t* competency)
{
    if(treeRoot.data == NULL)
    {
        treeRoot.data = competency;
    }

    tnode_t* currentNode = &treeRoot;
    while(compareCompetency(currentNode->data, competency)) /* Exits if competency is found */
    {
        /* Tells the direction the new node should go */
        int direction = compareCompetency(competency, currentNode->data); 
        if(direction < 0) /* Go left */
        {
            if(currentNode->leftChild == NULL) 
            {
                insertNode(competency, currentNode, direction);
            }
            currentNode = currentNode->leftChild;
        }
        else /* Go right */
        {
            if(currentNode->rightChild == NULL) 
            {
                insertNode(competency, currentNode, direction);
            }
            currentNode = currentNode->rightChild;
        }
    }
}

/* 
printCompetencies: Prints the competencies in the binary tree/subtree using In-order traversal.
@param node: Pointer to the root of the tree/subtree
*/
void printCompetencies(tnode_t* root)
{
    /* Non-existent node */
	if(root == NULL) return;

	printCompetencies(root->leftChild);
    
    info_t* competency = root->data;
    printf("%d %d %s %s\n", 
        competency->year,
        competency->semester,
        competency->code,
        competency->title
    );

	printCompetencies(root->rightChild);
}

/* 
freeAll: Free all of the nodes and its contents from the tree/subtree.
@param _root: Pointer to the root node of the tree/subtree
*/
void freeAll(tnode_t* node)
{
	/* Non-existent node */
	if(node == NULL) return;

	freeAll(node->leftChild);
	freeAll(node->rightChild);

    /* Frees the pointer to the data in the node */
    free(node->data);
	node->data = NULL;

	/* Frees the pointer to the node */
	free(node);
	node = NULL;
}

int main()
{
    char competency_code[8];
    char competency_title[64];
    int credit;
    int year;
    int semester; /* Spring = 0, Fall = 1 */ 
    int num_operations; /* The number of operations. */
	scanf("%d", &num_operations);
    for (int i = 0; i < num_operations; i++)
    {
        scanf("%7s %63s %d %d %d", competency_code, competency_title, &credit, &year, &semester);
        info_t* newNode = createNode(competency_code, competency_title, credit, year, semester); 
        insert(newNode); 
    }
    printCompetencies(&treeRoot);
    freeAll(&treeRoot);
}