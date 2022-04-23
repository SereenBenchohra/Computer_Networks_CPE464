/******************************************************************************
* myClient.c
*
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
#define TRUE 1
#define DEBUG_FLAG 1

void sendToServer(int socketNum);
int readFromStdin(uint8_t *buffer);
void checkArgs(int argc, char * argv[]);

int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor
	
	checkArgs(argc, argv);

	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[1], argv[2], DEBUG_FLAG);
	
	sendToServer(socketNum);
	
	close(socketNum);
	
	return 0;
}

void sendToServer(int socketNum)
{
	while (TRUE)
	{
		uint8_t sendBuf[MAXBUF];   //data buffer
		uint8_t recBuf[MAXBUF];   //data buffer

		int sendLen = 0;        //amount of data to send
		int sent = 0;
		int rec = 0;            //actual amount of data sent/* get the data and send it   */
	
	
		sendLen = readFromStdin(sendBuf);
		printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);
	
		sent =  sendPDU(socketNum, sendBuf, sendLen); // sends to server message

		if (!strcmp("exit", (const char *)sendBuf ) || (sent == 0)) // breaks from Client if ^C or exit is inputted 
			break;

		printf("Amount of data sent is: %d\n", sent);

		rec =  recvPDU(socketNum, recBuf, sendLen); // recieves message from Client

		printf("Message received on socket %d , length: %d Data: %s\n", socketNum, rec, sendBuf);

	}
	


}

int readFromStdin(uint8_t *buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	printf("Enter data: ");
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}
	
	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;
	
	return inputLen;
}

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 3)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}
