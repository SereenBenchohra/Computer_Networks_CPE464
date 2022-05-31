#include "windowing.h"


Window *create_Window(int windowsize )
{
     
   PDU_Data **pduArray = malloc(sizeof(PDU_Data * )* windowsize);
   Window *window= malloc(sizeof(Window));
   
   window->upper = windowsize;
   window->lower = 0;
   window->current = 0;
   window->windowsize = windowsize;
   window->pduArray = pduArray;
 
   return window;

}

void printWindow_metadata(Window * window)
{
    printf("windowsize %d, current %d, lower %d, upper %d\n", window->windowsize, window->current, window->lower, window->upper);

}

void printEntireWindow(Window * window)
{
   int i;
   PDU_Data * pdu_Data;
   for(i = 0; i < window->windowsize; i++)
   {

      pdu_Data = window->pduArray[i];
      if(pdu_Data != NULL)
  	      printf("\t%d sequenceNumber: %d pduSize: %d\n", i, pdu_Data->seqNum, pdu_Data->length );   
      else 
         printf("\t%d not valid\n",i);
   } 

}
// add sequence number as parameter and not extract it from packet
void addPDUtoWindow(Window * window, uint8_t *pduBuffer, int len, uint32_t seqNum)
{
      //uint32_t seqNum;
      int index;
      PDU_Data *pdu_data;
      pdu_data = malloc(sizeof(PDU_Data));
   
      index = seqNum % window->windowsize; 
      printf("index %d\n", index);
      printf("window size %d\n", window->windowsize); 
      pdu_data->seqNum = seqNum;
      pdu_data->length = len;
      pdu_data->index = index;
      pdu_data->isValid = 1;
      pdu_data->pdu = pduBuffer;    
    
      window->pduArray[index] = pdu_data;
}

int isOpen(Window * window)
{
   if( window->upper >  window->current)
      return 1;
   return 0;
}

PDU_Data * findPDU(Window * window, uint32_t seqNum)
{
         
	int i = 0;        
   i = seqNum % window->windowsize; 
	return  window->pduArray[i];
}
// int RR will prob be seq number + 1 
void process_RR(Window * window, int RR)
{

        int i;
        for(i = 0; i <window->windowsize; i++)
        {
            if(window->pduArray[i]->seqNum < RR)
            {
        	      free(window->pduArray[i]);
       	      window->pduArray[i] = NULL;
            }

        }

        if(window->lower < RR)
        {
            window->lower = RR;
    	      window->upper = window->lower + window->windowsize;
        }
}