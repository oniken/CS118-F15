# the compiler: gcc for C program, define as g++ for C++
CC = g++
RM = rm -rf
# compiler flags:
#  -g    adds debugging information to the executable file
CFLAGS  = -g

# the build target executable:
TARGET = server
CLIENT = client
PACKET = Packet
PACKETSTREAM = PacketStream

all: $(TARGET) $(CLIENT)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp $(PACKET).cpp $(PACKETSTREAM).cpp

$(CLIENT): $(CLIENT).cpp
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT).cpp $(PACKET).cpp $(PACKETSTREAM).cpp

clean:
	$(RM) $(TARGET) $(TARGET).o $(CLIENT) $(CLIENT).o
