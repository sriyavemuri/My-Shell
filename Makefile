# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Source files
SOURCES = mysh.c arraylist.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Target executable
TARGET = mysh

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
