#pragma once

#include <MQTTClient.h>
#include <stdbool.h>
#include <time.h>

#define CLEAR_TXT           "\033[2J\033[0;0H"
#define MQTT_ERROR(comment) fprintf(stderr, comment " (code %d)\n", mqtt_rc)
#define MQTTFN(expr, error)                                                    \
    if ((mqtt_rc = expr) != MQTTCLIENT_SUCCESS) {                              \
        MQTT_ERROR(error);                                                     \
        exit(EXIT_FAILURE);                                                    \
    }
#define CLIENTCMD(fn_name)                                                     \
    bool cmd_##fn_name(size_t argc, char** argv, State* state)

typedef struct {
    MQTTClient client;
    MQTTClient_connectOptions
        connect_opts; // = MQTTClient_connectOptions_initializer;
    MQTTClient_message curr_msg;
    MQTTClient_deliveryToken msg_delivery_token;
    char* room_id;
    char* client_name;
    const char* exe_path;
} State;

typedef bool (*Command_Fptr)(size_t argc, char** argv, State* state);

typedef enum {
    CMD_JOIN,
    CMD_LEAVE,
    CMD_HISTORY,
    CMD_SEND,
    CMD_HELP,
    CMD_CLEAR,
    CMD_QUIT,
    CMD_EXIT,
    CMD_ROOM,
    CMD_INVALID,
} Command;

static int CMD_NARGS[] = {
    [CMD_JOIN] = 1, [CMD_LEAVE] = 0, [CMD_HISTORY] = 0,
    [CMD_SEND] = 1, [CMD_HELP] = 0,  [CMD_CLEAR] = 0,
    [CMD_QUIT] = 0, [CMD_EXIT] = 0,  [CMD_ROOM] = 2,
};

static const char* CMD_NAMES[] = {
    [CMD_JOIN] = "join",       [CMD_LEAVE] = "leave",
    [CMD_HISTORY] = "history", [CMD_SEND] = "send",
    [CMD_HELP] = "help",       [CMD_CLEAR] = "clear",
    [CMD_EXIT] = "exit",       [CMD_QUIT] = "quit",
    [CMD_ROOM] = "room",       [CMD_INVALID] = "!! INVALID COMMAND !!",
};

Command get_cmd_from_name(const char* name);
void send_msg(State* state, const char* txt);
void recv_msg(char* topic_name, size_t topic_len, const char* payload,
              size_t payload_len);

char* make_room_id(const char* name);
char* read_cmd(State* state);
