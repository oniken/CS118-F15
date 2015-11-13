# the compiler: gcc for C program, define as g++ for C++
CC = g++
RM = rm -rf
# compiler flags:
#  -g    adds debugging information to the executable file
CFLAGS  = -g

# the build target executable:
TARGET = server
CLIENT = client

all: $(TARGET) $(CLIENT)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp

$(CLIENT): $(CLIENT).cpp
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT).cpp

clean:
	$(RM) $(TARGET) $(TARGET).o $(CLIENT) $(CLIENT).o
