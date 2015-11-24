CC=gcc
CFLAGS = -g 
# uncomment this for SunOS
# LIBS = -lsocket -lnsl

all: server client

server: server.o 
	$(CC) -o server server.o $(LIBS)

client: client.o 
	$(CC) -o client client.o $(LIBS)

server.o: server.cpp port.h

client.o: client.cpp port.h

clean:
	rm -f server client server.o client.o 
