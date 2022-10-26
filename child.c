#include "child.h"

int main(int argc, char** argv) {
	//variables for message queue
	message buf;
        key_t key;
        int msqid = 0;
        const int key_id = 1234;

	srand((unsigned) getpid());

	//setup message queue	
	key = ftok("./parent.c", key_id);
        msqid = msgget(key, 0644|IPC_CREAT);
	
	// recieve message if there is one, if not wait.
	msgrcv(msqid, &buf, sizeof(buf), 1, 0);
	
	//announce entry into critical section then go to sleep;
	int choice = (rand() % 1000);
	if(choice < 400) {
		buf.mint = 1000;	
	} else if (choice > 400 && choice < 800) {
		buf.mint = (rand() % 1000);
	} else {
		buf.mint = ((rand() % 1000) * -1);
	}
	buf.mtype = 1;
        
	if (msgsnd(msqid, &buf, sizeof(buf), 0) == -1) {
                perror("msgsnd");
                exit(1);
        }
	
	//detach shared memory and return success
	return EXIT_SUCCESS;
}


