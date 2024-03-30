#define _POSIX_C_SOURCE 200809L

#include <MQTTClient.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cmds.h"
#include "common.h"
#include "packet.h"

#define BROKER_ADDR    "tcp://localhost:1883"
#define MAX_ROOMID_LEN 50 //

int mqtt_rc;

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
    strncat(res, name, MAX_ROOMID_LEN);

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

void send_msg(State* state, const char* txt) {
    time_t tm;
    time(&tm);

    Packet pk = Packet_new(state->client_name, tm, txt);
    const char* payload = Packet_to_json(&pk);

    state->curr_msg = (MQTTClient_message){
        .qos = 1,
        .retained = 0,
        .payload = (void*)payload,
        .payloadlen = strlen(payload),
    };

    MQTTFN(MQTTClient_publishMessage(state->client, state->room_id,
                                     &state->curr_msg,
                                     &state->msg_delivery_token),
           "failed to publish message");

    if ((mqtt_rc = MQTTClient_waitForCompletion(
             state->client, state->msg_delivery_token, 10000)) ==
        MQTTREASONCODE_MAXIMUM_CONNECT_TIME)
        goto end;
    else
        nanosleep(&(struct timespec){.tv_sec = 1}, NULL);

end:
    free((void*)payload);
}

void init(State* state) {
    state->connect_opts.keepAliveInterval = 20;
    state->connect_opts.cleansession = true;

    MQTTFN(MQTTClient_create(&state->client, BROKER_ADDR, state->client_name,
                             MQTTCLIENT_PERSISTENCE_NONE, NULL),
           "failed to create client");

    MQTTFN(MQTTClient_setCallbacks(state->client, NULL, on_connection_lost,
                                   on_new_msg, on_delivery),
           "failed to set callbacks");
    MQTTFN(MQTTClient_connect(state->client, &state->connect_opts),
           "failed to connect to the server");
}

void deinit(State* state) {
    MQTTFN(MQTTClient_disconnect(state->client, 10000),
           "failed to disconnect from the server");

    free(state->room_id);
}

size_t split_cmd(char*** res, const char* cmd) {
    char new_buf[strlen(cmd) + 1];
    size_t cmd_cap = 5;
    size_t cmd_argc = 0;
    char** cmd_argv = calloc(cmd_cap, sizeof(char*));

    if (cmd_argv == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    strcpy(new_buf, cmd);
    char* currtok = strtok(new_buf, " ");

    while (currtok != NULL) {
        char* new_part = malloc(strlen(currtok) + 1);

        if (new_part == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        strcpy(new_part, currtok);
        currtok = strtok(NULL, " ");

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

bool eval_cmd(State* state, const char* cmdtxt) {
    // join mychannel
    // leave
    // history
    // send
    // help

    char** cmd_argv = NULL;
    size_t cmd_argc = split_cmd(&cmd_argv, cmdtxt);
    Command cmd;

    if ((cmd = get_cmd_from_name(cmd_argv[0])) == CMD_INVALID) {
        puts("invalid command");
        goto end;
    }

    bool cmd_rval = CMD_FPTRS[cmd](cmd_argc, cmd_argv, state);
    if (cmd_rval)
        return true;

end:
    for (size_t i = 0; i < cmd_argc; i++)
        free(cmd_argv[i]);
    free(cmd_argv);
    return false;
}

int main(int argc, char** argv) {
    State state = {.connect_opts = MQTTClient_connectOptions_initializer,
                   .client_name = "!!DEFAULT CLIENT NAME!!",
                   .room_id = make_room_id(""),
                   .exe_path = argv[0]};

    char* msg;
    bool should_exit = false;

    if (argc == 1) {
        cmd_help(NULL, NULL, &state);
        exit(EXIT_FAILURE);
    } else {
        state.client_name = argv[1];
    }

    init(&state);

    while (!should_exit) {
        msg = read_msg();
        if (msg == NULL)
            continue;

        should_exit = eval_cmd(&state, msg);

        free(msg);
    }

    deinit(&state);

    return 0;
}
