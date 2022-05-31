/* Server stop and wait code Writen: Hugh Smith */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "networks.h"
#include "srej.h"
#include "cpe464.h"
#include "windowing.h"

typedef enum State STATE;

enum State
{
    START, DONE, FILENAME, SEND_DATA, WAIT_ON_ACK,
     TIMEOUT_ON_ACK, WAIT_ON_EOF_ACK, TIMEOUT_ON_EOF_ACK
};

void process_server(int serverSocketNumber);
void process_client(int32_t serverSocketNumber, uint8_t *buf, int32_t recv_len, Connection * client);
STATE filename(Connection *client, uint8_t *buf, int32_t recv_len, int32_t *data_file, 
        int32_t *buf_size, Window *window);
STATE send_data(Connection *client, uint8_t *packet, int32_t *packet_len, int32_t data_file, 
        int32_t buf_size, uint32_t *seq_num, Window *window);
STATE timeout_on_ack(Connection *client, uint8_t *packet, int32_t packet_len);
STATE timeout_on_eof_ack(Connection *client, uint8_t *packet, int32_t packet_len);
STATE wait_on_ack(Connection *client, Window *window);
STATE wait_on_eof_ack(Connection *client);
int processArgs(int argc, char **argv);
void handleZombies(int sig);

int main(int argc, char *argv[])
{
    int32_t serverSocketNumber = 0;
    int portNumber =  0;
    portNumber = processArgs(argc, argv);
    sendtoErr_init(atof(argv[1]), DROP_ON, FLIP_ON, DEBUG_ON, RSEED_ON);

    /* set up the mains server port */
    serverSocketNumber = udpServerSetup(portNumber);

    process_server(serverSocketNumber);
    return 0;
}

void process_server(int serverSocketNumber)
{
    pid_t pid = 0;
    uint8_t buf[MAX_LEN];
    Connection *client = (Connection *) calloc(1, sizeof(Connection));
    uint8_t flag = 0;
    uint32_t seq_num = 0;
    int32_t recv_len =0;

    // We are going to fork() so need to clean up (SIGCHLD)
    signal(SIGCHLD, handleZombies);

    // get new client Connection, fork() child,
    while(1) // add select call here ? 
    {
        // block waiting for a new client
        recv_len = recv_buf(buf, MAX_LEN, serverSocketNumber, client, &flag, &seq_num);

        if(recv_len != CRC_ERROR)
        {
            if((pid = fork()) < 0)
            {
                perror("fork");
                exit(-1);
            }

            if(pid == 0)
            {
                // child process - a new process for each client
                printf("Child fork() - child-pid: %d\n", getpid());
                process_client(serverSocketNumber, buf, recv_len, client);
                exit(0);
            }
        }
    }
}

void process_client(int32_t serverSocketNumber, uint8_t *buf, int32_t recv_len, Connection *client)
{
    STATE state = START;
    int32_t data_file = 0;
    int32_t packet_len = 0;
    uint8_t packet[MAX_LEN];
    int32_t buf_size = 0;
    uint32_t seq_num = START_SEQ_NUM;
    Window *window  = NULL; // can't initialize until I retrieve the W size from Rcopy

    while(state != DONE)
    {
        switch(state)
        {
            case START:
                state =  FILENAME;
                break;
            case FILENAME:
                state = filename(client, buf, recv_len, &data_file, &buf_size, window);
                break;
            case SEND_DATA:
                state = send_data(client, packet, &packet_len, data_file, buf_size, &seq_num, window);
                break;
            case WAIT_ON_ACK:
                state = wait_on_ack(client);
                break;
            case TIMEOUT_ON_ACK:
                state = timeout_on_ack(client, packet, packet_len);
                break;
            case WAIT_ON_EOF_ACK:
                state = wait_on_eof_ack(client);
                break;
            case TIMEOUT_ON_EOF_ACK:
                state = timeout_on_eof_ack(client, packet, packet_len);
                break;
            case DONE:
                break;
            default:
                printf("In default and you should not be here!!!!\n");
                state = DONE;
                break;
        }
    }
    
}

STATE filename(Connection *client, uint8_t *buf, int32_t recv_len,
         int32_t *data_file, int32_t *buf_size, Window *window)
{
    uint8_t response[1];
    char fname[MAX_LEN];
    int32_t window_size = 0; // keep it consistent type with buf size
    STATE returnValue = DONE;
    int fileNameLen = recv_len - SIZE_OF_BUF_SIZE;

    // extract buffer sized used for sending data, window size and and also filename
    memcpy(buf_size, buf, SIZE_OF_BUF_SIZE);
    *buf_size = ntohl(*buf_size);
    memcpy(fname, &buf[sizeof(*buf_size)], fileNameLen);
    memcpy(&window_size, &buf_size[sizeof(*buf_size) + fileNameLen], sizeof(window_size));

    // Create client socket to allow for processing this particular client
    client->sk_num = safeGetUdpSocket();

    if(((*data_file) = open(fname, O_RDONLY)) < 0)
    {
        send_buf(response, 0, client, FNAME_BAD, 0, buf);
        returnValue = DONE;
    }
    else // create Window here 
    {
        window = create_Window(window_size);
        send_buf(response, 0, client, FNAME_OK, 0, buf);
        returnValue = SEND_DATA;
    }

    return returnValue;
}

STATE send_data(Connection *client, uint8_t *packet, int32_t *packet_len, 
        int32_t data_file, int buf_size, uint32_t *seq_num, Window *window)
{
    uint8_t buf[MAX_LEN];
    int32_t len_read = 0;
    STATE returnValue = DONE;

    len_read = read(data_file, buf, buf_size);

    switch(len_read)
    {
        case -1:
            perror("send_data, read error");
            returnValue = DONE;
            break;
        case 0:
            (*packet_len) = send_buf(buf, 1, client, END_OF_FILE, *seq_num, packet);
            returnValue = WAIT_ON_EOF_ACK;
            break;
        default: // add PDU to Window here 
            (*packet_len) = send_buf(buf, len_read, client, DATA, *seq_num, packet);
            addPDUtoWindow(window,buf, len_read,*seq_num);
            (*seq_num)++;
            returnValue = WAIT_ON_ACK;
            break;

    }
    return returnValue;
}
// modify to have RR and SREJ 
STATE wait_on_ack(Connection * client, Window *window)
{
    STATE returnValue = DONE;
    uint32_t crc_check = 0;
    uint8_t buf[MAX_LEN];
    uint8_t packet[MAX_LEN];

    int32_t len = MAX_LEN;
    uint8_t flag = 0;
    uint32_t seq_num = 0;
    static int retryCount = 0;

    if((returnValue = processSelect(client, &retryCount, TIMEOUT_ON_ACK, SEND_DATA,
         DONE)) == SEND_DATA)
    {
        crc_check = recv_buf(buf, len, client->sk_num, client, &flag, &seq_num);

        // if crc error ignore packet
        if(crc_check == CRC_ERROR)
        {
            returnValue = WAIT_ON_ACK;
        }
        else if (flag == RR) // if recieved an RR updated Sliding Window with process RR 
            process_RR(window, seq_num);
        else if(flag == SREJ) // if SREJ is recieved , resend the data 
        {
			send_buf(buf, len, client, DATA, seq_num, packet); // might have to utilize windowing here 
        }
        
        else if(flag != RR && flag != SREJ)
        {
            printf("I wait_on_ack but its not an ACK flag (this should never happen) is: %d\n", flag);
            returnValue = DONE;
        }
    }
    return returnValue;
}

STATE wait_on_eof_ack(Connection *client)
{
    STATE returnValue = DONE;
    uint32_t crc_check = 0;
    uint8_t buf[MAX_LEN];
    int32_t len = MAX_LEN;
    uint8_t flag = 0;
    uint32_t seq_num = 0;
    static int retryCount = 0;

    if((returnValue = processSelect(client, &retryCount, TIMEOUT_ON_EOF_ACK, DONE, DONE)) == DONE)
    {
        crc_check = recv_buf(buf, len, client->sk_num, client, &flag, &seq_num);

        // if crc error ignore packet
        if(crc_check == CRC_ERROR)
        {
            returnValue = WAIT_ON_EOF_ACK;
        }
        else if(flag != EOF_ACK)
        {
            printf("In wait_on_eof_ack but its not an EOF_ACK flag (this should never happen) is: %d\n", flag);
            returnValue = DONE;
        }
        else
        {
            printf("File tranfer complete successfully. \n");
            returnValue = DONE;
        }
    }
    return returnValue;
}
// Here is where we send a SREJ probably
STATE timeout_on_ack(Connection *client, uint8_t *packet, int32_t packet_len)
{
    safeSendto(packet, packet_len, client);
    return WAIT_ON_ACK;
}
// also 
STATE timeout_on_eof_ack(Connection *client, uint8_t *packet, int32_t packet_len)
{
    safeSendto(packet, packet_len, client);
    return WAIT_ON_EOF_ACK;
}

int processArgs(int argc, char **argv)
{
    int portNumber = 0;
    if(argc < 2 || argc > 3)
    {
        printf("Usage %s error_rate[port number]\n", argv[0]);
        exit(-1);
    }
    if(argc == 3)
    {
        portNumber = atoi(argv[2]);
    }
    else
    {
        portNumber = 0;
    }
    return portNumber;
}

// SIGCHLD handler - clean up terminated processes
void handleZombies(int sig)
{
    int stat = 0;
    while(waitpid(-1, &stat, WNOHANG) > 0)
   {}
}