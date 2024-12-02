#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    char *input = argv[1];
    int count = 0;

    for (size_t i = 0; i < strlen(input); i++) {
        if (input[i] == 'a') {
            count++;
        }
    }

    printf("%d\n", count);
    return 0;
}