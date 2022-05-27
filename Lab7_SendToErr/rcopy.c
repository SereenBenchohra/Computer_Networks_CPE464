// Client side - UDP Code				    
// By Hugh Smith	4/1/2017		

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

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "cpe464.h"

#include "pdu.h"

#define MAXBUF 80
#define PAYLOAD_MAXBUF 1400

void talkToServer(int socketNum, struct sockaddr_in6 * server);
int readFromStdin(char * buffer);
int checkArgs(int argc, char * argv[]);


int main (int argc, char *argv[])
 {
	int socketNum = 0;				
	struct sockaddr_in6 server;		// Supports 4 and 6 but requires IPv6 struct
	int portNumber = 0;
	
	portNumber = checkArgs(argc, argv);
	
	double err = 0;
	err = atof(argv[1]);

	if(err < 0 || err > 1)
	{
      fprintf(stderr, "0  < Error < 1\n");
      exit(EXIT_FAILURE);
   }

	sendtoErr_init(err, DROP_OFF, FLIP_ON, DEBUG_ON, RSEED_ON);

	socketNum = setupUdpClientToServer(&server, argv[2], portNumber);
	
	talkToServer(socketNum, &server);
	
	close(socketNum);

	return 0;
}

// 
void talkToServer(int socketNum, struct sockaddr_in6 * server)
{
	int serverAddrLen = sizeof(struct sockaddr_in6);
	char * ipString = NULL;
	int dataLen = 0; 
	int pduLength = 0;

	uint8_t pduBuffer[PAYLOAD_MAXBUF+8]; // prob memset to 0
	char buffer[PAYLOAD_MAXBUF + 1];
	uint32_t sequenceNum = 0;
	uint8_t flag = 3; // data packet 
	
	buffer[0] = '\0';
	while (buffer[0] != '.')
	{
		dataLen = readFromStdin(buffer);

		pduLength = createPDU(pduBuffer, sequenceNum++, flag, (uint8_t *)buffer, dataLen);

		outputPDU(pduBuffer, pduLength);
	
		safeSendto(socketNum, pduBuffer, pduLength, 0, (struct sockaddr *) server, serverAddrLen);
		
		pduLength = safeRecvfrom(socketNum, pduBuffer, (PAYLOAD_MAXBUF+ 7), 0, (struct sockaddr *) server, &serverAddrLen);
		
		outputPDU(pduBuffer, pduLength);

		ipString = ipAddressToString(server);
		printf("Server with ip: %s and port %d said it received %s\n", ipString, ntohs(server->sin6_port), buffer);
	      
	}
}

int readFromStdin(char * buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	buffer[0] = '\0';
	printf("Enter data: ");
	while (inputLen < (PAYLOAD_MAXBUF - 1) && aChar != '\n')
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

int checkArgs(int argc, char * argv[])
{

   int portNumber = 0;
	
   /* check command line arguments  */
	if (argc != 4)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
	
		portNumber = atoi(argv[3]);
		
	return portNumber;
}





