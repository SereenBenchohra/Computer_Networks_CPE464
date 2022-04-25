# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019

CC= gcc
CFLAGS= -g -Wall
LIBS = 


all:   cclient server

cclient: myClient.c networks.o gethostbyname.o
	$(CC) $(CFLAGS) -o cclient myClient.c appPDU.c packets.c HandleNode.c networks.o gethostbyname.o $(LIBS)

server: myServer.c networks.o gethostbyname.o
	$(CC) $(CFLAGS) -o server myServer.c pollLib.c safeUtil.c appPDU.c networks.o gethostbyname.o $(LIBS)

.c.o:
	gcc -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o

clean:
	rm -f server cclient *.o




