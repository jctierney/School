/******************************************************************************
 * Class:       reader.c
 * Author:      Jason Tierney
 * Date:        February 21, 2013
 * Description: Reads data from shared memory and writes it on the screen.
 * 
 * Copyright:   N/A : Academic purposes.
 *****************************************************************************/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define size 4096

// A struct that defines our shared memory space.
struct SharedMem
{
	int isWriter;
	char text[size];
};

/**
 * Main entry point to the program.
 */
int main()
{
	int isRunning = 1;
	void *shared_memory = (void *) 0;
	struct SharedMem * sharedMemory;
	int shmid;
	int key = getuid();
	srand((unsigned int) getpid());
	shmid = shmget((key_t)key, sizeof(struct SharedMem), 0666 | IPC_CREAT);
	if(shmid == 1)
	{
		fprintf(stderr, "There was an error with the call to shmget.\n");
		exit(EXIT_FAILURE);
	}
	
	shared_memory = shmat(shmid, (void *) 0, 0);
	if(shared_memory == (void*) -1)
	{
		fprintf(stderr, "There was an error with the call to shmat.\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Memory attached at %X\n", (int)shared_memory);
	sharedMemory = (struct SharedMem *) shared_memory;
	sharedMemory->isWriter = 0;
	while(isRunning)
	{
		if(sharedMemory->isWriter) // Check to see if we have info to share.
		{
			printf("You wrote: %s", sharedMemory->text);
			sleep(1);
			sharedMemory->isWriter = 0;
			if(strncmp(sharedMemory->text, "quit", 4) == 0)
			{
				isRunning = 0;
			}
		}
	}

	// Log failure of shmdt
	if(shmdt(shared_memory) == -1)
	{
		fprintf(stderr, "There was an error with the call to shmdt\n");
		exit(0);
	}

	// Log failure of shmctl.
	if(shmctl(shmid, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "There was an error with shmctl(IPC_RMID).\n");
		exit(EXIT_FAILURE);
	}
	
	// Exit on success, nothing failed.
	exit(EXIT_SUCCESS);
}
