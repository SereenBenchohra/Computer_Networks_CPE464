#ifndef PDU_H
#define PDU_H

#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include "checksum.h"
#include <stdio.h>

int createPDU(uint8_t *pduBuffer, uint32_t sequenceNumber, uint8_t flag, uint8_t *payload, int payloadLen);
void outputPDU(uint8_t *aPDU, int PDULength);

#endif