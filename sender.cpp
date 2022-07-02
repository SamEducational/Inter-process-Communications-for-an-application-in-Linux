/* File: sender.cpp */

#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include "msg.h"    /* For the message struct */

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void* sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{

	 std::ofstream outFile;
	 outFile.open("keyfile.txt");
	 outFile << "Hello World";
	 outFile.close();
	 key_t key = ftok("keyfile.txt", 'a');

	/* Allocates a piece of shared memory */
	shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, IPC_CREAT | 0666);
	if (shmid < 0)
	{
		perror("shmget");
		exit(1);
	}
	else
	{
		std::cout << " ID of the shared memory segment is acquired - sender, line 45\n";
    }
    
	/* Attaches to share memory */
	sharedMemPtr = shmat(shmid, NULL, 0);
	if (sharedMemPtr == (char *)(-1))
	{
		perror("shmat");
		exit(1);
	}
	else
	{
		std::cout << " Attached to shared memory - sender, line 57\n";
	}
	
    /* Attaches to message queue */
	msqid = msgget(key, 0666);
	if (msqid < 0)
	{
		perror("msgget - sender, line 64");
		exit(1);
	}
	else
	{
		std::cout << " Attached to message queue - sender, line 69\n";
	}
	
} // end of init




/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	std::cout << " Detaching from shared memory\n";
	shmdt(sharedMemPtr);

} // end of cleanUp




/**
 * The main send function
 * @param fileName - the name of the file
 */
 
void send(const char* fileName)
{
	/* Open the file for reading */
	FILE* fp = fopen(fileName, "r");

	/* A buffer to store message we will send to the receiver. */
	message sndMsg;
	sndMsg.mtype = SENDER_DATA_TYPE;

	/* A buffer to store message received from the receiver. */
	message rcvMsg;
	rcvMsg.mtype = RECV_DONE_TYPE;
	
	/* Was the file open? */
	if(!fp)
	{
		perror("fopen - sender, line 116");
		exit(-1);
	}

	/* Read the whole file */
	while(!feof(fp))
	{
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and store them in shared memory.
 		 * fread will return how many bytes it has actually read (since the last chunk may be less
 		 * than SHARED_MEMORY_CHUNK_SIZE).
 		 */
		if((sndMsg.size = fread(sharedMemPtr, sizeof(char), SHARED_MEMORY_CHUNK_SIZE, fp)) < 0)
		{
			perror("fread");
			exit(-1);
		}
		std::cout << " Number of bytes sent:" << sndMsg.size << "\n";

		/* Send a message to the receiver telling him that the data is ready */
		if (msgsnd(msqid, &sndMsg, sizeof(struct message) - sizeof(long), 0) < 0)
		{
		    perror("msgsnd - sender, line 137");
		}    
		else
		{
		    std::cout << " Reciever, message is ready - sender, line 141\n";
		}
		
		/* TODO: Wait until the receiver sends us a message of type RECV_DONE_TYPE telling us
 		 * that he finished saving the memory chunk.
 		 */
		if (msgrcv(msqid, &rcvMsg, sizeof(struct message) - sizeof(long), RECV_DONE_TYPE, 0)  == 0)
		{
			perror("msgrcv - sender, line 149");
			exit(1);
		}
		else
		{
			std::cout << " Receiver, we acquired your message - sender, line 154\n";
		}
	}

	/** Tells the receiver that we have nothing more to send. The message is of type
 	  * SENDER_DATA_TYPE with size field set to 0.
	  */
	sndMsg.size = 0;
	if (msgsnd(msqid, &sndMsg, sizeof(struct message) - sizeof(long) , 0) < 0)
	{
		perror("msgsnd - sender, line 164");
	}
	else
	{
		std::cout << " Receiver, there's nothing else to read - sender, line 168\n";
	}
	
	/* Close the file */
	fclose(fp);

} // end of send




int main(int argc, char** argv)
{

	/* Check the command line arguments */
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}

	/* Connect to shared memory and the message queue */
	init(shmid, msqid, sharedMemPtr);

	/* Send the file */
	send(argv[1]);

	/* Cleanup */
	cleanUp(shmid, msqid, sharedMemPtr);

	return 0;
}
