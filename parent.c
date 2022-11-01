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
}

int main(int argc, char **argv) {

	//Variables for handling Getopt options and loop to run child processes
	int option, totalChildProcesses = 4, clockIncrement = 1;
	int childrenRunningAtOneTime = 2;
        int childProcessCounter;

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

	//random values needed for system times
	srand(time(NULL));

	//Setup clock
	sclock clock;
	clock.seconds = 0;
	clock.nanoSeconds = 0;
	
	//create file for output
	FILE *out_file = fopen("schedule.log", "w");

	//Getopt options for program
	while ((option = getopt(argc, argv, "hn:s:m:")) != -1) {
                switch (option) {
                        case 'h' :
                                printf("To run this program please use the following format:\n");
                                printf("./oss [-h] [-n] [-s] [-m]\nWhere [-n] [-s] [-m] require arguments.\n");
				printf("Default process is: [./oss -n 4 -s 2]\n");
                                return 0;

                        case 'n' :
				totalChildProcesses = (atoi(optarg));
				break;

			case 's' :
				childrenRunningAtOneTime = (atoi(optarg));
				if (childrenRunningAtOneTime > 18) {
				perror("Error: parent.c : Can't be more than 18 child processes running at one time.");
				exit(-1);
				}
				break;

			case 'm' :
				clockIncrement = (atoi(optarg));
				break;

                        case '?':
                                printf("Driver : main : Invalid option.\n");
                                exit(-1);

                        default :
                                printf("Driver : main : Invalid argument\n");
                                return 0;
                
		}
        }

	//Setup key for message queue
	key = ftok("./parent.c",key_id);

	//Setup id for message queue
        msqid = msgget(key, 0644|IPC_CREAT);

	//For wait command
	pid_t wpid;
	int status = 0;
	int runningTotalChildProcesses = 0;
	clock.seconds += 1;
	time_t endwait = time(NULL) + 3;

	int i;
	for(i = 0; i < totalChildProcesses; i++) {
                if (fork() == 0) {
                        char* args[] = {"./child", 0};
                        execlp(args[0],args[0],args[1]);
                        fprintf(stderr,"Exec failed, terminating\n");
                        exit(1);
                }	
	}
	
	if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
        	perror("msgsnd");
                exit(1);
        }	
	pid_t childPid;
	while(time(NULL) < endwait && totalChildProcesses > runningTotalChildProcesses) {
		runningTotalChildProcesses++;
		/*if(isNotEmpty()) {
			printf("queue has %d processes in it\n", getlog());
			childPid = pop();
			char* args[] = {"./child", 0};
                        execlp(args[0],args[0],args[1]);
			printf("queue has %d processes after pop\n", getlog());
		}*/

		fprintf(out_file, "OSS: Dispatching child at second :%d nanoSecond:%d\n", clock.seconds, clock.nanoSeconds);
		int dispatchTime = (rand() % 400);
                incrementClock(&clock, dispatchTime);
		fprintf(out_file, "OSS: Total dispatching time in nanoSeconds:%d\n", dispatchTime);

		childPid = wait(NULL);
		
		msgrcv(msqid, &buf, sizeof(buf), 1, 0);
		if (buf.mint < 0) {
			fprintf(out_file, "OSS: Child :%d sent termination message using :%d nanoSeconds\n", childPid, (buf.mint* -1));
		} else if (buf.mint == 1000) {
			fprintf(out_file, "OSS: Child :%d used all the time available.\n", childPid);
			push(childPid);
                        fprintf(out_file, "OSS: Number of processes in blocked queue is: %d\n", getlog());
			//runningTotalChildProcesses--;
		} else {
			fprintf(out_file, "OSS: Child :%d didn't use all the time available.\n", childPid);
			fprintf(out_file, "OSS: Pugging child :%d into blocked queue..\n", childPid);
			push(childPid);
			fprintf(out_file, "OSS: Number of processes in blocked queue is: %d\n", getlog());
			//runningTotalChildProcesses--;
		}
		
		incrementClock(&clock, buf.mint);
                if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
                        perror("msgsnd");
                        exit(1);
                }

	}
	while ((wpid = wait(&status)) > 0);
	fprintf(out_file, "OSS: Final clock value in seconds is: %d : NanoSeconds is : %d\n", clock.seconds, clock.nanoSeconds);	
	
	//detach message queue memory	
	if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      		perror("msgctl");
      		exit(1);
   	}
	
	return EXIT_SUCCESS; 
}
