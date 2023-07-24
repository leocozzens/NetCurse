#ifndef MODEL_STRUCTS_H
#define MODEL_STRUCTS_H

#define BUFF_SIZE 256
#define USERDATA_SIZE sizeof(UserData)

typedef struct {
    char msg[BUFF_SIZE]; // WARNING: When adding additional multi-byte primitives ensure correct byte order
} __attribute__((packed)) UserData; // TODO: Allow for chained messages

#endif