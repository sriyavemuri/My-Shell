#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    int product = 1; // Initialize product as 1 for multiplication

    // Handle command-line arguments
    for (int i = 1; i < argc; i++) {
        product *= atoi(argv[i]);
    }

    // Check if input is coming from stdin (pipe or redirection)
    if (!isatty(STDIN_FILENO)) {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), stdin)) {
            char *token = strtok(buffer, " \n");
            while (token != NULL) {
                if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
                    product *= atoi(token);
                }
                token = strtok(NULL, " \n");
            }
        }
    }

    printf("%d\n", product);
    return 0;
}
