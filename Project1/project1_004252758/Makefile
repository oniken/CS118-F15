# the compiler: gcc for C program, define as g++ for C++
CC = g++
RM = rm -rf
# compiler flags:
#  -g    adds debugging information to the executable file
CFLAGS  = -g

# the build target executable:
TARGET = server

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp

clean:
	$(RM) $(TARGET) $(TARGET).o
