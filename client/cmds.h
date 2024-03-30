#pragma once

#include "common.h"
#include <stdbool.h>

CLIENTCMD(join);
CLIENTCMD(leave);
CLIENTCMD(history);
CLIENTCMD(send);
CLIENTCMD(help);
CLIENTCMD(clear);
CLIENTCMD(quit);
CLIENTCMD(exit);

static const Command_Fptr CMD_FPTRS[] = {
    [CMD_JOIN] = &cmd_join,       [CMD_LEAVE] = &cmd_leave,
    [CMD_HISTORY] = &cmd_history, [CMD_SEND] = &cmd_send,
    [CMD_HELP] = &cmd_help,       [CMD_CLEAR] = &cmd_clear,
    [CMD_QUIT] = &cmd_quit,       [CMD_EXIT] = &cmd_exit,
};
