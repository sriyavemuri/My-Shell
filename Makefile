# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Source files for mysh
SOURCES = mysh.c arraylist.c

# Object files for mysh
OBJECTS = $(SOURCES:.c=.o)

# Target executable
TARGET = mysh

# Additional utilities
UTILITIES = testdir/generateatest testdir/countatest

# Default target
all: $(TARGET) $(UTILITIES)

# Compile target for mysh
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build utility generateatest
testdir/generateatest: testdir/generateatest.c
	$(CC) $(CFLAGS) -o testdir/generateatest testdir/generateatest.c

# Build utility countatest
testdir/countatest: testdir/countatest.c
	$(CC) $(CFLAGS) -o testdir/countatest testdir/countatest.c

# Clean up build files
clean:
	rm -f $(OBJECTS) $(TARGET) testdir/*.o $(UTILITIES)
