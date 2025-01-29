/* 
	VIP Taxi Queue
	Template + implemenetation of Fundamental Data Structures, skill 00010.

	Created by Pasin Manurangsi, 2025-01-08
	Modified by Luka Bond, 2025-01-25
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_SIZE 5

/* --------------- Data structures --------------- */

/* Singly linked list node */
typedef struct _node
{
	void* data;
	struct _node* nextNode;
} node_t;

/* Linked List */
typedef struct _linkedlist
{
	node_t* head;
	node_t* tail;
} linkedlist_t;

/* Custom Queue */
typedef struct _rankQueue
{
	linkedlist_t* list;
	int rank;
} rankQueue_t;

/* Custom stack */
typedef struct _stack
{
	rankQueue_t** arr; /* Pointer to the array */
	int size;
	int maxSize;
} stack_t;

/* Party/Group structure */
typedef struct _party
{
	int amount;
	char name[25];

} party_t;

/* --------------- Global variables --------------- */

stack_t queueStack = {NULL, 0, MAX_STACK_SIZE}; /* Stack of Queues */

/* --------------- Linked list-based Queue implementation --------------- */

/* 
Enqueue: Add data (of any type) to a queue.
@param _data: Pointer to data (of any type) to be stored
@param _queue: Pointer to the queue
*/
void enqueue(void* _data, linkedlist_t* _queue)
{
	node_t* newNode = calloc(1, sizeof(node_t));
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
Dequeue: Remove data (of any type) from the front of queue and return it.
@param _queue: Pointer to the queue
@return: Pointer to the allocated data
*/
void* dequeue(linkedlist_t* _queue)
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

/* --------------- Array-based stack implementation --------------- */

/* 
push: Add data (of any type) to the top of the stack.
@param _data: Pointer to data (of any type) to be stored
@param _stack: Pointer to the stack
*/
void push(void* _data, stack_t* _stack)
{
	if(_stack->size == _stack->maxSize)
	{
		printf("Cannot add to stack, an overflow may occur\n");
		return;
	}
	++_stack->size;
	_stack->arr[(_stack->size) - 1] = _data;
}

/* 
pop: Remove data (of any type) from the top of stack and return it. 
@param _stack: Pointer to the stack
@return: Pointer to the allocated data
*/
void* pop(stack_t* _stack)
{	
	if(_stack->size == 0)
	{
		return NULL;
	}
	--_stack->size;
	return _stack->arr[_stack->size - 1]; /* Extract pointer to the data */
}

/* 
top: Return data (of any type) from the top of stack. 
@param _stack: Pointer to the stack
@return: Pointer to the allocated data
*/
void* top(stack_t* _stack)
{	
	if(_stack->size == 0)
	{
		return NULL;
	}
	return _stack->arr[_stack->size - 1]; /* Extract pointer to the data */
}

/* --------------- Helper functions --------------- */

/* 
freeList: Helper function to free all of the contents from the linked list
@param list: Pointer to the linked list
*/
void freeList(linkedlist_t* list)
{
	while(list->head != NULL)
	{
		/* Dequeue frees each node already */
		void* _data = dequeue(list); 
		free(_data);
		_data = NULL;
	}
}

/* 
createQueue: Helper function to create rank queues. 
@param rank: The rank of the queue
@return: Pointer to the queue
*/
rankQueue_t* createRankQueue(int rank)
{
	rankQueue_t* newQueue = calloc(1, sizeof(rankQueue_t));
	if(newQueue == NULL) 
	{
		printf("Failed to allocate new queue\n");
		exit(1);
	}

	linkedlist_t* newList = calloc(1, sizeof(linkedlist_t));
	if(newList == NULL) 
	{
		printf("Failed to allocate new linked list\n");
		exit(1);
	}

	newQueue->list = newList;
	newQueue->rank = rank;

	/* Push new queue into the stack */
	push(newQueue, &queueStack);
	return newQueue;
}

/* 
createQueue: Helper function to free rank queues. 
@param rankQueue: The pointer to the rank queue
*/
void freeRankQueue(rankQueue_t* rankQueue)
{
	linkedlist_t* list = rankQueue->list;
	
	/* Free all nodes from the list */
	freeList(list);

	/* Free pointer to struct containing list head and tail */
	free(list);
	list = NULL;

	/* Free pointer to struct containg rank and list */
	free(rankQueue);
	rankQueue = NULL;
}

/* --------------- Taxi/Party arrival --------------- */

/* 
taxiArrival	: Handles an arrival of a new taxi
*/
void taxiArrival()
{
	rankQueue_t* topQueue = top(&queueStack);
	if(topQueue == NULL) /* No parties of any rank*/
	{
		printf("Empty Queue\n");
		return;
	}

	linkedlist_t* partyList = topQueue->list;
	party_t* party = dequeue(partyList);

	printf("Pick up party %s with %d people.\n", party->name, party->amount);
	free(party);
	party = NULL;

	if(partyList->head == NULL)
	{
		/* Remove queue if list is empty */
		freeRankQueue(topQueue);
		pop(&queueStack);
	}
}

/* 
partyArrival: Handles an arrival of a new party.
@param count: The number of people in the party
@param partyName: The name of the party
*/
void partyArrival(int rank, int count, char partyName[])
{
	rankQueue_t* topQueue = top(&queueStack);
	if(topQueue == NULL) /* Stack is empty */
	{
		topQueue = createRankQueue(rank); /* Reassign stack head */
	}

	if(rank < topQueue->rank) /* Low priority */
	{
		printf("Rejected\n");
		return;
	}
	else if(rank > topQueue->rank) /* High priority, create new queue */
	{
		topQueue = createRankQueue(rank); /* Reassign stack head */
	}
	
	/* Current rank = input rank */
	party_t* newParty = calloc(1, sizeof(party_t));
	if(newParty == NULL) 
	{
		printf("Failed to allocate new party\n");
		exit(1);
	}

	newParty->amount = count;
	strncpy(newParty->name, partyName, 20);
	enqueue(newParty, topQueue->list);
	printf("Accepted\n");
}	

/* 
freeAll: Free the contents of the datastructures used 
*/
void freeAll()
{
	for (int i = 0; i < queueStack.maxSize; ++i)
	{
		if(queueStack.arr[i] == NULL)
		{
			continue;
		}

		freeRankQueue(queueStack.arr[i]);
		queueStack.arr[i] = NULL;
	}
}

/* --------------- Main --------------- */

/* 
Main function loops for however many timesteps is specified, 
reading from the standard input and handling taxi / party arrivals
*/
int main(int argc, char* argv[])
{
 	int timesteps; /* The total number of timesteps */
 	char input[10]; /* For the input prefix "Taxi" or "Party" */
 	char name[25]; /* The party name */
 	int rank; /* The party's status */
 	int count; /* The number of people in the party */

	rankQueue_t* queueArr[MAX_STACK_SIZE] = { NULL }; /* Array of pointers to queues */
	queueStack.arr = queueArr; /* Set stack array pointer */

 	scanf("%d", &timesteps);
 	for (int i = 0 ; i < timesteps ; i ++)
 	{
 		scanf("\n%s", input);
 		if (strcmp(input, "Taxi") == 0)
 		{
 			taxiArrival();
 		}
 		else
 		{
 			scanf(" %d %d %s", &rank, &count, name);
 			partyArrival(rank, count, name);
 		}
 	}
 	freeAll();
}
