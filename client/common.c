#include <string.h>

#include "common.h"

Command get_cmd_from_name(const char* name) {
    if (!strcmp(name, "join"))
        return CMD_JOIN;
    else if (!strcmp(name, "leave"))
        return CMD_LEAVE;
    else if (!strcmp(name, "history"))
        return CMD_HISTORY;
    else if (!strcmp(name, "send"))
        return CMD_SEND;
    else if (!strcmp(name, "help"))
        return CMD_HELP;
    else if (!strcmp(name, "clear"))
        return CMD_CLEAR;
    else if (!strcmp(name, "quit"))
        return CMD_QUIT;
    else if (!strcmp(name, "exit"))
        return CMD_EXIT;
    else
        return CMD_INVALID;
}
