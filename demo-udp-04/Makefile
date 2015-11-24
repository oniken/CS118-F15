CC=gcc
CFLAGS = -g 
# uncomment this for SunOS
# LIBS = -lsocket -lnsl

all: server client

server: server.o 
	$(CC) -o server server.o $(LIBS)

client: client.o 
	$(CC) -o client client.o $(LIBS)

server.o: server.c port.h

client.o: client.c port.h

clean:
	rm -f server client server.o client.o 
