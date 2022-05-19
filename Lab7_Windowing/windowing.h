
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

#define TRUE 1
#define FALSE 0
#define OPEN 1
#define CLOSE 0

typedef struct {
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
void add_PDU_to_Win_Buff(Window *window, uint8_t *pduBuffer, int size);
void printWindow_metadata(Window *window);
void printEntireWindow(Window *window);
void process_RR(Window *window, int RR);
int window_status(Window * window);

#endif