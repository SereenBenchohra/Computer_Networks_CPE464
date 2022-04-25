#include "packets.h"
#include <stdio.h>
#include <string.h>

void message_packets()
{
   printf("In message Packet\n");
}

void list_packets()
{
   printf("In List Packet\n");
}

void broadcast_packets()
{
   printf("In Broadcast Packet\n");
}

void exit_packets()
{
   printf("In Exit Packet\n");
}

void check_command(char *string)
{
   if (!strcmp(string, "%M") || !strcmp(string, "%m"))
      message_packets();
   if (!strcmp(string, "%B") || !strcmp(string, "%b"))
      broadcast_packets();
   if (!strcmp(string, "%L") || !strcmp(string, "%l"))
      list_packets();
   if (!strcmp(string, "%E") || !strcmp(string, "%e"))
      exit_packets();

}
// change to return type to void and pass in a list that way there is no overwrite
void split(unsigned char *string, char *delim)
{
   unsigned char *substring, *local;
   local = string; // have local copy to mutate and not affect original string

   substring = strsep(&local, delim); // gets first element in string
   // check for commands 
   check_command(substring);
   //printf("%s\n", substring);
     

}


