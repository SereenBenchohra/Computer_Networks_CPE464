
#ifndef APPPDU_H
#define APPPDU_H

int recvPDU(int clientSocket, uint8_t * dataBuffer, int bufferLen);
int sendPDU(int socketNumber, uint8_t * dataBuffer, int lengthOfData);

#endif