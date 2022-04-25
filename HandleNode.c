#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "HandleNode.h"


HandleNode *createHandleNode(int socketNumber, uint8_t * handleName)	// create the handle Node
{
   HandleNode *node = NULL; 
   node = malloc(sizeof(HandleNode));
   if (node == NULL)
   {
      perror("Malloc");
      exit(EXIT_FAILURE);
   }
   
   node->handle = handleName;
   node->handle_len = strlen((const char *)handleName);
   node->socketNum = socketNumber;
   node->prev = NULL;
   node->next = NULL;

   return node;  
}

