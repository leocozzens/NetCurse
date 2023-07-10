#include <model.h>

void init_queue(ActionQueue *userActions) {
    userActions->head = NULL;
    userActions->tail = NULL;
}

void enqueue(ActionQueue *userActions, UserData *data) {
    Action *newAction = malloc(sizeof(Action)); // TODO: Error check this malloc with preprocessor macro
    if(newAction == NULL) {
        fprintf(stderr, "MEM ERROR: allocation error\n");
        exit(1);
    }
    newAction->userPacket = data;
    newAction->nextAction = NULL;
    if(userActions->tail == NULL) userActions->head = newAction;
    else userActions->tail->nextAction = newAction;
    userActions->tail = newAction;
    print_queue(userActions);
}

void print_queue(ActionQueue *userActions) {
    if(userActions->head == NULL) return;
    Action *currentAction = userActions->head;
    printf("Messages:\n");
    while(currentAction != NULL) {
        printf("%s\n", currentAction->userPacket->msg);
        currentAction = currentAction->nextAction;
    }
}
// UserData *dequeue