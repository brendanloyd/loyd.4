#ifndef QUEUE_C
#define QUEUE_C 

#include "queue.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>

/*typedef struct queue_struct {
	pid_t childPid;
 	struct queue_struct *next;
} queue;*/

static queue *headptr = NULL;
static queue *tailptr = NULL;

void push(pid_t childPidToPush) {
	queue *newnode;
	int nodesize;
	nodesize = (sizeof(childPidToPush) + 2);
	
	if ((newnode = (queue *)(malloc(nodesize))) == NULL) {
		perror("Error: addmsg ");
		exit(-1);
	}
	newnode->childPid = childPidToPush;
	newnode->next = NULL;

	if (headptr == NULL)
	headptr = newnode;
	else
	tailptr->next = newnode;
	tailptr = newnode;

}

int isNotEmpty() {
	if(headptr = NULL) {
	return 0;
	}
	return 1;
}

pid_t pop() {
	pid_t toPop = headptr->childPid;
 	queue *nextNode = headptr;
 	nextNode = headptr->next;
        free(headptr);
        headptr = nextNode;

	return toPop;		

}

int getlog() {
	//int stringOffset = 0, sum = 0;
	queue *ptr = headptr;
	int processesInQueue = 0;
	/*char * finalString = NULL;
	size_t stringlen;*/
	//printf("outside get log while loop\n");	
	while (ptr != NULL) {
		//printf("inside get log while loop\n");
		/*stringlen = strlen(ptr->item.string);
		sum += stringlen;

		finalString = (char*) realloc(finalString, (sum + stringOffset));
		strncat(finalString, ptr->item.string, stringlen);
		ptr = ptr->next;
		stringOffset += sum;
		
		finalString = (char *) realloc(finalString, (sum + 1));
		finalString[sum + 1] = 0x00;*/
		processesInQueue++;
		ptr = ptr->next;

	}
	return processesInQueue;
}


/*
int savelog(char *filename) {
	FILE *out_file = fopen(filename, "a");	

	if(out_file == NULL){
		perror("Error: savelog ");
		return -1;
	} else {
		fprintf(out_file, "%s", getlog());
	} 

	return 0;
}

void clearlog(void) {
	list_t * nextNode = headptr;
	
	while (headptr != NULL) {
		nextNode = headptr->next;
		free(headptr);
		headptr = nextNode;
	}
}*/

#endif
