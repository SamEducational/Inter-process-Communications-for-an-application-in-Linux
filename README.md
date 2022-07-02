# Inter-process-Communications-for-an-application-in-Linux
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


Team members:

Sam Truong (samtruonh@csu.fullerton.edu)

Andres F. Romero Nogales

Jonathan Clyde Libut
