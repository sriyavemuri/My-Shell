# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Target executable
TARGET = myshell

# Default target
all: $(TARGET)

# Compile target
$(TARGET): myshell.c
	$(CC) $(CFLAGS) -o $(TARGET) myshell.c

# Clean up build files
clean:
	rm -f $(TARGET)