#include <stdio.h>

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
    fprintf(stderr, "send not implemented\n");
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
