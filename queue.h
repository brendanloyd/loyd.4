#ifndef QUEUE_H
#define QUEUE_H 


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
typedef struct queue_struct {
        pid_t childPid;
        struct queue_struct *next;
} queue;

static queue *headptr;
static queue *tailptr;

void push(pid_t);
int pop(void);
int getlog(void);

#endif
