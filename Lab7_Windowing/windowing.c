#include "windowing.h"

// create/ Initialize your Window buffer
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

void add_PDU_to_Win_Buff(Window * window, uint8_t *pduBuffer, int len) // add a PDU Window Buffer
{

      uint32_t seqNum;
      int index;
      PDU_Data *pdu_data = malloc(sizeof(PDU_Data));
  
      memcpy(&seqNum, pduBuffer, sizeof(uint32_t));

      seqNum = ntohl(seqNum);
 
      index = seqNum % window->windowsize; 
      printf("index %d\n", index);
      printf("window size %d\n", window->windowsize); 
      pdu_data->seqNum = seqNum;
      pdu_data->length = len;
      pdu_data->index = index;
      pdu_data->isValid = TRUE;
      pdu_data->pdu = pduBuffer;    
    
      window->pduArray[index] = pdu_data;
}

void printWindow_metadata(Window * window)
{
    printf("windowsize %d, current %d, lower %d, upper %d\n", window->windowsize, window->current, window->lower, window->upper);

}

void printEntireWindow(Window * window)
{
   int i;
   PDU_Data *pdu_Data;
   for(i = 0; i < window->windowsize; i++)
   {
      pdu_Data = window->pduArray[i];
      if(pdu_Data != NULL)
  	      printf("\t%d sequenceNumber: %d pduSize: %d\n", i, pdu_Data->seqNum, pdu_Data->length );   
      else 
         printf("\t%d not valid\n",i);
   } 

}

// Retrieve a particular PDU from the Window Buffer (e.g to be used when a PDU has been SREJ)
PDU_Data * findPDU(Window * window, uint32_t seqNum)
{  
	int i = seqNum % window->windowsize; 
	return  window->pduArray[i];
}

void process_RR(Window * window, int RR) // checks if the sequence num is less than RR , if so remove it
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

        if(window->lower < RR) // if window is lower than RR set lower window to RR value
        {
            window->lower = RR;
    	      window->upper = window->lower + window->windowsize; // update the upper based on the window size and lower value
        }
}

int window_status(Window *window) // returns the window's open /close status
{
   if( window->upper >  window->current)
      return OPEN;
   return CLOSE;
}
