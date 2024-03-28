#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLEAR_TXT "\033[2J\033[0;0H"

char* pc_getline(void) {
    char* line = NULL;

    printf("> ");

    if (getline(&line, &(size_t){0}, stdin) == -1) {
        perror("getline");
        exit(EXIT_FAILURE);
    }

    if (line[0] == '\n')
        return NULL;

    if (line[strlen(line) - 1] != '\n')
        putchar('\n');
    else
        line[strlen(line) - 1] = '\0';

    return line;
}

bool pc_process_line(char* line) {
    if (!strcmp(line, "clear")) {
        printf(CLEAR_TXT);
        return true;
    }

    if (!strcmp(line, "exit") || !strcmp(line, "quit")) {
        exit(EXIT_SUCCESS);
        return true;
    }

    return false;
}

int main(void) {
    char* line;

    for (;;) {
        line = pc_getline();
        if (line == NULL)
            continue;

        if (!pc_process_line(line))
            printf("got: %s\n", line);

        free(line);
    }
}
