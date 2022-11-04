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
static queue *headptrBlocked = NULL;
static queue *tailptrBlocked = NULL;

void pushReady(pid_t childPidToPush) {
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

void pushBlocked(pid_t childPidToPush) {
        queue *newnode;
        int nodesize;
        nodesize = (sizeof(childPidToPush) + 2);

        if ((newnode = (queue *)(malloc(nodesize))) == NULL) {
                perror("Error: addmsg ");
                exit(-1);
        }
        newnode->childPid = childPidToPush;
        newnode->next = NULL;

        if (headptrBlocked == NULL)
        headptrBlocked = newnode;
        else
        tailptrBlocked->next = newnode;
        tailptrBlocked = newnode;

}

int isNotEmptyReady() {
	if(headptr == NULL) {
	return 0;
	}
	return 1;
}

int isNotEmptyBlocked() {
        if(headptrBlocked == NULL) {
        return 0;
        }
        return 1;
}

int popReady() {

	pid_t toPop;
	toPop = (sizeof(headptr->childPid) + 1);
	toPop = headptr->childPid;
 	queue *nextNode = headptr;
 	nextNode = headptr->next;
        free(headptr);
        headptr = nextNode;

	return toPop;		

}

int popBlocked() {

        pid_t toPop;
        toPop = (sizeof(headptrBlocked->childPid) + 1);
        toPop = headptrBlocked->childPid;
        queue *nextNode = headptrBlocked;
        nextNode = headptrBlocked->next;
        free(headptrBlocked);
        headptrBlocked = nextNode;

        return toPop;

}

int getlogReady() {
	queue *ptr = headptr;
	int processesInQueue = 0;
	while (ptr != NULL) {
		processesInQueue++;
		ptr = ptr->next;

	}
	return processesInQueue;
}

int getlogBlocked() {
        queue *ptr = headptrBlocked;
        int processesInQueue = 0;
        while (ptr != NULL) {
                processesInQueue++;
                ptr = ptr->next;

        }
        return processesInQueue;
}
#endif
