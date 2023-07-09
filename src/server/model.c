#include <model.h>

void init_queue(ActionQueue *userActions) {
    userActions->head = NULL;
    userActions->tail = NULL;
}

// void enqueue(ActionQueue *useraActions, UserData *data) {

// }
// UserData *dequeue