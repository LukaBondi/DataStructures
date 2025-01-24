/* 
	Airport Taxi Queue Template
 	Created by Pasin Manurangsi, 2025-01-08
	Modified by Luka Bond, 2025-01-24
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --------------- Data structures --------------- */

/* Doubly linked list node */
typedef struct _node
{
	void* data;
	struct _node* prevNode;
	struct _node* nextNode;
} node_t;

/* Linked List-based Queue */
typedef struct _queue
{
	node_t* head;
	node_t* tail;
} queue_t;

/* Party/Group structure */
typedef struct _party
{
	int amount;
	char name[20];

} party_t;

/* ---------------Global variables --------------- */

queue_t partyQueue = { NULL, NULL };
queue_t taxiQueue = { NULL, NULL };
int taxiRegistration = 1;
int servedGroups = 0; 
int inlineGroups = 0; 

/* 
Enqueue: Add data (of any type) to a specified queue.
@param _data: Pointer to data (of any type) to be stored
@param _queue: Pointer to the queue
*/
void enqueue(void* _data, queue_t* _queue)
{
	node_t* newNode = calloc(1, sizeof(node_t));
	if(newNode == NULL)
	{
		printf("Failed to allocate new node\n");
		exit(1);
	}

	/* Filling node attributes */
	newNode->data = _data; /* Fill data */
	newNode->prevNode = _queue->tail; /* Fill current node */
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
void* dequeue(queue_t* _queue)
{	
	if(_queue->head == NULL)
	{
		return NULL;
	}

	node_t* prevHead = _queue->head;
	void* item = (_queue->head)->data;
	

	/* Reassign new head */
	_queue->head = (_queue->head)->nextNode;

	/* Free and set old pointer to NULL*/
	free(prevHead);
	prevHead = NULL;

	return item;
}

/* 
freeAll: Free all of the contents from the queue
@param _queue: Pointer to the queue
*/
void freeAll(queue_t* _queue)
{
	while(_queue->head != NULL)
	{
		void* _data = dequeue(_queue); /* Dequeue frees each node already */
		free(_data);
		_data = NULL;
	}
}

/* 
partyArrival: Handles an arrival of a new taxi
*/
void taxiArrival()
{
	party_t* _party = dequeue(&partyQueue);
	if(_party == NULL) /* If there are no parties, Queue taxi */
	{
		enqueue((void*)(uintptr_t)taxiRegistration, &taxiQueue);
	}
	else 
	{
		printf("Taxi %d (Arriving Taxi): Pick up party %s with %d people.\n",  taxiRegistration, _party->name, _party->amount);
		++servedGroups;
		--inlineGroups;

		free(_party);
		_party = NULL;
	}
	++taxiRegistration;
}

/* 
partyArrival: Handles an arrival of a new party.
@param count: The number of people in the party
@param partyName: The name of the party
*/
void partyArrival(int count, char partyName[])
{
	if(taxiQueue.head != NULL) /* If there is a taxi, immediate departure (No queue) */
	{
		printf("Taxi %d (Queued Taxi): Pick up party %s with %d people.\n", (int)(uintptr_t)dequeue(&taxiQueue), partyName, count);
		++servedGroups;
		return;
	}

	party_t* newParty = calloc(1, sizeof(party_t));
	if(newParty == NULL) 
	{
		printf("Failed to allocate new party\n");
		exit(1);
	}

	newParty->amount = count;
	strncpy(newParty->name, partyName, 20);
	enqueue(newParty, &partyQueue);
	++inlineGroups;
}	


/* Main function loops for however many timesteps is specified, 
 * reading from the standard input and handling taxi / party arrivals
 */
int main(int argc, char* argv[])
{
 	int timesteps; // The total number of timesteps
 	char input[10]; // For the input prefix "Taxi" or "Party"

 	char name[20]; // The party name 
 	int rank; // The party's status
 	int count; // The number of people in the party

 	scanf("%d", &timesteps);
 	for (int i = 0 ; i < timesteps ; i ++)
 	{
 		scanf("\n%s", input);
 		if (strcmp(input, "Taxi") == 0)
 		{
 			taxiArrival();
 		}
 		else if(strcmp(input, "Party") == 0)
 		{
 			scanf(" %d %s", &count, name);
 			partyArrival(count, name);
 		}
 	}
 	freeAll(&partyQueue);
	printf("\nServed: %d groups, In line: %d groups", servedGroups, inlineGroups);
}
