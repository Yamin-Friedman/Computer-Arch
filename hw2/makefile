# Makefile for the smash program
CC = g++
CFLAGS = -g -std=c++11
CCLINK = $(CC)
OBJS = cacheSim.o Cache.o L1.o L2.o VictimCache.o CacheLine.o
RM = rm -f
# Creating the  executable
cacheSim: $(OBJS)
	$(CCLINK) -o cacheSim $(OBJS)
# Creating the object files
cacheSim.o: cacheSim.cpp L1.h Cache.h 
	$(CC) -c $(CFLAGS) $*.cpp  
Cache.o: Cache.cpp Cache.h
	$(CC) -c $(CFLAGS) $*.cpp
L1.o:  L1.cpp L2.h Cache.h CacheLine.h L2.h VictimCache.h
	$(CC) -c $(CFLAGS) $*.cpp
L2.o: L2.cpp L2.h Cache.h CacheLine.h VictimCache.h
	$(CC) -c $(CFLAGS) $*.cpp
VictimCache.o:  VictimCache.cpp VictimCache.h Cache.h CacheLine.h
	$(CC) -c $(CFLAGS) $*.cpp
CacheLine.o: CacheLine.cpp CacheLine.h
	$(CC) -c $(CFLAGS) $*.cpp
# Cleaning old files before new make
clean:
	rm -f $(OBJS) 