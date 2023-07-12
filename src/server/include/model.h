#ifndef MODEL_H
#define MODEL_H

// Local headers
#include <model_structs.h>

void init_queue(ActionQueue *userActions);
void enqueue(ActionQueue *userActions, Action *newAction);
void print_queue(ActionQueue *userActions);
_Bool dequeue(ActionQueue *userActions, Action **retData);
void queue_cleanup(ActionQueue *userActions);

#endif