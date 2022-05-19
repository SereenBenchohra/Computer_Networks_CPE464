#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "appPDU.h"

// first two bytes are the PDU_len
int recvPDU(int clientSocket, uint8_t * dataBuffer, int bufferLen)
{
   uint16_t PDU_header_len, actualBufferLen;
   uint8_t nbytes =  recv(clientSocket, dataBuffer , 2, MSG_WAITALL); // get the header len for the next recv

   if (nbytes < 0 ) // if recv < -1 raise error
   {
      perror("Recv Fail");
      exit(EXIT_FAILURE);
   }

   if (nbytes == 0)
      return 0;
   
   memcpy(&PDU_header_len, dataBuffer, 2); // copies header length from Data Buffer 
   actualBufferLen = ntohs(PDU_header_len) - 2; // set it to host byte order and get buffer len

   nbytes = recv(clientSocket, dataBuffer , actualBufferLen , MSG_WAITALL);

   if (nbytes < 0 ) // if recv < -1 raise error
   {
      perror("Recv Fail");
      exit(EXIT_FAILURE);
   }

   if (nbytes > PDU_header_len )
   {
      perror("Buffer Not Big Enough for PDU len");
      exit(EXIT_FAILURE);

   }

   return actualBufferLen; // number of bytes received in the second recv())
}

int sendPDU(int socketNumber, uint8_t * dataBuffer, int lengthOfData)
{
   
  uint16_t PDU_header_len = lengthOfData + 2; // have the size of PDU be 2 bytes 
  uint8_t PDU_buffer[PDU_header_len];   // create a PDU buffer accounting for PDU header len  

   PDU_header_len = htons(PDU_header_len);
   
   memcpy(PDU_buffer, &PDU_header_len, 2); // write PDU header to the buffer
   memcpy(&PDU_buffer[2], dataBuffer, lengthOfData); // writes to the PDU buffer after the PDU header 

  uint8_t nbytes = send(socketNumber, PDU_buffer, lengthOfData + 2, MSG_WAITALL);

   if (nbytes  < 0 )
   {
     perror("Send");
     exit(EXIT_FAILURE); 
   }

   if (nbytes == 0)
      return 0;

  return lengthOfData;
  
}