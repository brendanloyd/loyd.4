#include "parent.h"
#include "queue.c"

//signal handler function to kill program if ctrl c is encountered
void terminateSigHandler(int sig) {
		printf("SIGINT signal encountered. Terminating.\n");
		printf("Clock value is: %d\n",*mem_ptr);
                shmdt(mem_ptr);
                kill(0, SIGQUIT);
}

//signal handler function to timeout after a certain determined about of time
void timeoutSigHandler(int sig) {
	if(sig == SIGALRM) {
		printf("SIGALRM signal ecountered indicating a timeout. Terminating\n");
	 	printf("Clock value is: %d\n",*mem_ptr);
		shmdt(mem_ptr);	
		kill(0, SIGQUIT);	
	}

}

void incrementClock(sclock *clock, int nanoIncrement) {
	if(nanoIncrement < 0) {
		nanoIncrement *= -1;
		clock->nanoSeconds += nanoIncrement;
	} else {
		clock->nanoSeconds += nanoIncrement;
	}
	if(clock->nanoSeconds > 1000000000) {
		clock->nanoSeconds -= 1000000000;
		clock->seconds += 1;
	}
}
void forkChildren(int num) {
        int i;
	for(i = 0; i < num; i++) {
                if (fork() == 0) {
                        char* args[] = {"./child", 0};
                        execlp(args[0],args[0],args[1]);
                        fprintf(stderr,"Exec failed, terminating\n");
                        exit(1);
                }
        }
}

int main(int argc, char **argv) {

	//Variables for signal handling
        signal(SIGTERM, terminateSigHandler);
        signal(SIGALRM, timeoutSigHandler);
        alarm(30);
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = terminateSigHandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	//Variables for message queue
	message buf;
        buf.mtype = 1;
        buf.mint = 1000;
        key_t key;
        int msqid = 0;
        const int key_id = 1234;

        //Setup key for message queue
        key = ftok("./parent.c",key_id);

        //Setup id for message queue
        msqid = msgget(key, 0644|IPC_CREAT);

	//random values needed for system times
	srand(time(NULL));

	//Setup clock
	sclock clock;
	clock.seconds = 1;
	clock.nanoSeconds = 0;

	//for wait command and geting childpid
	pid_t wpid;
        int status = 0;
	pid_t childPid;
	
	//for while loop establishing time to pop from queues, release and keep track of Children	
        int popAChild = 9000;
        int releaseChildren = 20000;
        int pushBlockedChildToReady = 15000;
	int totalChildren = 0;

	//create file for output
	FILE *out_file = fopen("schedule.log", "w");

	//create process control block variable
	pcb processControlBlock[20];

	//start 3 seconds and spawn children
	time_t endwait = time(NULL) + 3;
	forkChildren(20);
	totalChildren += 20;

	//send first message
	if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
        	perror("msgsnd");
                exit(1);
        }
	int firstRun = 1;
	while(time(NULL) < endwait && totalChildren < 100) {
		if(isNotEmptyReady() && clock.nanoSeconds > popAChild) {
			childPid = popReady();
			popAChild += 7000;
			fprintf(out_file, "OSS: Popping a child process pid: %d from queue\n", childPid);
		} else if(isNotEmptyBlocked() && clock.nanoSeconds > pushBlockedChildToReady) {
			childPid = popBlocked();
			pushReady(childPid);
			fprintf(out_file, "OSS: Popping a child process pid: %d from blocked queue and pushing to ready queue\n", childPid);
			pushBlockedChildToReady += 15000;
		}

		fprintf(out_file, "OSS: Dispatching child at second :%d nanoSecond:%d\n", clock.seconds, clock.nanoSeconds);
		int dispatchTime = (rand() % 10000) + 100;
                incrementClock(&clock, dispatchTime);
		fprintf(out_file, "OSS: Total dispatching time in nanoSeconds:%d\n", dispatchTime);

		childPid = wait(0);
		
		msgrcv(msqid, &buf, sizeof(buf), 1, 0);
		if (buf.mint < 0) {
			fprintf(out_file, "OSS: Child :%d sent termination message using :%d nanoSeconds\n", childPid, (buf.mint* -1));
		} else if (buf.mint == 1000) {
			fprintf(out_file, "OSS: Child :%d used all the time available.\n", childPid);
			pushReady(childPid);
			fprintf(out_file, "OSS: Pushing child :%d into ready queue..\n", childPid);
		} else {
			fprintf(out_file, "OSS: Child :%d didn't use all the time available.\n", childPid);
			pushBlocked(childPid);
			fprintf(out_file, "OSS: Pushing child :%d into blocked queue..\n", childPid);
		}
		if(firstRun) {
			int k;
			int pid = childPid;
			for(k = 0; k < 20; k++) {
				processControlBlock[k].child = pid;
				pid++;
			}
			firstRun = 0;
		}
		int u;
		for(u = 0; u < 20; u++) {
			int pid = childPid;
			int childInControlBlock = processControlBlock[u].child;
			if(pid == childInControlBlock) {
				if (buf.mint < 0) {
				processControlBlock[u].totalNanoSeconds += (buf.mint * -1);
				processControlBlock[u].lastBurst = (buf.mint * -1);
				} else {
				processControlBlock[u].totalNanoSeconds += buf.mint;
                                processControlBlock[u].lastBurst = buf.mint;
				}	
			}
		}
		
		incrementClock(&clock, buf.mint);
                if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
                        perror("msgsnd");
                        exit(1);
                }
		if(clock.nanoSeconds > releaseChildren) {
			forkChildren(10);
			totalChildren += 10;
			releaseChildren += 20000;
		}

	}
	while ((wpid = wait(&status)) > 0);
	fprintf(out_file, "OSS: Final clock value in seconds is: %d : NanoSeconds is : %d\n", clock.seconds, clock.nanoSeconds);	
	
	int k;
	for(k = 0; k < 20; k++) {
		fprintf(out_file, "child pid :%d spent: %d nanoSeconds on the core with a last burst of: %d\n", processControlBlock[k].child,processControlBlock[k].totalNanoSeconds,processControlBlock[k].lastBurst);
	}
	//detach message queue memory	
	if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      		perror("msgctl");
      		exit(1);
   	}
	
	return EXIT_SUCCESS; 
}
