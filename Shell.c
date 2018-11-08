//
// Created by Adityaraj Pednekar on 11/7/18.
//

/**
 * https://medium.com/meatandmachines/what-really-happens-when-you-type-ls-l-in-the-shell-a8914950fd73
 * https://en.wikipedia.org/wiki/List_of_Unix_commands
 * https://brennan.io/2015/01/16/write-a-shell-in-c/  ->READ COMPLETE VERY USEFUL
 * https://stackoverflow.com/questions/1296843/what-is-the-difference-between-null-0-and-0
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


void command_loop();

char *readLine();

char **readArgs(char *);

int executeCode(char **);

int main(int args, char *argv[]) {
    // Load config files, if any.
    // Run command loop.
    command_loop();
    // Perform any shutdown/cleanup.
    return EXIT_SUCCESS;
}

void command_loop() {
    char *lines;
    char **args;
    int status = 0;

    do {
        printf(">");
        lines = readLine();
        args = readArgs(lines);
        status = executeCode(args);

    } while (status);

}


char *readLine(void) {
    char *buffer = NULL;
    size_t bufsize = 0;  // Warning -> we pass address of 0 i.e buffsize to getline and not 0 itself
    getline(&buffer, &bufsize,
            stdin);  // Why 0? If the buffer is not large enough to hold the line, getline() resizes it with realloc(3) updating size as necessary
    return buffer;
}

char **readArgs(char *line) {
    //https://stackoverflow.com/questions/3889992/how-does-strtok-split-the-string-into-tokens-in-c   ->See last 4 comments to see how tokenizer works
    int tokenBufferSize = 64;
    char delimiters[] = " \t\r\n\a";
    int index = 0;
    char *argument;
    char **args = malloc(tokenBufferSize * sizeof(char *));

    if (!args) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    argument = strtok(line, delimiters);
    while (argument) {
        args[index] = argument;
        index++;

        if (index >= tokenBufferSize) {
            tokenBufferSize += 64;
            args = realloc(args, tokenBufferSize * sizeof(char *));
            if (!args) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        argument = strtok(NULL, delimiters);
    }
    args[index] = NULL;
    return args;
}

int executeCode(char **args) {
    pid_t childID = fork();
    int status;

    if (childID < 0) {
        perror("child not created");
    }

    if (childID == 0) { //in child
        if (execvp(args[0], args) == -1) {
            perror("error executing execvp");
        }
        exit(EXIT_FAILURE);
    } else { //in parent
        do {
            waitpid(childID, &status, WUNTRACED); //->  //https://stackoverflow.com/questions/35006850/waitpid-with-wuntraced  -> WUNTRACED

        } while (!WIFEXITED(status) && !WIFSIGNALED(status)); //TODO see how this part works?
    }
    return 1;
}


/* That’s what the exec() system call is all about.
 * It replaces the current running program with an entirely new one.
 * This means that when you call exec, the operating system stops your process,
 * loads up the new program, and starts that one in its place.
 * A process never returns from an exec() call (unless there’s an error).
 * */





