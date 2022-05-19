#include <stdlib.h>
#include "HandleNode.h"
#include "packets.h"
#include <stdio.h>
#include <string.h>
#include "appPDU.h"
#include "HandleNode.h"

#define TRUE 1
#define FALSE 0
#define MAXBUF 1024


//checks if Directory is a numeric process id by checking if its an integer or not, if it is it returns TRUE

// ensure correct format is sent in command line otherwise raise error
// create message packet to send to server. 
// Parse buffer for info needed for message packet
// check if valid handles and arguments 
// if so create packet buffer to send to server 

int getIndexforMessage(uint8_t *buffer, uint8_t buffer_len, int numHandles)
{
   int i = 0; 
   while (numHandles > 0)
   {
      if (buffer[i] == ' ')
         numHandles--;
      i++;
   }
   
   return i;
}

int message_packets(uint8_t *buffer, int bufferLen, HandleNode *node)
{
   uint8_t flag = MESSAGE_PACKET; // create the message packet flag
   int packet_len = 0;
   int sendLen;
   uint8_t sender_handle_len = node->handle_len;
   
   // writes initial contents into packet buffer 
   uint8_t packet[MAXBUF]; // set to max BUf since size is not known of packet size
   memcpy(packet, &flag, 1); // copies flag onto buffer
   memcpy(&packet[1], &sender_handle_len, 1); // copies sender length onto the buffer
   memcpy(&packet[2], node->handle, sender_handle_len); //copies sender handle name onto buffer */
   
   packet_len+= (2 + sender_handle_len);
   // retrieves command line data so eventually to write into packet buffer
   uint8_t numHandles_char; 
   uint8_t numHandles;
   memcpy(&numHandles_char, &buffer[3], 1); // copies num handles at position 3
   numHandles = numHandles_char - '0'; // converts it to int type 

   // copies command buffer from recv handle name (which starts at index 5 no matter what with proper input). Copying works 
   uint8_t handleBufLen = bufferLen - 5;
   uint8_t handlesBuf[handleBufLen];
   memcpy(handlesBuf, &buffer[5], handleBufLen); // works 

   // make a copy because of strtok destroys original string
   uint8_t handleBuf_copy[handleBufLen];
   memcpy(handleBuf_copy, handlesBuf, handleBufLen); 

   char *token, *rest;
   uint8_t len, index, i;
  //  num = numHandles;
   i = 0;
   rest = (char*)handleBuf_copy;
   index = sender_handle_len + 2; 
   memcpy(&packet[index], &numHandles, 1);
   packet_len++;
   index +=1; // increment index by one

   while ((token = strtok_r(rest, " ", &rest)) && (i< numHandles))
   {
      
      len = strlen(token); 
      memcpy(&packet[index], &len, 1); // copies receive handle length into buffer
      index++;
      memcpy(&packet[index], token, len); // copies handle name into buffer
      index+=len;
      packet_len += (1 + len);
     
      i++;
   }
   
   // get the index to for the beginning of the message
   int txt_index = getIndexforMessage(handlesBuf, handleBufLen, numHandles);
   
   // writes txt into buffer
   memcpy(&packet[index], &handlesBuf[txt_index], (handleBufLen - txt_index));

   packet_len += (handleBufLen - txt_index);

   sendLen = sendPDU(node->socketNum, packet, packet_len);
   
   return sendLen;
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


// parse the command line for commands %M, %B, %L, and %E
int read_commands(uint8_t *buffer, int bufferLen, HandleNode *node)
{
   // get first two characters of command line to see if one of the commands 
   int sendLen = 1;
   uint8_t parse_command[3];

   memcpy(parse_command, buffer, 2); // copies first two bytes to get command
   
   parse_command[2] = '\0';
   const char *command = (const char *)parse_command; 

   if (!strcmp(command, "%M") || !strcmp(command, "%m"))
      sendLen = message_packets(buffer, bufferLen, node);
   if (!strcmp(command, "%B") || !strcmp(command, "%b"))
      broadcast_packets();
   if (!strcmp(command, "%L") || !strcmp(command, "%l"))
      list_packets();
   if (!strcmp(command, "%E") || !strcmp(command, "%e"))
      exit_packets();
      
   return sendLen;
}

// generates and sends packet 1 to the server 
void create_packet_one(uint8_t *packet, HandleNode *node, int packet_length)
{
      
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

   node = getHandleNodeByHandle(handle, list); // check if it exists in list , if it does then flag is three

   if ((node != NULL) || (handle_len > HEADER_LEN_MAX) || (handle[0] >= '0' && handle[0] <= '9')) //print proper message later
      flag = ERROR_INITIAL_PACKET;

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

}

// first thing make sure the contents of the packet are right
// if they are route to other clients (check if linked list works)
// 
void recieve_packet_five(uint8_t *packet, int dataLength, int socketNum, HandleNode **list)
{
   uint8_t numHandles, sender_handle_len; 
   memcpy(&sender_handle_len, &packet[1], 1);
   uint8_t sender_handle[sender_handle_len];
   memcpy(sender_handle, &packet[2], sender_handle_len);
   // both work excellent
   printf("Sender Handle: %s\n", sender_handle); // 
   printf("Sender Handle Len: %d\n", sender_handle_len);
   memcpy(&numHandles, &packet[2 + sender_handle_len], 1); // get num Handles 
   int index = 3 + sender_handle_len;
   printf("Num Handles %d\n", numHandles); // works as well
   
   int num = numHandles;
   uint8_t len;
   while (num > 0) // gets segfault when num handles is 5 
   {
      memcpy(&len, &packet[index], 1);
      index++;
      printf("Recv Len: %d\n", len);
      
      uint8_t recv[len];

      memcpy(recv, &packet[index], len);
      recv[len] = '\0';
      printf("Recv Handle: %s\n", recv);
      index += len;
      memset(recv, 0, len); // clears buffer
      num--;
   }
   
   int msg_len = dataLength - index;
   uint8_t msg[msg_len];
   memcpy(msg, &packet[index], msg_len);

   printf("Msg:[%s]\n", msg); 
}

uint8_t get_flag(uint8_t *packet, int dataLength)
{
   uint8_t flag = 0; // initialize it to zero so we'll know if it read the flag
   memcpy(&flag, packet, 1 ); // gets flag data
   return flag;
}
// checks for the flag and recieve  on server side (maybe client)
void check_packet_type(uint8_t *packet, int dataLength, int socketNum, HandleNode **list)
{
   uint8_t flag = get_flag(packet, dataLength);
   uint8_t start = FALSE;
   switch (flag)
   {
     // case INITIAL_PACKET:
       // recieve_packet_one(packet, dataLength, socketNum, list);
     // break;

      case MESSAGE_PACKET:
         recieve_packet_five(packet, dataLength, socketNum, list);
      break;

   
   default:
      break;
   }
}

// checks for the flag and recieve  on client side 
void check_packet_type_client(uint8_t *packet, int dataLength, int socketNum)
{
   uint8_t flag = get_flag(packet, dataLength);
   switch (flag)
   {

      case MESSAGE_PACKET:
        printf("In message Packet\n");
        // recieve_packet_five();
      break;

   
   default:
      break;
   }
}