# the compiler: gcc for C program, define as g++ for C++
CC = gcc
RM = rm -rf
# compiler flags:
#  -g    adds debugging information to the executable file
CFLAGS  = -g

# the build target executable:
TARGET = server

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

client:
	gcc -g -o client client.c

clean:
	$(RM) $(TARGET) $(TARGET).o
