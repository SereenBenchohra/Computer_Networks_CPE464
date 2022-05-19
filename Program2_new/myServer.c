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
#include "HandleNode.h"


#include "networks.h"
#include "packets.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1
#define TRUE 1

void recvFromClient(int clientSocket);
void serverControl(int serverSocket, HandleNode **list);

int checkArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	int serverSocket = 0;   //socket descriptor for the server socket
	int portNumber = 0;
	
	portNumber = checkArgs(argc, argv);

	int packet_Length; 
	//create the server socket
	serverSocket = tcpServerSetup(portNumber);
	HandleNode *list = NULL; // create a empty handle node list to deal with
	// check for packet
/* 	int clientSocket = tcpAccept(serverSocket, DEBUG_FLAG); //socket descriptor for the client socket

	uint8_t buf[MAXBUF];
	int messageLen = recvPDU(clientSocket, buf, MAXBUF); // recieve message
	check_packet_type(buf, messageLen, clientSocket, &list); */
	serverControl(serverSocket, &list); // handles the poll and Client management
	
	/* close the sockets */
	close(serverSocket); // close server socket when done

	return 0;
}

void addNewClient(int serverSocket, HandleNode **list)
{
	int clientSocket = tcpAccept(serverSocket, DEBUG_FLAG); //socket descriptor for the client socket
	addToPollSet(clientSocket);
}
int processClient(int socketNumber, HandleNode **list)
{
	uint8_t buf[MAXBUF], sendBuf[MAXBUF]; // create buffers to get message from send and recieve PDU
	memset(buf, 0, MAXBUF);
	memset(sendBuf, 0, MAXBUF);

	// int messageLen = 0;
	int send = 0;
	int messageLen = recvPDU(socketNumber, buf, MAXBUF); // recieve message

   // recieve_packet_one(buf , messageLen, socketNumber); // don't need socket
	check_packet_type(buf, messageLen, socketNumber, list);

	//printf("Message received on socket %d , length: %d Data: %s\n", socketNumber,  messageLen, buf); // prints the socket, length, and message
	if (!strcmp("exit", (const char *)buf)) // sees if the CLient exits if  so returns 0 
		return 0;
	

	return messageLen;
}

void serverControl(int serverSocket, HandleNode **list) // sets up  polling to accept multiple clients 
{
	setupPollSet();
	addToPollSet(serverSocket); // adds main server to main poll set
//	uint8_t recBuf[MAXBUF];

	// int messageLen = 0;
	// int send = 0;
	// messageLen = recvPDU(serverSocket, recBuf, MAXBUF); // recieve message
	//uint8_t flag = get_flag(recBuf, messageLen);
	// printf("%d", flag);
	// recieve_packet_one(recBuf, messageLen,  serverSocket, list); 
	int socket, sock; 

	while (TRUE)
	{
		socket = pollCall(-1); // call Poll
		if (socket == serverSocket ) // if the socket is the server 
			addNewClient(serverSocket, list); // add a new Client
		else {
		   sock = processClient(socket, list); // otherwise process the client
			if (sock <= 0)	// if client is negative or 0 , means termination, and removes client from Poll set
				removeFromPollSet(socket);
		}

		print_list(list);
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

