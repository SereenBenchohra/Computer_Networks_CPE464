#ifndef PDU_H
#define PDU_H

#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include "checksum.h"
#include <stdio.h>

#define SETUP_PACKET_FLAG 1
#define SETUP_RESPONSE_PACKET_FLAG 2
#define DATA_PACKET_FLAG 3
#define RR_PACKET_FLAG 5
#define SREJ_PACKET_FLAG 6
#define FILE_CONTENTS_FLAG 7
#define FILENAME_RESPONSE_FLAG 8

int createPDU(uint8_t *pduBuffer, uint32_t sequenceNumber, uint8_t flag, uint8_t *payload, int payloadLen);
void outputPDU(uint8_t *aPDU, int PDULength);

#endif