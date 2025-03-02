/* listGraph.c
 * 
 * Directed graph implementation using an adjacency list
 * You must complete the missing parts of the implementation
 * 
 * Template for Fundamental Data Structures Lab 5
 * Created by Phasit Thanitkul, 2025-02-09
 * Modified by Luka Bond, 2025-03-02
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VERTICES 100 

/* --------------- Data structures --------------- */

/* Custom singly linked list node */
typedef struct _node
{
	int key;
	struct _node* nextNode;
} node_t;

/* Linked List */
typedef struct _linkedlist
{
	node_t* head;
	node_t* tail;
} linkedlist_t;

/* --------------- Global variables --------------- */

int verticesCount = 0;
int nodeKeys[MAX_VERTICES];
linkedlist_t adjacencyList[MAX_VERTICES];

/* --------------- Custom Linked list implementation --------------- */

/* 
listInsert: Insert a hash key into the list.
@param key: Vertex key to add in the list
@param list: Pointer to the list
*/
void listInsert(int key, linkedlist_t* list)
{
	node_t* newNode = calloc(1, sizeof(node_t));
	if(newNode == NULL)
	{
		printf("Failed to allocate new node\n");
		exit(1);
	}

	/* Filling node attributes */
	newNode->key = key; /* Assign key */
	newNode->nextNode = NULL; /* No next node (new tail) */
	
	if(list->tail != NULL)
	{
		/* Next node of current tail is the new node */
		(list->tail)->nextNode = newNode;
	}

	/* List is empty */
	if(list->head == NULL)
	{
		list->head = newNode;
	}
	list->tail = newNode; /* Reassigning the tail*/
}

/* ---------- Hash table - Linear probing implementation ---------- */

/* 
hash: A custom hash function to map keys to indexes.
@param key: Integer value to be hashed
@return: Hash function output
*/
int hash(int key)
{
    return abs(key) % MAX_VERTICES;
}

/* 
insert: Insert a value into the hash table (Linear probing).
@param arr: Pointer to hash table
@param value: Value to be inserted into the table
    (Exception: -1 represents an empty cell)
*/
void hashInsert(int* arr, int value)
{
    int hashKey = hash(value);
    while(arr[hashKey] != -1) /* Loop until cell is empty */
    {
        hashKey = (hashKey + 1) % (MAX_VERTICES); /* Move to next cell */
    }
    arr[hashKey] = value;
}

/* 
search: Search for a value in the hash table (Linear probing).
@param arr: Pointer to hash table
@param value: Value to be searched in the table
    (Exception: -1 represents an empty cell)
@return: The index of the key in the array, -1 if not found
*/
int search(int* arr, int value)
{
    int hashKey = hash(value);
    while(arr[hashKey] != -1) /* Loop until cell is empty */
    {
        if(arr[hashKey] == value) 
        { 
            return hashKey; /* Key found */
        }
        hashKey = (hashKey + 1) % (MAX_VERTICES); /* Move to next cell */
    }
    return -1; /* Not found */
}

/* --------------- Graph implementation ---------------- */

/* 
initializeGraph: Initialises graph attributes with base values.
*/
void initializeGraph() 
{
    memset(nodeKeys, -1, sizeof(nodeKeys)); /* Empty = -1 */
    memset(adjacencyList, 0, sizeof(adjacencyList)); /* Empty = -1 */
}

/* 
addVertex: Adds a vertex to the graph.
@param key: Vertex key to add in the hash table
*/
void addVertex(int key) {
    /* Max amount of vertices reached */
    if(verticesCount == MAX_VERTICES) return;

    hashInsert(nodeKeys, key);
    ++verticesCount;
}

/* 
addEdge: Adds an edge between two vertices.
@param start: Starting vertex
@param end: Ending vertex
*/
void addEdge(int start, int end) {
    /* Vertex indexes in the hash table */
    int startIdx = search(nodeKeys, start);
    int endIdx = search(nodeKeys, end);

    /* Vertex does not exist */
    if(startIdx < 0 || endIdx < 0) return;
    
    listInsert(end, &adjacencyList[startIdx]);
}

/* 
printAdjacent: Finds and prints all vertices adjacent to a given vertex.
@param vertex: Vertex to find in the graph.
*/
void printAdjacent(int vertex) {
    int vertexIdx = search(nodeKeys, vertex);
    if(vertexIdx < 0)
    {
        printf("Not_found");
        return;
    }

    int neighbours = 0;
    node_t* nodePtr = adjacencyList[vertexIdx].head;
    while(nodePtr != NULL)
    {
        printf("%d ", nodePtr->key);
        nodePtr = nodePtr->nextNode;
        ++neighbours;
    }

    if(neighbours == 0)
    {
        printf("None");
    }
    printf("\n");
}

/* 
freeGraph: Free the graph's memory.
*/
void freeGraph() {
    for (int i = 0; i < MAX_VERTICES; ++i)  
    {
        linkedlist_t neighbours = adjacencyList[i];
        node_t* nodePtr = neighbours.head;
        while(nodePtr != NULL)
        {
            /* Assign next node */
            node_t* nextPtr = nodePtr->nextNode;
            
            /* Free the the node and its contents */
            free(nodePtr);

            /* Reassign the node pointer */
            nodePtr = nextPtr;
        }
        neighbours.head = NULL;
        neighbours.tail = NULL;
    }
}

/* Main function to handle user input. */
int main() {
    char inputline[32];
    int numOperations, key, start, end;
    
    initializeGraph();
    
    scanf("%d", &numOperations);
    for (int i = 0; i < numOperations; i++) {
        scanf("%s", inputline);
        
        if (strcmp(inputline, "AddVertex") == 0) {
            scanf("%d", &key);
            addVertex(key);
        }
        else if (strcmp(inputline, "AddEdge") == 0) {
            scanf("%d %d", &start, &end);
            addEdge(start, end);
        }
        else if (strcmp(inputline, "Print") == 0) {
            scanf("%d", &key);
            printAdjacent(key);
        }
    }
    
    freeGraph();
    return 0;
}
