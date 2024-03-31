#define _POSIX_C_SOURCE 200809L

#include <MQTTClient.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "packet.h"

#define MAX_ROOMID_LEN 50 //

extern int mqtt_rc;

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
    else if (!strcmp(name, "room"))
        return CMD_ROOM;
    else
        return CMD_INVALID;
}

void send_msg(State* state, const char* txt) {
    time_t tm;
    time(&tm);

    Packet pk = Packet_new(state->client_name, tm, txt);
    const char* payload = Packet_to_json(&pk);

    state->curr_msg.qos = 1;
    state->curr_msg.retained = 0;
    state->curr_msg.payload = (void*)payload;
    state->curr_msg.payloadlen = strlen(payload);

    MQTTFN(
        MQTTClient_publishMessage(state->client, make_room_id(state->room_id),
                                  &state->curr_msg, &state->msg_delivery_token),
        "failed to publish message");

    if ((mqtt_rc = MQTTClient_waitForCompletion(
             state->client, state->msg_delivery_token, 10000)) ==
        MQTTREASONCODE_MAXIMUM_CONNECT_TIME)
        fprintf(stderr, "failed to deliver message due to timeout");

    free((void*)payload);
}

void recv_msg(char* topic_name, size_t topic_len, const char* payload,
              size_t payload_len) {
    fprintf(stderr, "got message \"%s\" from topic \"%s\"\n", payload,
            topic_name);
}

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

char* read_cmd(State* state) {
    char* line = NULL;
    char* room = state->room_id;

    if (room == NULL)
        room = "no room";
    printf("%s> ", room);

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
