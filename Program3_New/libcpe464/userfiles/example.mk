# Makefile for CPE464 library 
# Can link with 32, 64 and Mac versions of the library 
#
# Use if you want - you do NOT need to use this - provided to help only
#
# Usage (assume its called Makefile): make  (makes all for a particular machine architecture)
# note - if you are moving between machine architectures you must first delete your .o files

CC = gcc
CFLAGS = -g -Wall -Werror 

LIBS += -lstdc++ -ldl
SRCS = $(shell ls *.cpp *.c 2> /dev/null | grep -v rcopy.c | grep -v server.c | grep -v rcopy.cpp | grep -v server.cpp )
OBJS = $(shell ls *.cpp *.c 2> /dev/null | grep -v rcopy.c | grep -v server.c | grep -v rcopy.cpp | grep -v server.cpp | sed s/\.c[p]*$$/\.o/ )
HFILES = $(shell ls *.h 2> /dev/null)
LIBNAME = $(shell ls *cpe464*.a 2> /dev/null | tail -n 1)

#use the unique library names for each architecture
ARCH = $(shell arch)
OS = $(shell uname -s)
ifeq ("$(OS)", "Linux")
	LIBS1 = -lstdc++
endif

#handle macs
ifeq ("$(OS)", "Darwin")
	LIBNAME = $(shell ls *cpe464*.a 2> .dev.null | tail -n 1)
endif

ALL = check_lib rcopy server

all:  $(OBJS) $(ALL)

lib:
	make -f lib.mk

check_lib: 
	@if [ "$(LIBNAME)" = "" ]; then \
		echo " ";  \
		echo "Library missing: libcpe464.X.Y.a";  \
		echo "Need to install the CPE464 library - see polyLearn"; \
		echo ""; \
		exit -1; fi

	
echo:
	@echo "CFLAGS: " $(CFLAGS)  
	@echo "SRCS: $(SRCS)"
	@echo "Objects: $(OBJS)"
	@echo "HFILES: $(HFILES)"
	@echo "LIBNAME: $(LIBNAME)"

%.o: %.cpp $(HFILES)
	@echo "-------------------------------"
	@echo "*** Building $@"
	$(CC) -c $(CFLAGS)  $< -o $@ $(LIBS1)

%.o: %.c $(HFILES) 
	@echo "-------------------------------"
	@echo "*** Building $@"
	$(CC) -c $(CFLAGS) $< -o $@ $(LIBS1)

rcopy: rcopy.o  $(HFILES) $(OBJS)
	@echo "-------------------------------"
	@echo "*** Linking $@ with library: $(LIBNAME)... "
	$(CC) $(CFLAGS) -o $@ rcopy.o $(OBJS) $(LIBNAME) $(LIBS)
	@echo "*** Linking Complete!"
	@echo "-------------------------------"

server: server.o $(HFILES) $(OBJS)
	@echo "-------------------------------"
	@echo "*** Linking $@ with library: $(LIBNAME)... "
	$(CC) $(CFLAGS) -o $@ server.o $(OBJS) $(LIBNAME) $(LIBS)
	@echo "*** Linking Complete!"
	@echo "-------------------------------"

# clean .o
cleano: 
	@echo "-------------------------------"
	@echo "*** Cleaning Files..."
	@echo "Deleting *.o's only"
	rm -f *.o 
	@echo "-------------------------------"
	
# clean targets
clean: 
	@echo "-------------------------------"
	@echo "*** Cleaning Files..."
	@echo "Deleting *.o's and rcopy and server"
	rm -f *.o $(ALL)
	@echo "-------------------------------"
