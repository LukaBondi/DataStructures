/* dictionary.c
 * 
 *  Build a dictionary using hash table and query the dictionary
 *
 *  Template for Fundamental Data Structures Lab 4
 *  Created by Phasit Thanitkul, 2025-02-01
 *  Modified by Luka Bond, 2025-03-01
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --------------- Data structures --------------- */

/* Singly linked list node */
typedef struct _node
{
	char* word;
	char* definition;
	struct _node* nextNode;
} node_t;

/* Linked List */
typedef struct _linkedlist
{
	node_t* head;
	node_t* tail;
} linkedlist_t;

/* --------------- Global variables --------------- */

linkedlist_t hashDict[26];

/* --------------- Custom Linked list implementation --------------- */

/* 
insert: Insert a word-definition keypair into the list.
@param word: Pointer to word string to be stored
@param definition: Pointer to definition string to be stored
@param list: Pointer to the list
*/
void insert(char* word, char* definition, linkedlist_t* list)
{
	node_t* newNode = calloc(1, sizeof(node_t));
	if(newNode == NULL)
	{
		printf("Failed to allocate new node\n");
		exit(1);
	}

	/* Filling node attributes */
	newNode->word = strdup(word); /* Assign word */
	newNode->definition = strdup(definition); /* Assign definition */
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

/* --------------- Helper functions ---------------- */

/* 
hash: A custom string hash function (Based on the first letter).
@param str: Pointer to string to be hashed
@return: Hash function output (0 - 25)
*/
int hash(char* str)
{
    /* Consider only the first letter for hashing */
    return (*str - 'a') % 26; 
}


/* 
addWord: Adds a word and its definition to the dictionary.
@param word: Pointer to the word string
@param definition: Pointer to the definition string
@return: Hash function output (0 - 25)
*/
void addWord(char* word, char* definition)
{
    int strHash = hash(word);
    insert(word, definition, &hashDict[strHash]);
}

/* 
printDictionary: Prints all words and their definitions in the dictionary.
*/
void printDictionary()
{
    int dictLength = sizeof(hashDict) / sizeof(hashDict[0]);
    for (int i = 0; i < dictLength; ++i)
    {
        linkedlist_t wordList = hashDict[i];
        node_t* nodePtr = wordList.head;
        while(nodePtr != NULL)
        {
            printf("%s: %s\n", nodePtr->word, nodePtr->definition);
            nodePtr = nodePtr->nextNode;
        }
    }
}

/* 
queryDefinition: Query and print the definition(s) of a word.
@param word: Pointer to the word string
*/
void queryDefinition(char* word)
{
    int strHash = hash(word);
    linkedlist_t wordList = hashDict[strHash];
    
    int wordFound =  0;
    node_t* nodePtr = wordList.head;
    while(nodePtr != NULL)
    {
        if(!strcmp(word, nodePtr->word))
        {
            printf("%s: %s\n", nodePtr->word, nodePtr->definition);
            ++wordFound; /* Atleast one definition is found*/
        }
        nodePtr = nodePtr->nextNode;
    }

    if(wordFound == 0)
    {
        printf("Word not found\n");
    }
}

/* 
freeAll: Free all list nodes and its contents.
*/
void freeAll()
{
    int dictLength = sizeof(hashDict) / sizeof(hashDict[0]);
    for (int i = 0; i < dictLength; ++i)
    {
        linkedlist_t wordList = hashDict[i];
        node_t* nodePtr = wordList.head;
        while(nodePtr != NULL)
        {
            /* Assign next node */
            node_t* nextPtr = nodePtr->nextNode;
            
            /* Free the the node and its contents */
            free(nodePtr->word);
            free(nodePtr->definition);
            free(nodePtr);

            /* Reassign the node pointer */
            nodePtr = nextPtr;
        }
        wordList.head = NULL;
        wordList.tail = NULL;
    }
}

int main()
{
    char input[256];
    char word[64];
    char definition[256];
    int numOperations;
    scanf("%d", &numOperations);
    for (int i = 0; i < numOperations; i++)
    {
        scanf("%s", input);
        if (strcmp(input, "Add") == 0)
        {
            scanf("%s", word);
            scanf(" %[^\n]", definition);
            addWord(word, definition);
        }
        else if (strcmp(input, "Print") == 0)
        {
            printDictionary();
        }
        else if (strcmp(input, "Query") == 0)
        {
            scanf("%s", word);
            queryDefinition(word);
        }
    }
    freeAll();
}