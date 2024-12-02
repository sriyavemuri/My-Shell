#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return 1;
    }
    int count = atoi(argv[1]);
    if (count <= 0) {
        fprintf(stderr, "Error: Please provide a positive number.\n");
        return 1;
    }
    for (int i = 0; i < count; i++) {
        putchar('a');
    }
    putchar('\n');
    return 0;
}