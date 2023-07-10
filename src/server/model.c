#include <model.h>

void init_queue(ActionQueue *userActions) {
    userActions->head = NULL;
    userActions->tail = NULL;
    userActions->tailLock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(userActions->tailLock, NULL);
}

void enqueue(ActionQueue *userActions, Action *newAction) {
    MEM_ERROR(userActions, UNALLOC);
    MEM_ERROR(newAction, UNALLOC);
    newAction = newAction;
    newAction->nextAction = NULL;
    if(userActions->head == NULL) userActions->head = newAction; // Head is checked instead of tail because dequeue does not sync the tail if the head is null
    else userActions->tail->nextAction = newAction;
    userActions->tail = newAction;
}

void print_queue(ActionQueue *userActions) {
    if(userActions->head == NULL) return;
    Action *currentAction = userActions->head;
    printf("Messages:\n");
    while(currentAction != NULL) {
        printf("%s\n", currentAction->userPacket.msg);
        currentAction = currentAction->nextAction;
    }
}

_Bool dequeue(ActionQueue *userActions, Action **retAction) {
    MEM_ERROR(userActions, UNALLOC);
    MEM_ERROR(retAction, UNALLOC);
    if(userActions->head == NULL) return 0;
    *retAction= userActions->head;

    userActions->head = userActions->head->nextAction;
    if(userActions->head == NULL) userActions->tail = NULL;
    return 1;
}

void queue_cleanup(ActionQueue *userActions) {
    Action *currAction = userActions->head;
    Action *nextAction;
    while(currAction != NULL) {
        nextAction = currAction->nextAction;
        free(currAction);
        currAction = nextAction;
    }
    free(currAction);
    pthread_mutex_destroy(userActions->tailLock);
    free(userActions->tailLock);
}