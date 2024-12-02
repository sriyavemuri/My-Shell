#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <glob.h>
#include "arraylist.h"
#include <ctype.h>

// PART ONE: READING THE INPUT STRINGS
typedef struct {
	int fd;
	size_t current;     // Current position in the buffer.
    size_t length;     // Number of valid bytes in the buffer.
    char buffer[1024]; // Fixed-size buffer for reading.
} data;

char* readAndGetLine(data* stream) {
    if (!stream || stream->fd < 0) {
        return NULL; // Invalid stream
    }

    char* line = NULL; // Accumulated line
    size_t line_length = 0;
    size_t line_capacity = 128; // Initial capacity for the line

    line = malloc(line_capacity * sizeof(char));
    if (!line) {
        perror("Failed to allocate memory for line");
        return NULL;
    }

    while (1) {
        // If the buffer is exhausted, refill it
        if (stream->current == stream->length) {
            stream->length = read(stream->fd, stream->buffer, sizeof(stream->buffer));
            if (stream->length == 0) { // End of file
                if (line_length == 0) { // No data read for the line
                    free(line);
                    return NULL;
                }
                line[line_length] = '\0';
                return line;
            } else if (stream->length == (size_t)-1) { // Error
                perror("Error reading from stream");
                free(line);
                return NULL;
            }
            stream->current = 0; // Reset position
        }


        // Process characters in the buffer
        while (stream->current < stream->length) {
            char c = stream->buffer[stream->current++];
            if (c == '\n') { // Line complete
                line[line_length] = '\0';
                return line;
            }
            // Expand memory if line capacity is reached
            if (line_length + 1 >= line_capacity) {
                line_capacity *= 2;
                line = realloc(line, line_capacity * sizeof(char));
                if (!line) {
                    perror("Failed to reallocate memory for line");
                    return NULL;
                }
            }
            line[line_length++] = c;
        }
    }
}
// PART TWO: PARSING COMMANDS
arraylist_t *tokenize(char *line, char ***all_strings) {
    if (!line) return NULL;

    // Initialize the tokens arraylist
    arraylist_t *tokens = malloc(sizeof(arraylist_t));
    if (!tokens) {
        perror("Failed to allocate memory for tokens array");
        return NULL;
    }
    al_init(tokens, 10);

    // Initialize `all_strings` array
    size_t all_strings_capacity = 10;
    *all_strings = malloc(sizeof(char *) * all_strings_capacity);
    if (!*all_strings) {
        perror("Failed to allocate memory for all_strings array");
        free(tokens);
        return NULL;
    }
    size_t all_strings_length = 0;

    char token_buffer[1024];
    size_t buffer_length = 0;
    size_t len = strlen(line);
    int quotemode = 0;
    char quotetype = '\0';

    for (size_t i = 0; i <= len; i++) {
        char c = line[i];

        // Handle token separators or end of line
        if ((c == '"' || c == '\'') && !quotemode) {
            quotemode = 1;
            quotetype = c;
        } else if (c == quotetype && quotemode) {
            quotemode = 0;          // Exit quote mode
        } else if (isspace(c) || c == '\0' || c == '<' || c == '>' || c == '|') {
            if (buffer_length > 0) {
                token_buffer[buffer_length] = '\0'; // Null-terminate the token

                // Check for wildcard
                if (strchr(token_buffer, '*')) {
                    glob_t glob_result;
                    if (glob(token_buffer, 0, NULL, &glob_result) == 0) {
                        for (size_t j = 0; j < glob_result.gl_pathc; j++) {
                            // Expand each matched path and add to all_strings
                            if (all_strings_length >= all_strings_capacity) {
                                all_strings_capacity *= 2;
                                *all_strings = realloc(*all_strings, sizeof(char *) * all_strings_capacity);
                                if (!*all_strings) {
                                    perror("Failed to reallocate memory for all_strings");
                                    free(tokens);
                                    globfree(&glob_result);
                                    return NULL;
                                }
                            }

                            (*all_strings)[all_strings_length] = strdup(glob_result.gl_pathv[j]);
                            al_append(tokens, all_strings_length++);
                        }
                    } else {
                        // No matches; treat as a regular token
                        if (all_strings_length >= all_strings_capacity) {
                            all_strings_capacity *= 2;
                            *all_strings = realloc(*all_strings, sizeof(char *) * all_strings_capacity);
                            if (!*all_strings) {
                                perror("Failed to reallocate memory for all_strings");
                                free(tokens);
                                return NULL;
                            }
                        }
                        (*all_strings)[all_strings_length] = strdup(token_buffer);
                        al_append(tokens, all_strings_length++);
                    }
                    globfree(&glob_result);
                } else {
                    // Regular token (non-wildcard)
                    if (all_strings_length >= all_strings_capacity) {
                        all_strings_capacity *= 2;
                        *all_strings = realloc(*all_strings, sizeof(char *) * all_strings_capacity);
                        if (!*all_strings) {
                            perror("Failed to reallocate memory for all_strings");
                            free(tokens);
                            return NULL;
                        }
                    }
                    (*all_strings)[all_strings_length] = strdup(token_buffer);
                    al_append(tokens, all_strings_length++);
                }
                buffer_length = 0; // Reset buffer for next token
            }
        } else {
            // Add character to token buffer
            token_buffer[buffer_length++] = c;
        }
    }

    return tokens;
}

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
                fprintf(stderr, "cd: No such file or directory\n");
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
            const char *PATHS[] = {"/usr/local/bin", "/usr/bin", "/bin"};
            // char *command = arg[1];
            int boolean = 0;
            for (int i = 0; i < 3; i++) {
                char full[1024];
                snprintf(full, sizeof(full), "%s/%s", PATHS[i], arg[1]);
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
        // printf("\n");
        printf("mysh: exiting\n");
        exit(EXIT_SUCCESS);
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
        int status;
        waitpid(pid, &status, 0); // waits for child process
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            // If the child process exited with a non-zero status, print an error
            printf("mysh: Command failed with code %d\n", WEXITSTATUS(status));
        }
    }
}

void input_to_command_execution(arraylist_t* tokens, char** all_strings) {
    if (!tokens || tokens->length == 0) return;

    char** args = malloc(sizeof(char*) * (tokens->length + 1));
    if (!args) {
        perror("Failed to allocate memory for arguments");
        return;
    }

    char* input_file = NULL;
    char* output_file = NULL;
    int is_pipe = 0;

    unsigned arg_count = 0;

    for (unsigned i = 0; i < tokens->length; i++) {
        int index = tokens->data[i];
        char* current_token = all_strings[index];

        if (strcmp(current_token, "<") == 0 && i + 1 < tokens->length) {
            input_file = all_strings[tokens->data[++i]];
        } else if (strcmp(current_token, ">") == 0 && i + 1 < tokens->length) {
            output_file = all_strings[tokens->data[++i]];
        } else if (strcmp(current_token, "|") == 0) {
            is_pipe = 1;
            args[arg_count] = NULL;
            execute_command(args, input_file, output_file, is_pipe);
            arg_count = 0;
            input_file = output_file = NULL;
            is_pipe = 0;
        } else {
            args[arg_count++] = strdup(current_token);
        }
    }

    args[arg_count] = NULL;
    execute_command(args, input_file, output_file, is_pipe);
    for (unsigned i =0; i < arg_count; i++)  {
        free(args[i]);
    }
    free(args);
}

// MAIN
int main(int argc, char* argv[]) {

    data* stream = malloc(sizeof(data));
    if (!stream) {
        perror("Failed to allocate memory for input stream");
        return EXIT_FAILURE;
    }

    // interactive vs batch
    stream->fd = (argc == 1) ? STDIN_FILENO : open(argv[1], O_RDONLY);
    if (stream->fd == -1) {
        perror("Failed to open input file");
        free(stream);
        return EXIT_FAILURE;
    }

    stream->current = 0;
    stream->length = 0;

    int mode = isatty(STDIN_FILENO);
    
    // interactive welcome message
    if (mode==1 && argc ==1) {
        printf("Welcome to my shell!\n");
    }

    while (1) {
        if (mode==1 && argc==1) { // Interactive Mode
            printf("mysh> ");
            fflush(stdout);
        }

        char* line = readAndGetLine(stream);
        if (!line) break;

        char** all_strings = NULL;
        arraylist_t* tokens = tokenize(line, &all_strings);
        free(line);

        if (tokens && tokens->length > 0) {

            // Execute the command
            input_to_command_execution(tokens, all_strings);

            for (size_t i = 0; i < tokens->length; i++) {
                free(all_strings[i]);
            }
            free(all_strings);
            al_destroy(tokens);
            free(tokens);
        }
    }

    free(stream);

    // test batch
    if (!mode && argc>1) {
        printf("$\n");
        exit(EXIT_SUCCESS);
    }
    return EXIT_SUCCESS;
}
