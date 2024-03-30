#pragma once

#include <bits/types/time_t.h>

typedef struct {
    const char* author;
    time_t time;
    const char* msg;
} Packet;

Packet Packet_new(const char* author, time_t time, const char* msg);
Packet Packet_from_json(const char* json_str);
const char* Packet_to_json(Packet* pk);
void Packet_free(Packet* pk);
