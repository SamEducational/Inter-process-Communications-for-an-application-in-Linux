/* File: recv.cpp*/

#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
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
void *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile";


/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{

	std::ofstream outFile;
	outFile.open("keyfile.txt");
	outFile << "Hello World";
	outFile.close();
	key_t key = ftok("keyfile.txt", 'a');

	/*Allocates a piece of shared memory*/
	shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, IPC_CREAT | 0666);
	if (shmid < 0)
	{
		perror("shmget - receiver, line 47");
		exit(1);
	}
	else
	{
		std::cout << " Allocated a piece of shared memory - receiver, line 52\n";
	}
	
    /*Attaches to share memory*/
	sharedMemPtr = shmat(shmid, NULL, 0);
	if (sharedMemPtr == (char *)(-1))
	{
		perror("shmat - receiver, line 59");
		exit(1);
	}
	else
	{
		std::cout << " Attached to shared memory - receiver, line 64\n";
	}
	
    /*Creates message queue*/
	msqid = msgget(key, IPC_CREAT | 0644);
	if (msqid < 0)
	{
		perror("msgget - receiver, line 71");
		exit(1);
	}
	else
	{
		std::cout << " Message queue is created - receiver, line 76\n";
	}
	
} // end of init




/**
 * The main loop
 */

void mainLoop()
{
	/* The size of the mesage */
	int msgSize = 0;

	message sndMsg;
	sndMsg.mtype = RECV_DONE_TYPE;

	message rcvMsg;
	rcvMsg.mtype = SENDER_DATA_TYPE;

	/* Open the file for writing */
	FILE* fp = fopen(recvFileName, "w");

	/* Error checks */
	if(!fp)
	{
		perror("fopen - receiver, line 105");
		exit(-1);
	}

    /*Attempt to receive message*/
	msgSize = msgrcv(msqid, &rcvMsg, sizeof(struct message) - sizeof(long), rcvMsg.mtype, 0);
	if (msgSize < 0)
		{
			perror(" msgrcv: Error when receiving - receiver, line 113");
			fclose(fp);
			exit(1);
		}
	else
	{
		std::cout << " Message received - receiver, line 119\n";
	}
	
	/* Keep receiving until the sender set the size to 0, indicating that
 	 * there is no more data to send and ready to close file
 	 */
	while(msgSize != 0)
	{
		/* If the sender is not telling us that we are done, then get to work */
		if(msgSize != 0)
		{
			/* Save the shared memory to file */
			if(fwrite(sharedMemPtr, sizeof(char), rcvMsg.size, fp) < 0)  
			{
				perror("fwrite - receiver, line 133");
			}
			else
			{
				std::cout << " Shared memory has been saved to file - receiver, line 137\n";
			}
			
			std::cout << " Number of bytes received: "  << rcvMsg.size << "\n";

            /*Send message to sender that we're ready for the next file chunk*/
			msgSize =  msgsnd(msqid, &sndMsg, sizeof(struct message) - sizeof(long), 0) ;
			if (msgSize == -1)
			{
				perror("msgsnd - receiver, line 146");
				exit(1);
			}
			else
			{
				std::cout << " Sender, we're done with the message - receiver, line 151\n";
			}
		}
		else
		{
			fclose(fp);
		}
		
		/*Attempt to receive message to see if size is 0*/
		msgSize = msgrcv(msqid, &rcvMsg, sizeof(struct message) - sizeof(long), rcvMsg.mtype, 0);
		if (msgSize == 0)
		{
			perror("msgrcv - receiver, line 163\n");
			exit(1);
		}
		else
		{
			std::cout << " All right, there's nothing more - receiver, line 168\n";
		}
	}

} // end of mainLoop




/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* Detach from shared memory */
	std::cout << " Detaching from shared memory - recevier, line 187\n";
	shmdt(sharedMemPtr);
	
	/* Deallocate the shared memory chunk */
	std::cout << " Deallocating the shared memory chunk - receiver, line 191\n";
	shmctl(shmid, IPC_RMID, NULL);
	
	/* Deallocate the message queue */
	std::cout << " Deallocate the message queue - receiver, line 195\n";
	msgctl(msqid, IPC_RMID, NULL);

} // end of cleanUp




/**
 * Handles the exit signal
 * @param signal - the signal type
 */

void ctrlCSignal() 
{
	/* Free system V resources */
	cleanUp(shmid, msqid, sharedMemPtr);
	
} // end of ctrlCSignal




/**
 * Implements the termination by pressing Ctrl - C
 */
 
void signalHandlerFunc(int arg)
{
	std::cout << "recv terminated\n";
	ctrlCSignal();
	exit(0);

} // end of signalHandlerFunc




int main(int argc, char** argv)
{
	/* terminates recv */
	signal(SIGINT, signalHandlerFunc);

	/* Initialize */
	init(shmid, msqid, sharedMemPtr);

	/* Go to the main loop */
	mainLoop();

	return 0;

} // end of main
