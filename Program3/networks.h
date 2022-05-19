
// 	Writen - HMS April 2017
//  Supports TCP and UDP - both client and server


#ifndef __NETWORKS_H__
#define __NETWORKS_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BACKLOG 10

// for the server side
int tcpServerSetup(int serverPort);
int tcpAccept(int mainServerSocket, int debugFlag);
int udpServerSetup(int serverPort);

// for the client side
int tcpClientSetup(char * serverName, char * serverPort, int debugFlag);
int setupUdpClientToServer(struct sockaddr_in6 *serverAddress, char * hostName, int serverPort);


#endif
