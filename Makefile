CC = g++
CFLAGS = -O0 -g -I. -c -DLINUX
OUT = ../Debug/libTastySocket.a


OBJS =	TastySocket.o\
        TastyStream.o\
	    TastyClient.o\
	    TastyServer.o

all:	createLibrary

createLibrary:	$(OBJS)
		ar -r $(OUT) $(OBJS)

.cpp.o:
	$(CC) $(CFLAGS) -o $*.o $*.cpp

clean:
	rm -f *.o $(OUT) *~
