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
UTILITIES = testcfiles/generateatest testcfiles/sumtest testcfiles/product

# Default target
all: $(TARGET) $(UTILITIES)

# Compile target for mysh
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build utility generateatest
testcfiles/generateatest: testcfiles/generateatest.c
	$(CC) $(CFLAGS) -o testcfiles/generateatest testcfiles/generateatest.c

# Build utility sumtest
testcfiles/sumtest: testcfiles/sumtest.c
	$(CC) $(CFLAGS) -o testcfiles/sumtest testcfiles/sumtest.c

# Build utility sumtest
testcfiles/product: testcfiles/product.c
	$(CC) $(CFLAGS) -o testcfiles/product testcfiles/product.c

# Clean up build files
clean:
	rm -f $(OBJECTS) $(TARGET) testcfiles/*.o $(UTILITIES)
