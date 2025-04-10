/* orgChart
 *
 * Build a corporate hierarchy structure and sorted binary tree
 *
 *  Created by Chavakorn Arunkunarax and Phasit Thanitkul, 2025-01-12
 *  Modified by Luka Bond, 2025-04-08
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* --------------- Data structures --------------- */

/* Employee info */
typedef struct _employee
{
    char* name; 
    char* position;
    char* id;
} employee_t;

typedef struct _tnode tnode_t;

/* Linked List */
typedef struct _linkedlist
{
    tnode_t* head; /* List head */
	tnode_t* tail; /* List tail */
} linkedlist_t;

/* Tree node */
typedef struct _tnode
{
    employee_t data;
    struct _tnode* parent; /* The parent of the node */
    struct _tnode* nextNode; /* Next sibling node */
    linkedlist_t children; /* List of children */
} tnode_t;

/* Custom binary tree node */
typedef struct _bstnode
{
    char* index;
    tnode_t* employeeInfo;
    struct _bstnode* parent; /* The parent of the node */
    struct _bstnode* leftChild; /* Pointer to left child */
    struct _bstnode* rightChild; /* Pointer to right child */
} bstnode_t;

/* --------------- Global variables --------------- */

tnode_t organisationChart;
bstnode_t employeeIndex;

/* --------------- Custom linked list-based Tree implementation --------------- */

/* 
insertTreeNode: Add a new child node to the specified parent node in the tree
@param _parent: Pointer to the parent node
@param _name: The string to be sotred in the child node
*/
void orgTreeInsert(tnode_t* parent, employee_t info)
{
    if(parent == NULL) return;

    tnode_t* newNode = malloc(sizeof(tnode_t));
    if(newNode == NULL)
    {
        perror("Failed to allocate new tree node\n");
        exit(1);
    }

    newNode->data = info;
    newNode->parent = parent;
    newNode->nextNode = NULL;

    linkedlist_t newList = { NULL, NULL };
    newNode->children = newList;
    
    linkedlist_t* nodeList = &parent->children;
    if(nodeList->tail != NULL)
	{
		/* Next node of current tail is the new node */
		(nodeList->tail)->nextNode = newNode;
	}

	/* List is empty */
	if(nodeList->head == NULL)
	{
		nodeList->head = newNode;
	}
	nodeList->tail = newNode; /* Reassigning the tail*/
}

/* --------------- Binary Search Tree implementation --------------- */

/* 
insertChild: Add a new child node to the parent node in the binary tree.
@param _data: Pointer to the node data
@param _parent: Pointer to the parent node
*/
void indexTreeInsert(bstnode_t* root, char* index, tnode_t* orgNode)
{
    if(root == NULL) return;

    bstnode_t* newNode = malloc(sizeof(bstnode_t));
    if(newNode == NULL)
    {
        perror("Failed to allocate new BST node\n");
        exit(1);
    }

    bstnode_t* parent = NULL;
    bstnode_t* currNode = root;
    int dir = 0;
    while(currNode != NULL)
    {
        parent = currNode;
        dir = strcmp(index, currNode->index);
        if(dir == 0)
        {
            return;
        }
        else 
        {
            currNode = dir < 0 ? currNode->leftChild : currNode->rightChild;
        }
    }
    
    newNode->index = index;
    newNode->employeeInfo = orgNode;
    newNode->parent = parent;
    newNode->leftChild = NULL;
    newNode->rightChild = NULL;

    if(dir < 0)
    {
        parent->leftChild = newNode;
    }
    else
    {
        parent->rightChild = newNode;
    }
}

tnode_t* indexTreeSearch(bstnode_t* root, char* index)
{
    bstnode_t* currNode = root;
    while(currNode != NULL)
    {
        int dir = strcmp(index, currNode->index);
        if(dir == 0)
        {
            return currNode->employeeInfo;
        }
        else 
        {
            currNode = dir < 0 ? currNode->leftChild : currNode->rightChild;
        }
    }
    return NULL; /* Not found */
}

/* --------------- Helper functions ---- ----------- */

char* allocString(const char* str)
{
    char* dupedStr = strdup(str);
    if (dupedStr == NULL)
    {
        perror("Failed to allocate new string\n");
        exit(1);
    }
    return dupedStr;
}

char* concatString(const char* str1, const char* str2)
{
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    /* +1 for null terminator */
    char* newStr = malloc(len1 + len2 + 1);
    if (newStr == NULL) 
    {
        perror("Failed to allocate new string\n");
        exit(1);
    }

    snprintf(newStr, len1 + len2 + 1, "%s%s", str1, str2);
    return newStr; 
}

/* Adds employee to the corporate hierarchy structure and sorted binary tree
 */
void addEmployee(char* name, char* employeeId, char* jobTitle, char* supervisorName, char* supervisorId)
{
    char* supIndex = concatString(supervisorName, supervisorId);
    char* empIndex = concatString(name, employeeId);
    
    employee_t newEmployee = { 
        allocString(name), 
        allocString(jobTitle), 
        allocString(employeeId)
    };

    if(!strcmp(supIndex, "----"))
    {
        free(supIndex);
        supIndex = NULL;

        organisationChart.data = newEmployee;
        employeeIndex.index = empIndex;
        employeeIndex.employeeInfo = &organisationChart;
        return;
    }

    tnode_t* supNode = indexTreeSearch(&employeeIndex, supIndex);
    if(supNode == NULL)
    {
        printf("Cannot add %s %s", name, employeeId);
        return;
    }
    orgTreeInsert(supNode, newEmployee);
    indexTreeInsert(&employeeIndex, empIndex, (supNode->children).tail);
} 

void printPreOrder(tnode_t* root, int depth)
{
    if(root == NULL) return;
    
    employee_t empInfo = root->data;
    for (int i = 0; i < depth; ++i)
    {
        printf("...");
    }
    printf("%s %s %s\n", empInfo.name, empInfo.id, empInfo.position);
    
    tnode_t* currNode = (root->children).head;
    while(currNode != NULL)
    {
        printPreOrder(currNode, depth + 1);
        currNode = currNode->nextNode;
    }
}

/* Prints the corporate hierarchy structure, pre-order
 */
void printCorporateHierarchy()
{
    printPreOrder(&organisationChart, 0);
}

void printInOrder(bstnode_t* root)
{
    if(root == NULL) return;

    printInOrder(root->leftChild);

    employee_t empInfo = (root->employeeInfo)->data;
    printf("%s %s %s\n", empInfo.name, empInfo.id, empInfo.position);
    
    printInOrder(root->rightChild);
}

/* Prints the sorted list of employees
 */
void printSortedList()
{
    printInOrder(&employeeIndex);
}

/* Searches for an employee in the corporate hierarchy structure
 */
void searchEmployee(char* name, char* employeeId)
{
    char* empIndex = concatString(name, employeeId);
    tnode_t* empNode = indexTreeSearch(&employeeIndex, empIndex);
    if(empNode == NULL)
    {
        printf("not_found");
    }

    tnode_t* supNode = empNode->parent;
    printf("%s %s %s\n", 
        (empNode->data).position,
        (supNode->data).name,
        (supNode->data).id
    );
}

void freeIndex(bstnode_t* root)
{
    if(root == NULL) return;
  
    freeIndex(root->leftChild);
    freeIndex(root->rightChild);

    employee_t empInfo = (root->employeeInfo)->data;

    free(empInfo.name);
    free(empInfo.position);
    free(empInfo.id);

    free(root->employeeInfo);
    free(root->index);
    free(root);
}


/* Free the contents of the datastructures used */
void freeAll()
{
	freeIndex(employeeIndex.leftChild);
    employeeIndex.leftChild = NULL;
	
    freeIndex(employeeIndex.rightChild);
    employeeIndex.rightChild = NULL;

    free(employeeIndex.index);
    employeeIndex.index = NULL;

    organisationChart.children.head = NULL;
    organisationChart.children.tail = NULL;

    employee_t* empInfo = &organisationChart.data;
    free(empInfo->name);
    free(empInfo->position);
    free(empInfo->id);

    empInfo->name = NULL;
    empInfo->position = NULL;
    empInfo->id = NULL;
}


int main()
{
    char input[128];
    char name[32];
    char employeeId[8];
    char jobTitle[32];
    char supervisorName[32];
    char supervisorId[8];

    int numEmployees;
    int numQuestions;

    scanf("%d", &numEmployees);
    scanf("%d", &numQuestions);

    for (int i = 0; i < numEmployees; i++)
    {
        scanf("\n%s %s %s %s %s", name, employeeId, jobTitle, supervisorName, supervisorId);
        addEmployee(name, employeeId, jobTitle, supervisorName, supervisorId);
    }

    printCorporateHierarchy();

    for (int i = 0; i < numQuestions; i++)
    {
        scanf("\n%s %s", name, employeeId);
        searchEmployee(name, employeeId);
    }

    printSortedList();

    freeAll();
    fflush(stdout);
    return 0;
}

