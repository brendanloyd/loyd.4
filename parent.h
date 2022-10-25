#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h> 

void terminateSigHandler(int);
void timeoutSigHandler(int); 

#define PERMS 0644
#define BUFF_SZ sizeof ( int )

int* mem_ptr;
extern int errno;

typedef struct my_msgbuf {
	long mtype;
	int mint;
} message;

typedef struct sclock {
	int seconds;
	int nanoSeconds;
} sclock;
