#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#define PATHCOUNT 3

const char *PATHS[PATHCOUNT] = {"/usr/local/bin", "/usr/bin", "/bin"};
// PART ONE: READING THE INPUT STRINGS

// PART TWO: PARSING COMMANDS

// PART THREE: THE LAUNCHER/COMMAND EXECUTOR
int is_builtin(char *command) {
    // Built-In Commands: cd, pwd, exit, which
    return strcmp(command, "cd") == 0 || strcmp(command, "pwd") == 0 || strcmp(command, "which") == 0 || strcmp(command, "exit") == 0;
}

void execute_builtin(char **arg) {
    /* cd */
    if (strcmp(arg[0], "cd") == 0) {
        if (arg[1] == NULL) {
            fprintf(stderr, "Failure with cd: Missing arguments.\n");
        } else  {
            if (chdir(arg[1]) == -1) {
                perror("Failure with cd: Directory does not exist.\n");
            }
        }
    } else if (strcmp(arg[0],"pwd") == 0) {
        char cwd[1024]; // cwd = current working directory
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("Failure with pwd.\n");
        }
    } else if (strcmp(arg[0],"which") == 0) {
        if (arg[1] == NULL) {
            fprintf(stderr, "Failure with which: Missing arguments.\n");
        } else {
            char *command = arg[1];
            if (strchr(arg[1], '/')) {
                printf("%s\n", arg[1]);
                return;
            }
            
            for (int i = 0; i < PATHCOUNT; i++) {
                char full[1024];
                snprintf(full, sizeof(full), "%s%s", PATHS[i], arg[1]);
                if (access(full, X_OK) == 0) {
                    printf("%s\n", full);
                    return;
                }
            }
        }
    } else if (strcmp(arg[0],"exit") == 0) {
        for (int i = 1; arg[i] != NULL; i++) {
            printf("%s ", arg[i]);
        }
        printf("\n");
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[]) {
    // 3a. Interactive vs Batch Mode Support
    if (isatty(STDIN_FILENO)) {
        /* INTERACTIVE */
        printf("Welcome to my shell!\n");
        while (1) {
            printf("mysh> ");
            // CALL TO READ THE INPUT STRINGS
            // CALL TO PARSE COMMANDS
        }
        printf("mysh: exiting\n");
    } else {
        /* BASH */
        // CALL TO READ THE INPUT STRINGS
        // CALL TO PARSE COMMANDS
    }
}

// 3b. Creating the spawning processes
// 3c. Handling Input and Output Redirection
// 3d. Executing Commands with Pipes
// 3e. Waiting for child process to finish
// 3f. Exit/End Program