#include <stdint.h>

#ifndef HANDLENODE_H
#define HANDLENODE_H


typedef struct node
{
   int handle_len; // store handle len 
   uint8_t *handle;  //store handle name
   int socketNum; // store socket number associated with that name
   struct node *next, *prev; 

}HandleNode;

HandleNode *createHandleNode(int socketNumber, uint8_t *handleName); // create the handle Node
HandleNode *getHandleNodeByHandle(uint8_t *handle, HandleNode **list);
void addToHandleNodeList(int socketNumber, uint8_t * handleName, HandleNode **list);
void print_list(HandleNode **list);


#endif 