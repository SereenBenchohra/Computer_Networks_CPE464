
// 
// Writen by Hugh Smith, April 2020
//
// Put in system calls with error checking
// and and an s to the name: srealloc()
// keep the function paramaters same as system call

#include <stdlib.h>
#include <stdio.h>

#include "safeUtil.h"


void * srealloc(void *ptr, size_t size)
{
	void * returnValue = NULL;
	
	if ((returnValue = realloc(ptr, size)) == NULL)
	{
		printf("Error on realloc (tried for size: %d\n", (int) size);
		exit(-1);
	}
	
	return returnValue;
} 

void * sCalloc(size_t nmemb, size_t size)
{
	void * returnValue = NULL;
	if ((returnValue = calloc(nmemb, size)) == NULL)
	{
		perror("calloc");
		exit(-1);
	}
	return returnValue;
}

