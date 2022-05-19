/******************************************************************************
* tcp_server.c
*
* CPE 464 - Program 1
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "pollLib.h"
#include "appPDU.h"


#include "networks.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1
#define TRUE 1

void recvFromClient(int clientSocket);
void serverControl(int serverSocket);

int checkArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	int serverSocket = 0;   //socket descriptor for the server socket
	int portNumber = 0;
	
	portNumber = checkArgs(argc, argv);
	
	//create the server socket
	serverSocket = tcpServerSetup(portNumber);
	serverControl(serverSocket); // handles the poll and Client management
	
	/* close the sockets */
	close(serverSocket); // close server socket when done

	return 0;
}

void addNewClient(int serverSocket)
{
	int clientSocket = tcpAccept(serverSocket, DEBUG_FLAG); //socket descriptor for the client socket
	addToPollSet(clientSocket);
}

int processClient(int socketNumber)
{
	uint8_t buf[MAXBUF], sendBuf[MAXBUF]; // create buffers to get message from send and recieve PDU

	int messageLen = 0;
	int send = 0;
	messageLen = recvPDU(socketNumber, buf, MAXBUF); // recieve message
	printf("Message received on socket %d , length: %d Data: %s\n", socketNumber,  messageLen, buf); // prints the socket, length, and message
	if (!strcmp("exit", (const char *)buf)) // sees if the CLient exits if  so returns 0 
		return 0;
	
	send = sendPDU(socketNumber, sendBuf, messageLen); // sends the message back to Client
	printf("Amount of data sent is: %d\n", send);

	return messageLen;
}

void serverControl(int serverSocket) // sets up  polling to accept multiple clients 
{
	setupPollSet();
	addToPollSet(serverSocket); // adds main server to main poll set
	int socket, sock; 

	while (TRUE)
	{
		socket = pollCall(-1); // call Poll
		if (socket == serverSocket ) // if the socket is the server 
			addNewClient(serverSocket); // add a new Client
		else {
		   sock = processClient(socket); // otherwise process the client
			if (sock <= 0)	// if client is negative or 0 , means termination, and removes client from Poll set
				removeFromPollSet(socket);
		}
			
	}
	
}
// not used for this part 
void recvFromClient(int clientSocket)
{

		while (TRUE)
		{
			uint8_t buf[MAXBUF];
			uint8_t sendBuf[MAXBUF];

			int messageLen = 0;
			int send = 0;
	
			//now get the data from the client_socket
			messageLen = recvPDU(clientSocket, buf, MAXBUF);

			if (!strcmp("exit", (const char *)buf ) || (messageLen == 0))
				break;
			printf("Message received on socket %d , length: %d Data: %s\n", clientSocket,  messageLen, buf);

			send = sendPDU(clientSocket, sendBuf, messageLen);
		
			printf("Amount of data sent is: %d\n", send);


		}
	
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}
	
	return portNumber;
}

