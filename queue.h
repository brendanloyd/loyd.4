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
static queue *headptrBlocked;
static queue *tailptrBlocked;

void pushReady(pid_t);
int popReady(void);
int getlogReady(void);
void pushBlocked(pid_t);
int popBlocked(void);
int getlogBlocked(void);
#endif
