// Hugh Smith april 2017
// Network code to support TCP/UDP client and server connections
// June 2018 - modified to support ipv6

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

#include "networks.h"
#include "cpe464.h"
#include "gethostbyname.h"

int safeGetUdpSocket()
{
    int socketNumber = 0;
    if((socketNumber = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
    {
        perror("safeGetUdpSocket(), socket(), call: ");
        exit(-1);
    }
    return socketNumber;
}

int safeSendto(uint8_t *packet, uint32_t len, Connection * to)
{
    int send_len = 0;
    if((send_len = sendtoErr(to->sk_num, packet, len, 0, (struct sockaddr *)&(to->remote), to->len)) < 0)
    {
        perror("in safeSendto(), sendto() call");
        exit(-1);
    }   
    return send_len;
}

int safeRecvfrom(int recv_sk_num, uint8_t * packet, int len, Connection * from)
{
    int recv_len = 0;
    from->len = sizeof(struct sockaddr_in6);

    if((recv_len = recvfrom(recv_sk_num, packet, len, 0, 
            (struct sockaddr *)&(from->remote), &from->len)) < 0)
    { 
        perror("in safeRecvfrom, recvfrom call");
        exit(-1);
    }
    return recv_len;
}

int udpServerSetup(int portNumber)
{
    struct sockaddr_in6 server;
    int socketNumber = 0;
    int serverAddrLen = 0;

    // create the socket
    socketNumber = safeGetUdpSocket();

    // set up the socket
    server.sin6_family = AF_INET6;              //internet (IPV6 OR ipv4) family
    server.sin6_addr = in6addr_any;             // use any local IP address
    server.sin6_port = htons(portNumber);       // if 0 = os picks

    // bind the name (address) to a port
    if(bind(socketNumber, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind() call error");
        exit(-1);
    }

    // Get the port number
    serverAddrLen = sizeof(server);
    getsockname(socketNumber, (struct sockaddr *)&server, (socklen_t *)&serverAddrLen);
    printf("Server using Port #: %d\n", ntohs(server.sin6_port));

    return socketNumber;
}

int udpClientSetup(char * hostName, int portNumber, Connection *connection)
{
    memset(&connection->remote, 0, sizeof(struct sockaddr_in6));
    connection->sk_num = 0;
    connection->len = sizeof(struct sockaddr_in6);
    connection->remote.sin6_family = AF_INET6;
    connection->remote.sin6_port = htons(portNumber);

    // create the socket
    connection->sk_num = safeGetUdpSocket();

    if(gethostbyname6(hostName, &connection->remote) == NULL)
    {
        printf("Host not found: %s\n", hostName);
        return -1;
    }
    printf("Server info - ");
    printIPv6Info(&connection->remote);

    return 0;
}


int select_call(int32_t socket_num, int32_t seconds, int32_t microseconds)
{
    // not the greatest function since it only works on 1-socket (one file descriptor)
    // but that is all that is needed for this program.
    fd_set fdvar;
    struct timeval aTimeout;
    struct timeval * timeout = NULL;

    // if either time is -1 then wait forever (block)
    // since timeout defauts to pointint to null
    if(seconds != -1 && microseconds != -1)
    {
        aTimeout.tv_sec = seconds;
        aTimeout.tv_usec = microseconds;
        timeout = &aTimeout;
    }

    FD_ZERO(&fdvar);                // reset variable
    FD_SET(socket_num, &fdvar);     //

    if(select(socket_num+1, (fd_set *)&fdvar, NULL, NULL, timeout) < 0)
    {
        perror("Select");
        exit(-1);
    }

    if(FD_ISSET(socket_num, &fdvar))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void printIPv6Info(struct sockaddr_in6 * client)
{
    // nameOfAddress is just used to print out "Client" or "Server" or whatever you want on your output
    char ipString[INET6_ADDRSTRLEN];

    inet_ntop(AF_INET6, &client->sin6_addr, ipString, sizeof(ipString));
    printf("IP: %s Port: %d \n", ipString, ntohs(client->sin6_port));
}
