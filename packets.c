#include <stdlib.h>
#include "HandleNode.h"
#include "packets.h"
#include <stdio.h>
#include <string.h>
#include "appPDU.h"
#include "HandleNode.h"


void message_packets()
{
   printf("In message Packet\n");
}

void list_packets()
{
   printf("In List Packet\n");
}

void broadcast_packets()
{
   printf("In Broadcast Packet\n");
}

void exit_packets()
{
   printf("In Exit Packet\n");
}

/* void check_command(unsigned char *string)
{
   if (!strcmp(string, "%M") || !strcmp(string, "%m"))
      message_packets();
   if (!strcmp(string, "%B") || !strcmp(string, "%b"))
      broadcast_packets();
   if (!strcmp(string, "%L") || !strcmp(string, "%l"))
      list_packets();
   if (!strcmp(string, "%E") || !strcmp(string, "%e"))
      exit_packets();

} */
// change to return type to void and pass in a list that way there is no overwrite
/* void split(unsigned char *string, char *delim)
{
   unsigned char *substring, *local;
   local = string; // have local copy to mutate and not affect original string

   substring = (unsigned char *)strsep(local, delim); // gets first element in string
   // check for commands 
   check_command(substring);
   //printf("%s\n", substring);
     

} */

// generates and sends packet 1 to the server 
void create_packet_one(uint8_t *packet, HandleNode *node, int packet_length)
{
   
   // uint16_t PDU_len = node->handle_len + 3 ; // account for chat header
   
   uint8_t handle_len = node->handle_len;
//   int sent = 0;
   uint8_t flag = INITIAL_PACKET;
   //copy PDU len in buffer (DONT send PDU does this)s
   // copy flag in buffer
   memcpy(packet, &flag, 1);

   // copy handle len into buffer
   memcpy(&packet[1], &handle_len, 1);
   // copy handle name into buffer
   memcpy(&packet[2], node->handle, handle_len);

   sendPDU(node->socketNum, packet, packet_length);

  //  printf("%d\n", sent); prints proper value

}

void print_bad_handle_error_in_client(HandleNode *node)
{
   if (node->handle_len > HEADER_LEN_MAX)
      fprintf(stderr, "Invalid handle, handle longer than 100 characters: %s\n", node->handle);
   else if(node->handle[0] >= '0' && node->handle[0] <= '9')
      fprintf(stderr, "Invalid handle, handle starts with a number\n");
   else
      fprintf(stderr, "Handle already in use: %s\n", node->handle);

   
   free(node);

   exit(EXIT_FAILURE);
}
// checks if the handle sent from client (packet 1 is proper ) if handle is improper send flag 3 , otherwise send flag 2 
uint8_t proper_handle(uint8_t *handle, uint8_t handle_len, HandleNode **list)
{

   uint8_t flag = CONFIRM_GOOD_HANDLE; // set default to good handle 
   HandleNode *node;

   // check if it exists in list , if it does then flag is three
   node = getHandleNodeByHandle(handle, list);
   printf("Handle first char %c\n", handle[0]);
   printf("Header Len %d\n", handle_len);
   printf("Node value %p\n", node );
   if ((node != NULL) || (handle_len > HEADER_LEN_MAX) || (handle[0] >= '0' && handle[0] <= '9')) //print proper message later
      flag = ERROR_INITIAL_PACKET;

   printf("Flag from Server value : %d\n", flag);
   return flag;
}

// if the first flag, gets the handle and checks if handle is proper then send appropiate flag back 
void recieve_packet_one(uint8_t *packet, int datalength, int socketNum, HandleNode **list) 
{
   uint8_t handle_len;
   uint8_t flag = 0;
   // memcpy(&flag, packet, 1);
   memcpy(&handle_len, &packet[1], 1);
   //int length = datalength - 2;
   uint8_t handle[handle_len];
   memcpy(handle, &packet[2], handle_len);
   // check if handle is proper or not 
   flag = proper_handle(handle, handle_len, list);

   // if handle is good handle , add it to linked list 
   if (flag == CONFIRM_GOOD_HANDLE)
   {
      addToHandleNodeList(socketNum, handle, list);
      sendPDU(socketNum, &flag, 1); // send flag back
   }
   else
      sendPDU(socketNum, &flag, 1); // send flag back


   // int sendPDU(int socketNumber, uint8_t * dataBuffer, int lengthOfData)

}


uint8_t get_flag(uint8_t *packet, int dataLength)
{
   uint8_t flag = 0; // initialize it to zero so we'll know if it read the flag
   memcpy(&flag, packet, 1 ); // gets flag data
   return flag;
}
// checks for the flag and recieve 
void check_packet_type(uint8_t *packet, int dataLength, int socketNum, HandleNode **list)
{
   uint8_t flag = get_flag(packet, dataLength);
   switch (flag)
   {
      case INITIAL_PACKET:
         recieve_packet_one(packet, dataLength, socketNum, list);

      break;

   
   default:
      break;
   }
}

