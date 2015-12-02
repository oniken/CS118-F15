CC=g++
CFLAGS = -g 
RM = rm -rf
# uncomment this for SunOS
# LIBS = -lsocket -lnsl

all: server client

server: server.cpp
	$(CC) $(CFLAGS) -o server server.cpp PacketStream.cpp Packet.cpp

client: client.cpp 
	$(CC) $(CFLAGS) -o client client.cpp PacketStream.cpp Packet.cpp

server.o: server.cpp port.h

client.o: client.cpp port.h

clean:
	rm -f server client server.o client.o 
