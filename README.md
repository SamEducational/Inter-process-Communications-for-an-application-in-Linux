# Inter-process-Communications-for-an-application-in-Linux (C++)
Design an application that can be used to transfer files between each other using shared memory and message passing which includes signal management. They are to be separated into two programs: sender and receiver. 

# Sender
• Sender: this program shall implement the process that sends files to the receiver process.
It shall perform the following sequence of steps:
1. The sender shall be invoked as ./sender file.txt where sender is the name of the executable and file.txt is the name of the file to transfer.
2. The program shall then attach to the shared memory segment, and connect to the message queue both previously set up by the receiver.
3. Read a predefined number of bytes from the specified file, and store these bytes in the chunk of shared memory.
4. Send a message to the receiver (using a message queue). The message shall contain a field called size indicating how many bytes were read from the file.
5. Wait on the message queue to receive a message from the receiver confirming successful reception and saving of data to the file by the receiver.
6. Go back to step 3. Repeat until the whole file has been read.
7. When the end of the file is reached, send a message to the receiver with the size field set to 0. This will signal to the receiver that the sender will send no more.
8. Close the file, detach shared memory, and exit.
See flow chart @ Sender Process.html file in the folder. Download and open.

# Receiver:
• Receiver: this program shall implement the process that receives files from the sender process. It shall perform the following sequence of steps:
1. The program shall be invoked as ./recv where recv is the name of the executable.
2. The program shall setup a chunk of shared memory and a message queue.
3. The program shall wait on a message queue to receive a message from the sender program. When the message is received, the message shall contain a field called size denoting the number of bytes the sender has saved in the shared memory chunk.
4. If size is not 0, then the receiver reads the size number of bytes from shared memory, saves them to the file (always called recvfile), sends a message to the sender acknowledging successful reception and saving of data, and finally goes back to step 3.
5. Otherwise, if the size field is 0, then the program closes the file, detaches the shared memory, deallocates shared memory and message queues, and exits.

When the user presses CTRL+C in order to terminate the receiver, the receiver shall deallocate memory and the message queue and then exit. This can be implemented by setting up a signal handler for the SIGINT signal. Sample file illustrating how to do this has been provided (signaldemo.cpp).

# Glossary of terms

Shared Memory - A term defining circumstances where “two or more processes read and write to a shared section of memory (Operating Systems Concepts, Tenth Edition).”

Message Passing - A term defining circumstances where “packets of information in predefined formats are moved between processes by the operating system (Operating Systems Concepts, Tenth Edition) .”

Signal - A signal is a software generated interrupt that is sent to a process by the OS because the user pressed CTRL+C or another process tells something to this process (Geeksforgeeks). 

ftok(): is use to generate a unique key. (1)

msgget(): either returns the message queue identifier for a newly created message queue or returns the identifiers for a queue which exists with the same key value. (1)

msgsnd(): Data is placed on to a message queue by calling msgsnd(). (1)

msgrcv(): messages are retrieved from a queue. (1)

msgctl(): It performs various operations on a queue. Generally it is use to destroy message queue. (1)
shmget(): int shmget(key_t,size_tsize,intshmflg); upon successful completion, shmget() returns an identifier for the shared memory segment. (2)
shmat(): Before you can use a shared memory segment, you have to attach yourself to it using shmat(). void *shmat(int shmid ,void *shmaddr ,int shmflg); shmid is a shared memory id. shmaddr specifies a specific address to use but we should set it to zero and OS will automatically choose the address. (2)
shmdt(): When you’re done with the shared memory segment, your program should detach itself from it using shmdt(). int shmdt(void *shmaddr); (2)
shmctl(): when you detach from shared memory,it is not destroyed. So, to destroy shmctl() is used. shmctl(int shmid,IPC_RMID,NULL); (2)
Notes:
Message passing is considered easier to implement than shared memory
Shared memory is faster than message passing

# References

References

https://www.geeksforgeeks.org/ipc-using-message-queues/  

https://www.geeksforgeeks.org/ipc-shared-memory/ 

Operating Systems Concepts, Tenth Edition 
https://www.geeksforgeeks.org/signals-c-language/

# Team members

Sam Truong (samtruonh@csu.fullerton.edu)

Andres F. Romero Nogales

Jonathan Clyde Libut
