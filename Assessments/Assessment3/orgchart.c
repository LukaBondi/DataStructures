/* orgChart
 *
 *  Build a corporate hierarchy structure and sorted binary tree
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

/* Forward declare tree node */
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
    employee_t data; /* Employee struct */
    struct _tnode* parent; /* The parent of the node */
    struct _tnode* nextNode; /* Next sibling node */
    linkedlist_t children; /* List of children */
} tnode_t;

/* Binary tree node */
typedef struct _bstnode
{
    char* index; /* Pointer to index */
    tnode_t* employeeInfo; /* Pointer to tree node */
    struct _bstnode* parent; /* The parent of the index */
    struct _bstnode* leftChild; /* Pointer to left index */
    struct _bstnode* rightChild; /* Pointer to right index */
} bstnode_t;

/* --------------- Global variables --------------- */

tnode_t organisationChart; /* Organisation tree */
bstnode_t employeeIndex; /* Employee index bst */

/* --------------- Custom linked list-based Tree implementation --------------- */

/* 
orgTreeInsert: Add a new child node to the specified parent node in the tree.
@param parent: Pointer to the parent node
@param info: Struct containing employee info
*/
void orgTreeInsert(tnode_t* parent, employee_t info)
{
    /* Non-existent parent */
    if(parent == NULL) return;

    /* Allocate new tree node */
    tnode_t* newNode = malloc(sizeof(tnode_t));
    if(newNode == NULL)
    {
        perror("Failed to allocate new tree node\n");
        exit(1);
    }

    /* Fill in node attributes */
    newNode->data = info;
    newNode->parent = parent;
    newNode->nextNode = NULL;

    /* Start new child list */
    linkedlist_t newList = { NULL, NULL };
    newNode->children = newList;
    
    /* Fetch the parent's child list */
    linkedlist_t* nodeList = &(parent->children);
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
indexTreeInsert: Add a new index into the binary tree.
@param root: Pointer to the bst root
@param index: Pointer to the index string
@param orgNode: Pointer to the tree node
*/
void indexTreeInsert(bstnode_t* root, char* index, tnode_t* orgNode)
{
    /* Non-existent root */
    if(root == NULL) return;

    /* Allocate new bst node */
    bstnode_t* newNode = malloc(sizeof(bstnode_t));
    if(newNode == NULL)
    {
        perror("Failed to allocate new BST node\n");
        exit(1);
    }

    /* Traverse the bst, return if node already exists */
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
    
    /* Fill in node attributes */
    newNode->index = index;
    newNode->employeeInfo = orgNode;
    newNode->parent = parent;
    newNode->leftChild = NULL;
    newNode->rightChild = NULL;

    /* Insert bst node according to the direction */
    if(dir < 0)
    {
        parent->leftChild = newNode;
    }
    else
    {
        parent->rightChild = newNode;
    }
}

/* 
indexTreeSearch: Search for an index in the binary tree.
@param root: Pointer to the bst root
@param index: Pointer to the index string to compare
@return: Pointer to the corresponding tree node of the given index
*/
tnode_t* indexTreeSearch(bstnode_t* root, char* index)
{
    /* Traverse the bst */
    bstnode_t* currNode = root;
    while(currNode != NULL)
    {
        int dir = strcmp(index, currNode->index);
        if(dir == 0)
        {
            /* Return employe info if the index exists */
            return currNode->employeeInfo;
        }
        else 
        {
            /* Traverse in accordance to the direction of the comparison */
            currNode = dir < 0 ? currNode->leftChild : currNode->rightChild;
        }
    }
    return NULL; /* Not found */
}

/* --------------- Helper functions ---------------- */

/* 
allocString: Duplicates the given string.
@param str: Pointer to the string to duplicate
@return: Pointer to the duplicated string (Must free)
*/
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

/* 
concatString: Concatenates two given strings.
@param str1: Pointer to the first string
@param str2: Pointer to the second string
@return: Pointer to the concatenated string (Must free) 
*/
char* concatString(const char* str1, const char* str2)
{
    size_t len1 = strlen(str1); /* Length of string 1 */
    size_t len2 = strlen(str2); /* Length of string 2 */

    /* +1 for null terminator */
    char* newStr = malloc(len1 + len2 + 1);
    if (newStr == NULL) 
    {
        perror("Failed to allocate new string\n");
        exit(1);
    }

    /* Copy both strings content into the newly allocated string */
    snprintf(newStr, len1 + len2 + 1, "%s%s", str1, str2);
    return newStr; 
}

/* 
addEmployee: Adds employee to the corporate hierarchy structure and sorted binary tree.
@param name: Pointer to the employee's name string
@param employeeId: Pointer to the employee's id string
@param jobTitle: Pointer to the employee's job title string
@param supervisorName: Pointer to the supervisor's name string
@param supervisorId: Pointer to the supervisor's id string
*/
void addEmployee(char* name, char* employeeId, char* jobTitle, char* supervisorName, char* supervisorId)
{
    /* Generate index for employee and supervisor */
    char* supIndex = concatString(supervisorName, supervisorId);
    char* empIndex = concatString(name, employeeId);
    
    /* Create struct containing employee info */
    employee_t newEmployee = { 
        allocString(name), 
        allocString(jobTitle), 
        allocString(employeeId)
    };

    /* Insert info and index for tree and bst root (Top of the hierarchy) */
    if(!strcmp(supIndex, "----"))
    {
        free(supIndex);
        supIndex = NULL;

        organisationChart.data = newEmployee;
        employeeIndex.index = empIndex;
        employeeIndex.employeeInfo = &organisationChart;
        return;
    }

    /* Find supervisor node in the organisation chart */
    tnode_t* supNode = indexTreeSearch(&employeeIndex, supIndex);
    if(supNode == NULL)
    {
        /* Non-existent supervisor */
        printf("Cannot add %s %s", name, employeeId);
        return;
    }

    /* Insert employee info and index for organisation chart and employee index */
    orgTreeInsert(supNode, newEmployee); 
    indexTreeInsert(&employeeIndex, empIndex, (supNode->children).tail);
} 

/* 
printPreOrder: Prints the employee info pre-ordered (To sustain hierarchy).
@param root: Pointer to the organisation chart root
@param depth: Current traversed depth within the tree
*/
void printPreOrder(tnode_t* root, int depth)
{
    /* Non-existent root */
    if(root == NULL) return;
    
    /* Extract and print employee data */
    employee_t empInfo = root->data;
    for (int i = 0; i < depth; ++i)
    {
        printf("..."); /* Using depth to indicate order in hierarchy */
    }
    printf("%s %s %s\n", empInfo.name, empInfo.id, empInfo.position);
    
    /* Traverse through the children in the list and perform recursion */
    tnode_t* currNode = (root->children).head;
    while(currNode != NULL)
    {
        printPreOrder(currNode, depth + 1);
        currNode = currNode->nextNode;
    }
}

/* 
printCorporateHierarchy: Prints the corporate hierarchy structure, pre-order.
*/
void printCorporateHierarchy()
{
    printPreOrder(&organisationChart, 0);
}

/* 
printInOrder: Prints the employee info in-ordered (For sorting).
@param root: Pointer to the employee index root
*/
void printInOrder(bstnode_t* root)
{
    if(root == NULL) return;

    printInOrder(root->leftChild);

    employee_t empInfo = (root->employeeInfo)->data;
    printf("%s %s %s\n", empInfo.name, empInfo.id, empInfo.position);
    
    printInOrder(root->rightChild);
}

/* 
printSortedList: Prints the sorted list of employees.
*/
void printSortedList()
{
    printInOrder(&employeeIndex);
}

/* 
searchEmployee: Searches for an employee in the corporate hierarchy structure.
*/
void searchEmployee(char* name, char* employeeId)
{
    
    char* empIndex = concatString(name, employeeId); /* Create employee index */
    tnode_t* empNode = indexTreeSearch(&employeeIndex, empIndex); /* Extracts employee node */
    if(empNode == NULL)
    {
        /* Non-existent employee */
        printf("not_found");
    }

    /* Get the employee's supervisor and prints their info */
    tnode_t* supNode = empNode->parent;
    printf("%s %s %s\n", 
        (empNode->data).position,
        (supNode->data).name,
        (supNode->data).id
    );
}

/* 
freeIndex: Frees the content of the employee index, organisation chart and employee info.
**Note: Freed memory doesn't need to be set to NULL because the memory location of the dangling pointers 
        will also be freed. Therefore, no dangling pointers except for the one pointing to original root.
*/
void freeIndex(bstnode_t* root)
{
    /* Non-existent node */
    if(root == NULL) return;
  
    /* Perform recursion on the left and right sub-trees */
    freeIndex(root->leftChild);
    freeIndex(root->rightChild);

    /* Extract employee info from the tree node */
    employee_t empInfo = (root->employeeInfo)->data;

    /* Free every string from the info struct */
    free(empInfo.name);
    free(empInfo.position);
    free(empInfo.id);

    free(root->employeeInfo); /* Free tree node */
    free(root->index); /* Free index string */
    free(root); /* Free current bst node */
}


/* 
freeAll: Free the contents of the datastructures used.
**Note: Root of organisation tree and index bst were not dynamically allocated.
*/ 
void freeAll()
{
    /* Free left child of root and set it to null*/
	freeIndex(employeeIndex.leftChild);
    employeeIndex.leftChild = NULL;
	
    /* Free right child of root and set it to null*/
    freeIndex(employeeIndex.rightChild);
    employeeIndex.rightChild = NULL;

    /* Free the index string and set it to null */
    free(employeeIndex.index);
    employeeIndex.index = NULL;

    /* Set children list head and tail to NULL to prevent dangling pointers */
    organisationChart.children.head = NULL;
    organisationChart.children.tail = NULL;

    /* Extract the root employee info */
    employee_t* empInfo = &organisationChart.data;

    /* Free every string from the info struct */
    free(empInfo->name);
    free(empInfo->position);
    free(empInfo->id);

    /* Set pointers to NULL to prevent dangling pointers */
    empInfo->name = NULL;
    empInfo->position = NULL;
    empInfo->id = NULL;
}


int main()
{
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
    fflush(stdout); /* Flush output (Soemtimes output is stuck in the buffer) */
    return 0;
}

