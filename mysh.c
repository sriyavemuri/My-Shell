#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include "arraylist.h"
#include <glob.h>

typedef struct {
	arraylist_t arguments; 
	char *execpath;
	char *inputfile;
	char *outputfile;
} command;

typedef struct {
	int fd;
	size_t current;     // Current position in the buffer.
    	size_t length;     // Number of valid bytes in the buffer.
    	char buffer[1024]; // Fixed-size buffer for reading.
} data;


arraylist_t* tokenize(char* line) {
    if (!line) return NULL;

    arraylist_t* tokens = malloc(sizeof(arraylist_t));
    if (!tokens) {
        perror("Failed to allocate memory for tokens array");
        return NULL;
    }

    // Initialize the arraylist with an initial capacity of 10
    al_init(tokens, 10);

    size_t len = strlen(line);
    char token_buffer[1024]; // Temporary buffer for a single token
    size_t buffer_length = 0;

    for (size_t i = 0; i <= len; i++) {
        char c = line[i];

        if (isspace(c) || c == '<' || c == '>' || c == '|' || c == '\0') {
            // If end of a token is reached
            if (buffer_length > 0) {
                token_buffer[buffer_length] = '\0';

                // Handle wildcard expansion
                if (strchr(token_buffer, '*')) {
                    glob_t results;
          if (glob(token_buffer, 0, NULL, &results) == 0) {
                        for (size_t j = 0; j < results.gl_pathc; j++) {
                char* expanded = malloc(strlen(results.gl_pathv[j]) + 1);
                            if (expanded) {
                                strcpy(expanded, results.gl_pathv[j]);
                    al_append(tokens, expanded);
                            }
                        }
                    } else {
              
                   char* no_match = malloc(strlen(token_buffer) + 1);
                        if (no_match) {
                      strcpy(no_match, token_buffer);
             al_append(tokens, no_match);
                        }
                    }
                    globfree(&results);
                } else {
                    // Regular token, add as-is
                    char* new_token = malloc(strlen(token_buffer) + 1);
               if (new_token) {
                  strcpy(new_token, token_buffer);
                 al_append(tokens, new_token);
                    }
                }

                buffer_length = 0; // Reset the buffer
            }

          // usual tokens
            if (c == '<' || c == '>' || c == '|') {
                char* special_token = malloc(2);
                if (special_token) {
                    special_token[0] = c;
                    special_token[1] = '\0';
                    al_append(tokens, special_token);
                }
            }
        } else {
      
     token_buffer[buffer_length++] = c;
        }
    }

    return tokens;
}



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
            stream->length = read(stream->fd, stream->buffer, BUFFER_SIZE);
            if (stream->length == 0) { // End of file
           if (line_length == 0) { // No data read for the line
         free(line);
           return NULL;
                }
                line[line_length] = '\0';
             return line;
            } else if (stream->length < 0) { // Error
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


int main(int argc, char **argv) {

    int mode = isatty(STDIN_FILENO);
    
    if (argc > 2) {
        fprintf(stderr, "Error: Too many arguments\n");
        return EXIT_FAILURE;
    }

    if(mode == 1){
	    //interactive mode
	    printf("Welcome to my shell!\n");
	    
	    
	    // Initialize the data structure
	    data* stream = malloc(sizeof(data));
	 
	    stream->fd = STDIN_FILENO;
	    stream->current = 0;
	    stream->length = 0;

	    // Main interactive loop
	    while (stream != NULL && get_exit_flag() == CONTINUE) {
		printf("mysh> ");
		fflush(stdout);
	       // execute stream
	       char* line = readAndGetLine(stream);
	       arraylist_t* tokens = tokenize(line);
	       // execute tokens
	       
	       
	    }

	    free(stream);
	    printf("mysh: exiting");
    
    
    }
    else{
    	
    	if(argc == 1){
    	// batch mode with no file provided
    	 input_stream* stream = malloc(sizeof(input_stream));
	 
	    stream->fd = STDIN_FILENO;
	    stream->pos = 0;
	    stream->len = 0;

    while (stream != NULL && stream->fd != -1) {
        char* line = readAndGetLine(stream);
        arraylist_t* tokens = tokenize(line);
        //execute tokens
    }   
    free(stream);
    	
    	}
    	else{
    	// batch mode with file 
    	int fd = open(argv[1], O_RDONLY);

          input_stream* stream = malloc(sizeof(input_stream));
	 
	    stream->fd = fd;
	    stream->pos = 0;
	    stream->len = 0;

    while (stream != NULL && stream->fd != -1) {
        char* line = readAndGetLine(stream);
        arraylist_t* tokens = tokenize(line);
       //execute tokens
    }   
    free(stream);
    	}
    }

    return EXIT_SUCCESS;
}
