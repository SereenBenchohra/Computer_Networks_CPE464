# udpCode makefile
# written by Hugh Smith - Feb 2021

CC = gcc
CFLAGS = -g -Wall

#uncomment next two lines if your using sendtoErr() library
#LIBS += libcpe464.2.21.a -lstdc++ -ldl
#CFLAGS += -D__LIBCPE464_

CC = gcc
CFLAGS = -g -Wall 
#CFLAGS = -g

all:  windowing_test

windowing_test: windowing_test.c
	$(CC) $(CFLAGS) -o windowing_test windowing_test.c checksum.c pdu.c windowing.c

clean:
	rm -f windowing_test

