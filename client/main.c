#include <MQTTClient.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmds.h"

#define CLEAR_TXT           "\033[2J\033[0;0H"
#define MQTT_ERROR(comment) fprintf(stderr, comment "%d\n", mqtt_rc)
#define MQTTFN(expr, error)                                                    \
    if ((mqtt_rc = expr) != MQTTCLIENT_SUCCESS) {                              \
        MQTT_ERROR(error);                                                     \
        exit(EXIT_FAILURE);                                                    \
    }
#define BROKER_ADDR    "tcp://localhost:1883"
#define MAX_ROOMID_LEN 50 //

typedef struct {
    MQTTClient client;
    MQTTClient_connectOptions
        connect_opts; // = MQTTClient_connectOptions_initializer;
    char* room_id;
    char* client_name;
} State;

typedef enum {
    CMD_JOIN,
    CMD_LEAVE,
    CMD_HISTORY,
    CMD_SEND,
    CMD_HELP,
    CMD_CLEAR,
    CMD_QUIT,
} Command;

static int CMD_NARGS[] = {
    [CMD_JOIN] = 1, [CMD_LEAVE] = 0, [CMD_HISTORY] = 0, [CMD_SEND] = 1,
    [CMD_HELP] = 0, [CMD_CLEAR] = 0, [CMD_QUIT] = 0,
};

static const char* CMD_NAMES[] = {
    [CMD_JOIN] = "join", [CMD_LEAVE] = "leave", [CMD_HISTORY] = "history",
    [CMD_SEND] = "send", [CMD_HELP] = "help",   [CMD_CLEAR] = "clear",
    [CMD_QUIT] = "quit",
};

static const Command_Fptr CMD_FPTRS[] = {
    [CMD_JOIN] = &cmd_join,       [CMD_LEAVE] = &cmd_leave,
    [CMD_HISTORY] = &cmd_history, [CMD_SEND] = &cmd_send,
    [CMD_HELP] = &cmd_help,       [CMD_CLEAR] = &cmd_clear,
    [CMD_QUIT] = &cmd_quit,
};

int mqtt_rc;
State state;

char* make_room_id(const char* name) {
    char* res = calloc(MAX_ROOMID_LEN + sizeof("poochat/"), 1);

    if (res == NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }

    if (strlen(name) > MAX_ROOMID_LEN)
        fprintf(stderr, "room name too long, truncating to %d characters\n",
                MAX_ROOMID_LEN);

    strcpy(res, "poochat/");
    strlcat(res, name, MAX_ROOMID_LEN);

    return res;
}

char* read_msg(void) {
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

void on_delivery(void* _, MQTTClient_deliveryToken deliver_tok) {
    fprintf(stderr, "message delivered with token %d", deliver_tok);
}

void on_connection_lost(void* _, char* cause) {
    fprintf(stderr, "connection to the broker was lost (cause: %s)\n", cause);

    MQTTClient_free(cause);
}

int on_new_msg(void* _, char* topic_name, int topic_len,
               MQTTClient_message* msg) {
    char* data = msg->payload;
    fprintf(stderr, "got some data! (\"%s\" from topic \"%s\")", data,
            topic_name);

    MQTTClient_freeMessage(&msg);
    MQTTClient_free(topic_name);
    return 1;
}

void init(void) {
    MQTTFN(MQTTClient_create(&state.client, BROKER_ADDR, state.client_name,
                             MQTTCLIENT_PERSISTENCE_NONE, NULL),
           "failed to create client");

    MQTTFN(MQTTClient_setCallbacks(state.client, NULL, on_connection_lost,
                                   on_new_msg, on_delivery),
           "failed to set callbacks");
    MQTTFN(MQTTClient_connect(state.client, &state.connect_opts),
           "failed to connect to the server");
}

void deinit(void) {
    MQTTFN(MQTTClient_disconnect(state.client, 10000),
           "failed to disconnect from the server");

    free(state.room_id);
}

size_t split_cmd(char*** res, const char* cmd) {
    char new_buf[strlen(cmd) + 1];
    size_t cmd_cap = 5;
    size_t cmd_argc = 0;
    char** cmd_argv = calloc(cmd_cap, sizeof(char*));

    strcpy(new_buf, cmd);
    char* currtok = strtok(new_buf, " ");

    while (currtok != NULL) {
        currtok = strtok(NULL, " ");
        char* new_part = malloc(strlen(currtok) + 1);

        if (new_part == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        strcpy(new_part, currtok);

        if (cmd_argc + 1 > cmd_cap) {
            cmd_argv = realloc(cmd_argv, sizeof(char*) * (cmd_cap *= 5));

            if (cmd_argv == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        cmd_argv[cmd_argc++] = new_part;
    }

    *res = cmd_argv;
    return cmd_argc;
}

bool eval_cmd(const char* cmd) {
    // join mychannel
    // leave
    // history
    // send
    // help

    char** cmd_argv = NULL;
    size_t cmd_argc = split_cmd(&cmd_argv, cmd);

    if (!strcmp(cmd_argv[0], CMD_NAMES[CMD_CLEAR])) {
        printf(CLEAR_TXT);
    } else if (!strcmp(cmd_argv[0], CMD_NAMES[CMD_QUIT])) {
        return true;
    }

    free(cmd_argv);
    return false;
}

int main(int argc, char** argv) {
    state = (State){.connect_opts = MQTTClient_connectOptions_initializer,
                    .client_name = "!!DEFAULT CLIENT NAME!!",
                    .room_id = make_room_id("")};

    char* msg;
    bool should_exit = false;

    if (argc == 1) {
        printf("usage: poochat [Client ID]");
        exit(EXIT_FAILURE);
    } else {
        state.client_name = argv[1];
    }

    init();

    while (!should_exit) {
        msg = read_msg();
        if (msg == NULL)
            continue;

        should_exit = eval_cmd(msg);

        free(msg);
    }

    deinit();

    return 0;
}
