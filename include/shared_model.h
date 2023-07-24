#ifndef MODEL_STRUCTS_H
#define MODEL_STRUCTS_H

#define BUFF_SIZE 256
#define USERDATA_SIZE sizeof(UserData)

typedef struct {
    char msg[BUFF_SIZE]; // TODO: Find crossplatform standard data format
} UserData; // TODO: Allow for chained messages

#endif