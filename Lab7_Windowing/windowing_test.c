#include "windowing.h"

#include "pdu.h"

#define PAYLOAD_MAXBUF 1400


void clear_scanf_buf()                                                             
{                                                                               
   int c;                                                                       
   while( (c = getchar()) != '\n' && c != EOF);                                 
   if (c == EOF)
   {
      fprintf(stderr, "Unexpected EOF\n");                                         
      exit(EXIT_FAILURE);  
   }
                                                                                   
}                                                                               
  

int readFromStdin(char * buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	printf("Enter data: ");
	while (inputLen < (PAYLOAD_MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}
	
	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;
	
	return inputLen;
}



int main(int argc, char * argv[])
{
   Window *window;
   window = create_Window(4); 
   int datalen;
   uint32_t seqNum =  0;
   int RR = 0;
   int len;
   char buffer[PAYLOAD_MAXBUF];

   uint8_t pduBuffer[PAYLOAD_MAXBUF + 8];
   while(1)
   {

      if( window->upper > window->current) // if window is open
      {
         datalen = readFromStdin(buffer); // read data from STDIN
         len = createPDU(pduBuffer, seqNum++, 3,(uint8_t *)buffer , datalen); // create a pdu and add it to your window
         add_PDU_to_Win_Buff(window, pduBuffer, len+1);
         printWindow_metadata(window); // print out meta data
         window->current++;/// increment current window        
      }
      else // when window is closed  
      {
     
         printEntireWindow(window); 
         printWindow_metadata(window);
         printf("Enter RR: ");
         scanf("%d",&RR);
         clear_scanf_buf();  // clear buffer to prevent overflow
         process_RR(window, RR); // rr the sequence
         printEntireWindow(window);
         printWindow_metadata(window);
      }
   }
   return 0;
}

