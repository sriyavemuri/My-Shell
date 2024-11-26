#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

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
    } else if (strcmp(arg[0],"pwd") == 0) { /* pwd */
        char cwd[1024]; // cwd = current working directory
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("Failure with pwd.\n");
        }
    } else if (strcmp(arg[0],"which") == 0) { /* which */
        if (arg[1] == NULL) {
            fprintf(stderr, "Failure with which: Missing arguments.\n");
        } else {
            const char *PATHS = {"/usr/local/bin", "/usr/bin", "/bin"};
            char *command = arg[1];
            int boolean = 0;
            for (int i = 0; i < 3; i++) {
                char full[1024];
                snprintf(full, sizeof(full), "%s%s", PATHS[i], arg[1]);
                if (access(full, X_OK) == 0) {
                    printf("%s\n", full);
                    boolean = 1;
                    return;
                }
            }
            if (boolean == 0) {
                fprintf(stderr, "Failure with which: %s command not found\n", arg[1]);
            }
        }
    } else if (strcmp(arg[0],"exit") == 0) { /* exit */
        for (int i = 1; arg[i] != NULL; i++) {
            printf("%s ", arg[i]);
        }
        printf("\n");
        exit(EXIT_SUCCESS);
    }
}

/**
 * Recursive function to handle piping. This is recursive so that multiple arguments can be handled.
 * i.e. the input a | b | c | d should work
 */
void handlepiping(char **command, int numberOfCommands, int input) {
    if (numberOfCommands == 0) {
        return; // base case of 0 commands left
    }
    int pipefd[2];
    if (numberOfCommands > 1 && pipe(pipefd) == -1) { // may have to fix this
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == 0) {
        // Child
        if (input != STDIN_FILENO) {
            dup2(input, STDIN_FILENO); // redirecting input
            close(input);
        }
        if (numberOfCommands > 1) {
            dup2(pipefd[1], STDOUT_FILENO); // redirects output to pipe
            close(pipefd[0]);
            close(pipefd[1]);
        }
        
        // Here you would parse the next command. IMPLEMENT ONCE PARSE COMMAND FUNCTION WORKS
        // Currently commented out this entire part, can uncomment and fix later
        // if (inputfile) {
        //     int infd = open(inputfile, 0_RDONLY);
        //     if (infd = -1) {
        //         perror("Failed to open input file");
        //         exit(EXIT_FAILURE);
        //     }
        //     dup2(infd, STDIN_FILENO);
        //     close(fd_in);
        // }
        // if (outputfile) {
        //     int outfd = open(outputfile, 0_WRONLY | 0_CREAT | 0_TRUNC, 0640);
        //     if (outfd == -1) {
        //         perror("Failed to open output file");
        //         exit(EXIT_FAILURE);
        //     }
        //     dup2(outfd, STDOUT_FILENO);
        //     close(outfd);
        // }
        // execvp(arg[0], arg);
        // perror("Execution failed");
        // exit(EXIT_FAILURE);
    } else if (pid <0) {
        perror("Fork failed at Child Process creation");
        exit(EXIT_FAILURE);
    }
    // Parent process
    if (input != STDIN_FILENO) {
        close(input);
    }
    if (numberOfCommands > 1) {
        close(pipefd[1]);
    }
    waitpid(pid, NULL, 0); // waits for child process

    // Recursive call
    if (numberOfCommands > 1) {
        handlepiping(&command[1], numberOfCommands - 1, pipefd[0]);
    }
}

void execute_command(char **arg, char *inputf, char *outputf, int ispipe) {
    // Handle built in functions
    if (is_builtin(arg[0])) {
        execute_builtin(arg);
        return;
    }
    // handling piping
    if (ispipe) {
        char **commands = malloc(sizeof(char *) * 128);
        if (commands == NULL) {
            perror("Failed to allocate memory for commands.");
            exit(EXIT_FAILURE);
        }

        int numberOfCommands = 0;
        char *token = strtok(arg[0], "|");
        while (token != NULL) {
            if (numberOfCommands >= 128) {
                commands = realloc(commands, sizeof(char *) * (numberOfCommands + 128));
                if (commands == NULL) {
                    perror("Failed to reallocate memory for commands");
                    exit(EXIT_FAILURE);
                }
            }
            commands[numberOfCommands++] = strdup(token);
            token = strtok(NULL, "|");
        }
        handlepiping(commands, numberOfCommands, STDIN_FILENO);

        // free for malloc

        for (int i = 0; i < numberOfCommands; i++) {
            free(commands[i]);
        }
        free(commands);
        return;
    }
    
    // normal execution
    pid_t pid = fork();
    if (pid ==0) {
        // child
        if (inputf) {
            int infd = open(inputf, O_RDONLY);
            if (infd == -1) {
                perror("Normal execution of files. Failed to open input file.");
                exit(EXIT_FAILURE);
            }
            dup2(infd, STDIN_FILENO);
            close(infd);
        }
        if (outputf) {
            int outfd = open(outputf, O_WRONLY | O_CREAT | O_TRUNC , 0640);
            if (outfd == -1) {
                perror("Normal execution of files. Failed to open output file");
                exit(EXIT_FAILURE);
            }
            dup2(outfd, STDOUT_FILENO);
            close(outfd);
        }
        execvp(arg[0], arg);
        perror("Execution of failed");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Fork failed");
    } else {
        // parent
        waitpid(pid, NULL, 0); // waits for child process
    }
}

void input_to_command_execution(char *i) {
    char **arg = NULL;
    // parsing command (sample method). can change this later.
    if (!parsing_command(i, &arg)) {
        return; // returns if parsing fails
    }
    // execute command
    if (arg[0] == NULL) {
        return; // no command
    }
    execute_builtin(arg); // change later

    // free args (use if malloc is used)
    /* insert command here */
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