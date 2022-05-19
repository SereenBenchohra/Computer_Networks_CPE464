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
#include "appPDU.h"


#include "networks.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1
#define TRUE 1

void recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	int serverSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;
	
	portNumber = checkArgs(argc, argv);
	
	//create the server socket
	serverSocket = tcpServerSetup(portNumber);

	// wait for client to connect

	// loop until recv is 0
	
	while (TRUE)
	{
		/* code */
		clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
		recvFromClient(clientSocket);
		close(clientSocket);

	}
	

	
	/* close the sockets */
	close(serverSocket);

	return 0;
}

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

