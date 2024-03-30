#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <json-c/json.h>

#include "packet.h"

Packet Packet_new(const char* author, time_t time, const char* msg) {
    return (Packet){author, time, msg};
}

Packet Packet_from_json(const char* json_str) {
    // { "author": "goofy", "time": "1711804070", "msg": "ur an idiot lmfao" }
    Packet res = {0};

    json_object* author;
    json_object* time;
    json_object* msg;
    const char* author_js;
    const char* msg_js;
    char* author_s = NULL;
    char* msg_s = NULL;

    json_tokener* tok = json_tokener_new();
    json_object* root = json_tokener_parse_ex(tok, json_str, strlen(json_str));

    if ((author = json_object_object_get(root, "author")) == NULL) {
        fprintf(stderr,
                "error whilst getting json field \"author\" from packet\n");
    }

    if ((time = json_object_object_get(root, "time")) == NULL) {
        fprintf(stderr,
                "error whilst getting json field \"time\" from packet\n");
    }

    if ((msg = json_object_object_get(root, "msg")) == NULL) {
        fprintf(stderr,
                "error whilst getting json field \"msg\" from packet\n");
    }

    res.time = (time_t)json_object_get_int64(time);
    author_js = json_object_get_string(author);
    msg_js = json_object_get_string(msg);

    if ((author_s = malloc(strlen(author_js) + 1)) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if ((msg_s = malloc(strlen(msg_js) + 1)) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(author_s, author_js);
    strcpy(msg_s, msg_js);

    res.author = author_s;
    res.msg = msg_s;

    json_tokener_free(tok);
    json_object_put(root);

    return res;
}

const char* Packet_to_json(Packet* pk) {
    json_object* root = json_object_new_object();
    char* res = NULL;

    if (root == NULL) {
        fprintf(stderr, "failed to create a new json object");
        exit(EXIT_FAILURE);
    }

    json_object_object_add(root, "author", json_object_new_string(pk->author));
    json_object_object_add(root, "msg", json_object_new_string(pk->msg));
    json_object_object_add(root, "time", json_object_new_int64(pk->time));

    const char* res_j =
        json_object_to_json_string_ext(root, JSON_C_TO_STRING_PLAIN);

    if ((res = malloc(strlen(res_j) + 1)) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(res, res_j);
    json_object_put(root);

    return res;
}

void Packet_free(Packet* pk) {
    free((void*)pk->author);
    free((void*)pk->msg);
}
