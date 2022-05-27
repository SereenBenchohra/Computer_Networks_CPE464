#include "windowing.h"

#include "pdu.h"

#define PAYLOAD_MAXBUF 1400


void printEOFerror()                                                            
{                                                                               
   fprintf(stderr, "Unexpected EOF\n");                                         
   exit(EXIT_FAILURE);                                                          
}                                                                               
                                                                                
void suddenEOF(int input)                                                       
{                                                                               
   if(input == EOF)                                                             
      printEOFerror();                                                          
                                                                                
}

void remove_extra()                                                             
{                                                                               
   int c;                                                                       
   while( (c = getchar()) != '\n' && c != EOF);                                 
   suddenEOF(c);                                                                
                                                                                
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

      if( window->upper > window->current)
      {
         datalen = readFromStdin(buffer);
         len = createPDU(pduBuffer, seqNum++, 3,(uint8_t *)buffer , datalen);
         addPDUtoWindow(window, pduBuffer, len+1);
         printWindow_metadata(window);
         window->current++;        
      }

      else 
      {
      
         printEntireWindow(window);

         printWindow_metadata(window);
         printf("Enter RR: ");
         scanf("%d",&RR);
         remove_extra();  
         process_RR(window, RR); 

         printEntireWindow(window);

         printWindow_metadata(window);
      }
   }
   return 0;
}

