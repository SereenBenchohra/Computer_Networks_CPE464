#include "pdu.h"

int createPDU(uint8_t *pduBuffer, uint32_t sequenceNumber, uint8_t flag, uint8_t *payload, int payloadLen)
{
   int pduLength = 0;
   uint16_t chksum = 0;

   // store 4 -byte  sequence in Network order prior to putting the number in the PDU
   int netSeqNum = htonl(sequenceNumber);
   memcpy(pduBuffer, &netSeqNum, sizeof(uint32_t));

   memcpy(&pduBuffer[4], &chksum, sizeof(uint16_t));

   memcpy(&pduBuffer[6], &flag, sizeof(uint8_t));

   memcpy(&pduBuffer[7], payload, payloadLen);

   pduLength = payloadLen + 4 + 2 + 1;

   chksum = in_cksum((unsigned short int *)pduBuffer, pduLength); // get real chksum (according to hadi )

   memcpy(&pduBuffer[4], &chksum, sizeof(uint16_t));

   return pduLength;
}

void outputPDU(uint8_t *aPDU, int PDULength)
{
  uint32_t seq_num;
  uint8_t flag; 
  // uint16_t checksum;
  int payLoadLen = PDULength - 7; 
  uint8_t payLoad[payLoadLen];

  memcpy(&seq_num, aPDU, sizeof(uint32_t));
  // memcpy(&checksum, &aPDU[4], sizeof(uint16_t));
  memcpy(&flag, &aPDU[6], sizeof(uint8_t));
  memcpy(payLoad, &aPDU[7], payLoadLen);

  uint16_t checksum = in_cksum((unsigned short int *)aPDU, PDULength);

  printf("Sequence Number: %u\n Flag: %u\nChecksum: %u\nPayload %s\n", seq_num, flag, checksum, payLoad);
  
}