#include <stdbool.h>
#include <stddef.h>

#define CLIENTCMD(fn_name)                                                     \
    bool cmd_##fn_name(size_t argc, char** argv, void* ctx)

typedef bool (*Command_Fptr)(size_t argc, char** argv, void* ctx);

CLIENTCMD(join);
CLIENTCMD(leave);
CLIENTCMD(history);
CLIENTCMD(send);
CLIENTCMD(help);
CLIENTCMD(clear);
CLIENTCMD(quit);
