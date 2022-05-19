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
#include "pollLib.h"
#include "HandleNode.h"
#include "packets.h"

#include "appPDU.h"

#include "networks.h"

#define HANDLE_ARG 1 
#define SERVER_NAME_ARG 2
#define SERVER_PORT 3

#define MAXBUF 1024
#define TRUE 1
#define DEBUG_FLAG 1

void clientControl(int socketNum, HandleNode *node);
int readFromStdin(uint8_t *buffer);
void checkArgs(int argc, char * argv[]);

int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor
	int first = 0; // if first time  

	checkArgs(argc, argv);

	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[SERVER_NAME_ARG], argv[SERVER_PORT], DEBUG_FLAG);
	HandleNode *node = createHandleNode(socketNum, (uint8_t *)argv[HANDLE_ARG]);

	int packet_length = node->handle_len + 2; // acount 1 byte for the flag and another for the handle length 
	// send packet 1 to server 
	uint8_t packet[packet_length];
	create_packet_one(packet, node, packet_length); // create //packet to send to Server 
	// create_packet_one(packet, node, packet_length); 
	uint8_t recBuf[MAXBUF];   //data buffer
	memset(recBuf, 0, MAXBUF);		

	recvPDU(socketNum, recBuf, MAXBUF);


	uint8_t flag = get_flag(recBuf, 1);
	printf("Flag : %d\n", flag);
	// checks to see if flag recieved from server for handle, (tested for good and bad handle works great, header len includes null)
	if (flag == CONFIRM_GOOD_HANDLE)
		clientControl(socketNum, node); // might have to change parameters to incorporate Handle Node
	else if (flag == ERROR_INITIAL_PACKET)
		print_bad_handle_error_in_client(node);	
	
	free(node);
	close(socketNum);
	
	return 0;
}

int sendToServer(int socketNum, HandleNode *node)
{
	uint8_t sendBuf[MAXBUF];   //data buffer
	memset(sendBuf, 0, MAXBUF);
	int sendLen = 0;        //amount of data to send
	sendLen = readFromStdin(sendBuf); // read from buffer and parse for the the three commands 
	
	read_commands(sendBuf, sendLen, node);

	if (!strcmp("exit", (const char *)sendBuf ) || (sendLen == 0 ))
		return -1;

	return sendLen;
}


int recvFromServer(int socketNum, HandleNode *node)
{
	
	uint8_t recBuf[MAXBUF];
	int datalength;  //data buffer
	memset(recBuf, 0, MAXBUF);
	datalength = recvPDU(socketNum, recBuf, MAXBUF);	

	check_packet_type_client(recBuf, datalength, socketNum);
	
	if (!strcmp("exit", (const char *)recBuf ) || (datalength == 0))
		return -1;
	
	return datalength;

}

// parse the cmd line of Client for %m and others things
void clientControl(int socketNum, HandleNode *node) // change this to handle node and where we send packets 
{

	setupPollSet();
	addToPollSet(socketNum); // add server as 
	addToPollSet(STDIN_FILENO); // add stdin to POLL set

	int socketPoll ,ret;
	// create_packet_one(uint8_t *packet, HandleNode *node, int packet_length);
	while (TRUE)
	{

		printf("$: ");
		fflush(stdout);
		socketPoll = pollCall(-1); // call Poll
		
			if(socketPoll == STDIN_FILENO) // check for stdio
			{
				ret = sendToServer(socketNum, node);
				if( ret <= 0)
					break;
			}
			else 
			{
				ret = recvFromServer(socketNum, node);
				if( ret <= 0)
					break;
			}
				
	}	

	close(socketPoll);
}


// change to $ prompt for chat program
int readFromStdin(uint8_t *buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	//printf("$: ");
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
	if (argc != 4)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}
