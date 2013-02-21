/******************************************************************************
 * Class:       reader.c
 * Author:      Jason Tierney
 * Date:        February 21, 2013
 * Description: Writes data to shared memory and writes it on the screen.
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

// Size of our memory structure.
#define FOO 4096

// Struct that defines our shared memory space.
struct SharedMemory
{
	int writer;
	char text[FOO];
};

/**
 * Main entry point to the application.
 */
int main()
{
	struct SharedMemory *sharedMemory;
	char buffer[FOO];
	int isRunning = 1;
	void *shared_memory = (void*) 0;		
	int shmId;
	int key = getuid();
	shmId = shmget((key_t)key, sizeof(struct SharedMemory), 0666 | IPC_CREAT);
	if(shmId == -1)
	{
		fprintf(stderr, "shmget failed ... miserably.\n");
		exit(0);
	}

	shared_memory = shmat(shmId, (void *) 0, 0);
	
	// Handle an error with shmat.
	if(shared_memory == (void *) -1)
	{
		fprintf(stderr, "There was an error with the call to shmat.\n");
		exit(0);
	}

	printf("Memory starts at %X\n", (int)shared_memory);
	sharedMemory = (struct SharedMemory *)shared_memory;
	while(isRunning)
	{
		while(sharedMemory->writer == 1)
		{
			sleep(1);
			printf("We are a terminal in waiting...\n");
		}
		
		printf("What would you like to say? ");
		fgets(buffer, BUFSIZ, stdin);
		strncpy(sharedMemory->text, buffer, FOO);
		sharedMemory->writer = 1;
		if(strncmp(buffer, "quit", 4) == 0)
		{
			isRunning = 0;
		}
	}

	// Handle an error for shmdt.
	if(shmdt(shared_memory) == -1)
	{
		fprintf(stderr, "There was an error with the call to shmdt.\n");
		exit(0);
	}

	return 0;
}
