//
// Written Hugh Smith, Updated: April 2022
// Use at your own risk.  Feel free to copy, just leave my name in it.
//

// Note this is not a robust implementation 
// 1. It is about as un-thread safe as you can write code.  If you 
//    are using pthreads do NOT use this code.
// 2. pollCall() always returns the lowest available file descriptor 
//    which could cause higher file descriptors to never be processed
//
// This is for student projects so I don't intend on improving this. 

#include <poll.h>
#include <stdlib.h>
#include <stdio.h>

#include "safeUtil.h"
#include "pollLib.h"


// Poll global variables 
static struct pollfd * pollFileDescriptors;
static int maxFileDescriptor = 0;
static int currentPollSetSize = 0;

static void growPollSet(int newSetSize);

// Poll functions (setup, add, remove, call)
void setupPollSet()
{
	currentPollSetSize = POLL_SET_SIZE;
	pollFileDescriptors = (struct pollfd *) sCalloc(POLL_SET_SIZE, sizeof(struct pollfd));
}


void addToPollSet(int socketNumber)
{
	
	if (socketNumber >= currentPollSetSize)
	{
		// needs to increase off of the biggest socket number since
		// the file desc. may grow with files open or sockets
		// so socketNumber could be much bigger than currentPollSetSize
		growPollSet(socketNumber + POLL_SET_SIZE);		
	}
	
	if (socketNumber + 1 >= maxFileDescriptor)
	{
		maxFileDescriptor = socketNumber + 1;
	}

	pollFileDescriptors[socketNumber].fd = socketNumber;
	pollFileDescriptors[socketNumber].events = POLLIN;
}

void removeFromPollSet(int socketNumber)
{
	pollFileDescriptors[socketNumber].fd = 0;
	pollFileDescriptors[socketNumber].events = 0;
}

int pollCall(int timeInMilliSeconds)
{
	// returns the socket number if one is ready for read
	// returns -1 if timeout occurred
	// if timeInMilliSeconds == -1 blocks forever (until a socket ready)
	// (this -1 is a feature of poll)
	// If timeInMilliSeconds == 0 it will return immediately after looking at the poll set
	
	int i = 0;
	int returnValue = -1;
	int pollValue = 0;
	
	if ((pollValue = poll(pollFileDescriptors, maxFileDescriptor, timeInMilliSeconds)) < 0)
	{
		perror("pollCall");
		exit(-1);
	}	
			
	// check to see if timeout occurred (poll returned 0)
	if (pollValue > 0)
	{
		// see which socket is ready
		for (i = 0; i < maxFileDescriptor; i++)
		{
			//if(pollFileDescriptors[i].revents & (POLLIN|POLLHUP|POLLNVAL)) 
			//Could just check for specific revents, but want to catch all of them
			//Otherwise, this could mask an error (eat the error condition)
			if(pollFileDescriptors[i].revents > 0) 
			{
				//printf("for socket %d poll revents: %d\n", i, pollFileDescriptors[i].revents);
				returnValue = i;
				break;
			} 
		}

	}
	
	// Ready socket # or -1 if timeout/none
	return returnValue;
}

static void growPollSet(int newSetSize)
{
	int i = 0;
	
	// just check to see if someone screwed up
	if (newSetSize <= currentPollSetSize)
	{
		printf("Error - current poll set size: %d newSetSize is not greater: %d\n",
			currentPollSetSize, newSetSize);
		exit(-1);
	}
	
	printf("Increasing poll set from: %d to %d\n", currentPollSetSize, newSetSize);
	pollFileDescriptors = srealloc(pollFileDescriptors, newSetSize * sizeof(struct pollfd));	
	
	// zero out the new poll set elements
	for (i = currentPollSetSize; i < newSetSize; i++)
	{
		pollFileDescriptors[i].fd = 0;
		pollFileDescriptors[i].events = 0;
	}
	
	currentPollSetSize = newSetSize;
}



