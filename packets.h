#ifndef PACKETS_H
#define PACKETS_H

#include <stdint.h>
#include "HandleNode.h"

// void split(unsigned char *string, char *delim);


#define CHAT_HEADER_LEN 3
#define HEADER_LEN_MAX 100

#define INITIAL_PACKET 1
#define CONFIRM_GOOD_HANDLE 2

#define ERROR_INITIAL_PACKET 3

#define BROADCAST_PACKET 4

#define MESSAGE_PACKET 5
#define ERROR_MESSAGE_PACKET 7
#define CLIENT_EXIT_PACKET 8

#define SERVER_EXIT_PACKET 9
#define CLIENT_LIST_PACKET 10
#define SERVER_LIST_NUM_PACKET 11

#define SERVER_LIST_NAME_PACKET 12
#define SERVER_LIST_DONE_PACKET 13

uint8_t get_flag(uint8_t *packet, int dataLength);
void create_packet_one(uint8_t *packet, HandleNode *node, int packet_length);
void check_packet_type(uint8_t *packet, int dataLength, int socketNum, HandleNode **list);
void recieve_packet_one(uint8_t *packet, int datalength, int socketNum, HandleNode **list);
void print_bad_handle_error_in_client(HandleNode *node);



#endif