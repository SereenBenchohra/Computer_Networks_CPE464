
#ifndef WINDOWING_H
#define WINDOWING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>


#define TRUE  1
#define FALSE 0

typedef struct{
	uint32_t seqNum;
   uint8_t *pdu;
   int index, isValid, length;
 
} PDU_Data;


typedef struct 
{
   int upper, lower, current, windowsize;

   PDU_Data **pduArray;    

} Window;


Window *create_Window(int windowsize );

void printWindow_metadata(Window *window);


void printEntireWindow(Window *window);

int isWindowEmpty(Window *window);


void addPDUtoWindow(Window *window, uint8_t *pduBuffer, int size, uint32_t seqNum);


void process_RR(Window *window, int RR);

PDU_Data *findPDU(Window * window, uint32_t seqNum);


void remove_PDU_data(Window * window, uint32_t seqNum); // remove data from the queue once finished 



int isOpen(Window * window);

#endif