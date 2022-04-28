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
      perror("Node Malloc");
      exit(EXIT_FAILURE);
   }
   
   node->handle = handleName;
   node->handle_len = strlen((const char *)handleName);
   node->socketNum = socketNumber;
   node->prev = NULL;
   node->next = NULL;

   return node;  
}


// Function sees if the handle exists if so, return the Node that has the handle, otherwise return NULL if it doesn't exists
HandleNode *getHandleNodeByHandle(uint8_t *handle, HandleNode **list)
{
   HandleNode *node = NULL;
   HandleNode *cur = *list; // have a current node that  
   while (cur != NULL)
      if (!strcmp((const char *)cur->handle, (const char*)handle)) // check if they are equal to each other 
         node = cur;
      else
         cur = cur->next;      
     
   return node;
}

void addToHandleNodeList(int socketNumber, uint8_t * handleName, HandleNode **list)
{
   HandleNode *node, *cur;
   node = createHandleNode(socketNumber, handleName);	// create the handle Node
   cur = *list;
   if (*list == NULL) // if list is empty set list t
   {
      *list = node;
      return;
   }   

   while (cur->next != NULL) // loops until end of list
      cur = cur->next;
   cur->next = node;
   node->prev = cur;
   
}