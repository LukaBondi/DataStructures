/* cmkl_book
 * 
 * Build a social network with basic functionalities
 *
 *  Template for Fundamental Data Structures Lab 2
 *  Created by Pasin Manurangsi, 2025-01-13
 * 	Modified by Luka Chanakan Bond, 2025-04-27
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* --------------- Data structures --------------- */

/* Singly linked list node */
typedef struct _lnode
{
	char* email;
	struct _lnode* nextNode;
} lnode_t;

/* Linked List */
typedef struct _linkedlist
{
	lnode_t* head;
	lnode_t* tail;
} linkedlist_t;

/* Graph node status */
typedef enum {
	WHITE,
	GREY,
	BLACK
} nodeColor;

/* Graph node */
typedef struct _gnode
{
	char* firstName;
	char* lastName;
	char* emailAddress; /* Node key */
	linkedlist_t friends; /* Adjacency list */
	nodeColor status; /* Traversal status */
} gnode_t;

/* --------------- Global variables --------------- */

char* delMarker = "DEL"; /* Hash table DEL Marker */
int tableSize = 10;
int elementCount = 0; 
gnode_t* nodeKeys = NULL;

/* --------------- Custom Linked list implementation --------------- */

/* 
listInsert: Insert an email into the list.
@param email: Pointer to the email to be added
@param list: Pointer to the list
*/
void listInsert(char* email, linkedlist_t* list)
{
	lnode_t* newNode = malloc(sizeof(lnode_t));
	if(newNode == NULL)
	{
		perror("List node allocation error: ");
		exit(1);
	}

	/* Filling node attributes */
	newNode->email = email; /* Assign key */
	newNode->nextNode = NULL; /* No next node (new tail) */

	/* Next node of current tail is the new node */
	if(list->tail != NULL)
	{
		(list->tail)->nextNode = newNode;
	}
	
	/* List is empty */
	if(list->head == NULL)
	{
		list->head = newNode;
	}
	list->tail = newNode; /* Reassigning the tail*/
}

/* 
listSeach: Search for an email in the list.
@param email: Pointer to the email to be searched for
@param list: Pointer to the list
@return: Pointer to the list node, NULL if not found
*/
lnode_t* listSearch(char* email, linkedlist_t* list)
{
	/* Empty list */
	if(list->head == NULL) return NULL;

	lnode_t* currNode = list->head;
	while(currNode != NULL)
	{
		/* Node found */
		if(!strcmp(currNode->email, email)) return currNode; 
		currNode = currNode->nextNode;
	}
	return NULL; /* Node not found */
}

/* 
listDelete: Delete an email in the list.
@param email: Pointer to the email to be deleted 
@param list: Pointer to the list
*/
void listDelete(char* email, linkedlist_t* list)
{
	/* Empty list */
	if(list->head == NULL) return;

	lnode_t* prevNode = NULL; 
	lnode_t* currNode = list->head;
	while(currNode != NULL)
	{
		/* Node found */
		if(!strcmp(currNode->email, email)) break;
		prevNode = currNode;
		currNode = currNode->nextNode;
	}
	if(currNode == NULL) return; /* Node not found */

	/* Deletion at head -> Move head */
	if(currNode == list->head) 
	{
		list->head = currNode->nextNode; 
	}
	
	/* Deletion at tail -> Move tail */
	if(currNode == list->tail) 
	{
		list->tail = prevNode;
	}

	/* Redirect next node pointer (of the previous node) */
	if(prevNode != NULL)
	{
		prevNode->nextNode = currNode->nextNode;
	}

	free(currNode); /* Deletion */
}

/* ---------- Hash table - Linear probing implementation ---------- */

/* 
hash: A custom string hash function.
	Adapted from: https://cp-algorithms.com/string/string-hashing.html
@param key: Pointer to string to be hashed
@param size: Hash table size
@return: Hash function output
*/
int hash(char* key, int size)
{
	/* Invalid string */
	if(key == NULL) 
	{
		printf("Unable to hash string\n");
		return -1;
	}

	/* Hash function conditions */
	const int p = 53;
	const int m = size;
	int hash = 0;
	int p_pow = 1;

	/* Looping through each character */
	for (int i = 0; i < strlen(key); ++i)
	{
		char c = key[i];
		hash = (hash + (c - 'a' + 1) * p_pow) % m;
		p_pow = (p_pow * p) % m;
	}
    return hash;
}

/* 
hashInsert: Insert a key into the hash table (Linear probing).
@param table: Pointer to the hash table
@param size: Hash table size
@param person: Graph node containing the key
*/
void hashInsert(gnode_t* table, int size, gnode_t person)
{
	int hashKey = hash(person.emailAddress, size);
	while(table[hashKey].emailAddress != NULL) /* Loop until cell is empty */
	{
		hashKey = (hashKey + 1) % (size); /* Move to next cell */
	}
	table[hashKey] = person; /* Copy attributes over */
}

/* 
hashSearch: Search for a key in the hash table (Linear probing).
@param table: Pointer to the hash table
@param size: Hash table size
@param email: Pointer to the email to be searched for
@return: Pointer to the graph node, NULL if not found
*/
gnode_t* hashSearch(gnode_t* table, int size, char* email)
{
	int hashKey = hash(email, size);
	while(table[hashKey].emailAddress != NULL) /* Loop until cell is empty */
	{
		if(!strcmp(table[hashKey].emailAddress, email)) return (table + hashKey);
		hashKey = (hashKey + 1) % (size); /* Move to next cell */
	}
	return NULL;
}

/* 
hashDelete: Delete a key in the hash table (Linear probing).
@param table: Pointer to the hash table
@param size: Hash table size
@param email: Pointer to the email to be deleted
*/
void hashDelete(gnode_t* table, int size, char* email)
{
	gnode_t* node = hashSearch(table, size, email); /* Search for node */
	if(node == NULL) return; /* Node not found */

	/* Free allocated strings */
	free(node->firstName);
	free(node->lastName);
	free(node->emailAddress);

	/* Clear friends list */
	linkedlist_t* list = &(node->friends);
	while(list->head != NULL)
	{
		listDelete((list->head)->email, list);
	}

	/* The DEL marker is globally allocated, DO NOT FREE */
	memset(node, 0, sizeof(gnode_t)); /* Set all values to NULL */
	node->emailAddress = delMarker; /* Set key to DEL marker */
}

/* 
rebuildTable: Rebuilds the hash table and reinserts all elements.
@param table: Pointer to the hash table pointer (Table memory location will change)
@param size: Pointer to the table size
*/
void rebuildTable(gnode_t** table, int* size, int* count)
{
	/* New table size is doubled */
	int newSize = (*size) * 2;
	gnode_t* newTable = calloc(newSize, sizeof(gnode_t));
	if(newTable == NULL)
	{
		perror("Table rebuild error: ");
		exit(1);
	}
	
	/* Reinsert all valid entries and recount elements*/
	int elements = 0;
	for (int i = 0; i < (*size); i++) 
	{
		gnode_t node = (*table)[i];
		if (node.emailAddress == NULL || /* Empty string */
			!strcmp(node.emailAddress, delMarker)) /* Deleted marker */
		{
			continue; 
		}

		/* Reinsertion*/
		hashInsert(newTable, newSize, node);
		++elements; 
	}

	/* Free old table */
	free(*table);

	/* Update table pointer and size */
	*table = newTable;
	*size = newSize;
	*count = elements;
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
        perror("String allocation error: ");
        exit(1);
    }
    return dupedStr;
}

/* --------------- Graph implementation ---------------- */

/* 
newAccount: Creates a new account.
@param emailAddress: Pointer to the email address to duplicate
@param firstName: Pointer to the first name anstring to duplicate
@param lastName: Pointer to the last name to duplicate
*/
void newAccount(char* emailAddress, char* firstName, char* lastName)
{
	/* Check for duplicate account */
	if(hashSearch(nodeKeys, tableSize, emailAddress) != NULL)
	{
		printf("Error: Duplicate\n");
		return;
	}
	
	/* Element has exceeded table threshold */
	if(elementCount > tableSize * 0.75) 
	{
		rebuildTable(&nodeKeys, &tableSize, &elementCount);
	}

	/* Create new graph node */
	gnode_t newPerson = {
		allocString(firstName),
		allocString(lastName),
		allocString(emailAddress),
		{ NULL, NULL },
		WHITE
	};

	/* Insert graph node into the table */
	hashInsert(nodeKeys, tableSize, newPerson);
	++elementCount;
	printf("Success\n");
}

/* 
deleteAccount: Deletes an account.
@param emailAddress: Pointer to the email address
*/
void deleteAccount(char* emailAddress)
{
	/* Check for an existing account */
	gnode_t* node = hashSearch(nodeKeys, tableSize, emailAddress) ;
	if(node == NULL)
	{
		/* Non-existent account */
		printf("Error: Not Found\n");
		return;
	}

	/* Delete all edges connected to the current node */
	for(int i = 0; i < tableSize; ++i)
	{
		/* Go through adjacency list */
		linkedlist_t* list = &(nodeKeys[i].friends);
		listDelete(emailAddress, list);
	}

	/* Delete graph node from the table */
	printf("Success: %s %s\n", node->firstName, node->lastName);
	hashDelete(nodeKeys, tableSize, emailAddress);
}

/* 
addFriend: Makes two accounts friend of each other.
@param firstEmailAddress: Pointer to the first email address
@param secondEmailAddress: Pointer to the second email address
*/
void addFriend(char* firstEmailAddress, char* secondEmailAddress)
{
	/* Check if the emails are the same */
	if(!strcmp(firstEmailAddress, secondEmailAddress))
	{
		printf("Error: Cannot add friend to self\n");
		return;
	}

	/* Check for existing accounts */
	gnode_t* node1 = hashSearch(nodeKeys, tableSize, firstEmailAddress);
	gnode_t* node2 = hashSearch(nodeKeys, tableSize, secondEmailAddress);
	if(node1 == NULL || node2 == NULL)
	{
		/* Non-existent account */
		printf("Error: Account Not Found\n");
		return;
	}
	
	/* Check if both people are already not friends */
	linkedlist_t* list1 = &(node1->friends);
	linkedlist_t* list2 = &(node2->friends);

	/* Searches for friend in friend list */
	lnode_t* person2 = listSearch(node2->emailAddress, list1);
	lnode_t* person1 = listSearch(node1->emailAddress, list2);
	if(person1 != NULL || person2 != NULL)
	{
		/* Friend exists, already friended */
		printf("Error: Accounts are already friends\n");
		return;
	}

	/* Insert edge from person 1 to person 2 */
	listInsert(node2->emailAddress, list1); 

	/* Insert edge from person 2 to person 1 */
	listInsert(node1->emailAddress, list2);

	printf("Success\n");
}

/* 
unfriend: Unfriends two accounts.
@param firstEmailAddress: Pointer to the first email address
@param secondEmailAddress: Pointer to the second email address
*/
void unfriend(char* firstEmailAddress, char* secondEmailAddress)
{
	/* Check if the emails are the same */
	if(!strcmp(firstEmailAddress, secondEmailAddress))
	{
		printf("Error: Cannot unfriend self\n");
		return;
	}

	/* Check for existing accounts */
	gnode_t* node1 = hashSearch(nodeKeys, tableSize, firstEmailAddress);
	gnode_t* node2 = hashSearch(nodeKeys, tableSize, secondEmailAddress);
	if(node1 == NULL || node2 == NULL)
	{
		/* Non-existent account */
		printf("Error: Account Not Found\n");
		return;
	}

	/* Check if both people are already not friends */
	linkedlist_t* list1 = &(node1->friends);
	linkedlist_t* list2 = &(node2->friends);
	
	/* Searches for friend in friend list */
	lnode_t* person2 = listSearch(node2->emailAddress, list1);
	lnode_t* person1 = listSearch(node1->emailAddress, list2);
	if(person1 == NULL || person2 == NULL)
	{
		/* Non-existent friend */
		printf("Error: Cannot unfriend accounts that are not friends\n");
		return;
	}

	/* Delete edge from person 1 to person 2 */
	listDelete(node2->emailAddress, list1); 

	/* Delete edge from person 2 to person 1 */
	listDelete(node1->emailAddress, list2);

	printf("Success\n");
}

/* 
listFriends: Prints the account's friends.
@param emailAddress: Pointer to the email address
*/
void listFriends(char* emailAddress)
{
	/* Check for an existing account */
	gnode_t* node = hashSearch(nodeKeys, tableSize, emailAddress);
	if(node == NULL)
	{
		/* Non-existent account */
		printf("Error: Account Not Found\n");
		return;
	}

	/* Check if friend list is empty */
	lnode_t* friendList = (node->friends).head;
	if(friendList == NULL)
	{
		printf("No Friend\n");
		return;
	}

	/* Loop over friends in list */
	while(friendList != NULL)
	{
		gnode_t* friend = hashSearch(nodeKeys, tableSize, friendList->email);
		printf("%s %s", friend->firstName, friend->lastName); /* Print friend */
		if(friendList->nextNode != NULL)
		{
			/* Print only if it's not the last node (Beautiful printing) */
			printf(", ");
		}
		friendList = friendList->nextNode; /* Next friend */
	}
	printf("\n");
}

/* 
listSuggestions: Prints the account's friend suggestions.
@param emailAddress: Pointer to the email address
*/
void listSuggestions(char* emailAddress)
{
	/* Check for an existing account */
	gnode_t* node = hashSearch(nodeKeys, tableSize, emailAddress);
	if(node == NULL)
	{
		/* Non-existent account */
		printf("Error: Account Not Found\n");
		return;
	}

	/* Check if friend list is empty */
	lnode_t* friendList = (node->friends).head;
	if(friendList == NULL)
	{
		printf("No Friend Suggestion\n");
		return;
	}

	/* Colour all nodes white (All nodes unexplored) */
	for(int i = 0; i < tableSize; ++i)
	{
		nodeKeys[i].status = WHITE;
	}
	
	/* Colour self and all friends black (Exclude from suggestion list) */
	node->status = BLACK; /* Ignore the person in question */
	while(friendList != NULL) 
	{
		gnode_t* friend = hashSearch(nodeKeys, tableSize, friendList->email);
		friend->status = BLACK; /* Ignore friends of the person in question */
		friendList = friendList->nextNode; /* Next friend */
	}

	friendList = (node->friends).head; /* Move back to head */
	int suggestionCount = 0; /* Keep track */
	while(friendList != NULL)
	{
		gnode_t* friend = hashSearch(nodeKeys, tableSize, friendList->email);
		lnode_t* suggestionList = (friend->friends).head; /* Extract suggestion list from friend */
		while(suggestionList != NULL)
		{
			gnode_t* suggestion = hashSearch(nodeKeys, tableSize, suggestionList->email);
			if(suggestion->status != WHITE) 
			{
				/* Skip suggestion, suggestion is either self or a friend */
				suggestionList = suggestionList->nextNode;
				continue;
			}
			
			if(suggestionCount > 0)
			{
				/* Print only if it's not the first suggestion (Beautiful printing) */
				printf(", ");
			}

			/* Print suggestion */
			printf("%s %s", suggestion->firstName, suggestion->lastName);
			++suggestionCount;
			suggestion->status = BLACK; /* Colour black to not explore again */
			suggestionList = suggestionList->nextNode; /* Next suggestion */
		}
		friendList = friendList->nextNode; /* Next friend */
	}

	/* Friends don't have other friends (What a sad world) */
	if(suggestionCount == 0)
	{
		printf("No Friend Suggestion");
	}
	printf("\n");
}

/* Free the contents of the datastructures used */
void freeAll()
{
	/* Loop over all nodes */
	for(int i = 0; i < tableSize; ++i)
	{
		/* Check if node is empty or marked deleted */
		gnode_t* node = (nodeKeys + i);
		if(node->emailAddress == NULL || node->emailAddress == delMarker) continue;

		/* Free all strings in the node */
		free(node->firstName); 
		free(node->lastName);
		free(node->emailAddress);
	
		/* Extract friend list */
		linkedlist_t* list = &(node->friends);
		while(list->head != NULL)
		{
			/* Remove all friends from the list */
			listDelete((list->head)->email, list);
		}
		memset(node, 0, sizeof(gnode_t)); /* Set all node attributes to 0 and NULL */
	}
	free(nodeKeys); 	/* Free table pointer */
	nodeKeys = NULL; 	/* Set table pointer to NULL */
	tableSize = 0; 		/* Set table size to 0 */
	elementCount = 0; 	/* Set element count to 0 */
}

int main()
{
	nodeKeys = calloc(tableSize, sizeof(gnode_t));
	if(nodeKeys == NULL)
	{
		perror("Node key dictionary allocation error: ");
		exit(1);
	}

	char input[64]; /* Input operation name */
	char emailAddress[55]; /* Email address of a person */
	char secondEmailAddress[55]; /* Email address of another person */
	char firstName[25]; /* First name of the person */
	char lastName[25]; /* First name of the person */
	int numOperations; /* The number of operations. */
	scanf("%d", &numOperations);
	for (int i = 0 ; i < numOperations ; i++)
	{
		scanf("\n%s", input);
		if (strcmp(input, "New") == 0)
		{
			scanf(" %s %s %s", emailAddress, firstName, lastName);
			newAccount(emailAddress, firstName, lastName);
		}
		else if (strcmp(input, "Delete") == 0)
		{
			scanf(" %s", emailAddress);
			deleteAccount(emailAddress);
		}
		else if (strcmp(input, "Add") == 0)
		{
			scanf(" %s %s", emailAddress, secondEmailAddress);
			addFriend(emailAddress, secondEmailAddress);
		}
		else if (strcmp(input, "Unfriend") == 0)
		{
			scanf(" %s %s", emailAddress, secondEmailAddress);
			unfriend(emailAddress, secondEmailAddress);
		}
		else if (strcmp(input, "Friend") == 0)
		{
			scanf(" %s", emailAddress);
			listFriends(emailAddress);
		}
		else
		{
			scanf(" %s", emailAddress);
			listSuggestions(emailAddress);
		}
	}
	freeAll();
	fflush(stdout); /* Flush output (Sometimes output is stuck in the buffer) */
    return 0;
}