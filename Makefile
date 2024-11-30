# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Source files
SOURCES = myshell.c arraylist.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Target executable
TARGET = myshell

# Default target
all: $(TARGET)

# Compile target
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJECTS) $(TARGET)
