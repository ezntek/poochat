#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmds.h"
#include "common.h"

CLIENTCMD(join) {
    fprintf(stderr, "join not implemented\n");
    return false;
}

CLIENTCMD(leave) {
    fprintf(stderr, "leave not implemented\n");
    return false;
}

CLIENTCMD(history) {
    fprintf(stderr, "history not implemented\n");
    return false;
}

CLIENTCMD(send) {
    char* msg = NULL;
    size_t msg_cap = 1;

    argc--;
    argv++;
    if (argc < CMD_NARGS[CMD_SEND]) {
        fprintf(stderr, "not enough args");
        goto end;
    }

    if (state->room_id == NULL) {
        fprintf(stderr, "must supply room id first!");
        goto end;
    }

    for (size_t i = 0; i < argc; i++)
        msg_cap += strlen(argv[i]) + 1; // account for one space character

    msg = calloc(msg_cap, 1);

    if (msg == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < argc; i++) {
        strcat(msg, argv[i]);
        strcat(msg, " ");
    }
    msg[msg_cap - 2] = '\0'; // delete trailing space

    send_msg(state, msg);
    free(msg);

end:
    return false;
}

CLIENTCMD(room) {
    // usage: room <join/leave/get> [room name]
    argc--;
    argv++;

    if (argc == 0) {
        fprintf(stderr, "must supply a subcommand to `room`!\n");
        goto printhelp;
    }

    const char* subcmd = argv[0];

    if (!strcmp(subcmd, "join")) {
        if (state->room_id != NULL) {
            fprintf(stderr, "already joined room `%s`\n", state->room_id);
            return false;
        }

        state->room_id = malloc(strlen(argv[1]));

        if (state->room_id == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        strcpy(state->room_id, argv[1]);
    } else if (!strcmp(subcmd, "leave")) {
        free(state->room_id);
        state->room_id = NULL;
    } else if (!strcmp(subcmd, "get")) {
        if (state->room_id == NULL)
            puts("no room");
        else
            puts(state->room_id);
    } else {
        fprintf(stderr, "invalid subcommand \"%s\"\n", subcmd);
        goto printhelp;
    }

    return false;
printhelp:
    puts("usage: room <join/leave/get> [room name]");
    return false;
}

CLIENTCMD(help) {
    puts("no bold support yet :(");
    printf("usage: %s [client name]\n", state->exe_path);
    return false;
}

CLIENTCMD(clear) {
    printf(CLEAR_TXT);
    return false;
}

CLIENTCMD(quit) {
    puts("Bye");
    return true;
}

CLIENTCMD(exit) {
    return cmd_quit(NULL, NULL, NULL); // this is fine...
}
