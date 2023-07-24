// POSIX headers
#include <pthread.h>

// Local headers
#include <model.h>

void init_queue(ActionQueue *userActions) {
    userActions->head = NULL;
    userActions->tail = NULL;
    userActions->queueLock = malloc(sizeof(pthread_mutex_t));
    userActions->queueReady = malloc(sizeof(sem_t));
    pthread_mutex_init(userActions->queueLock, NULL);
    sem_init(userActions->queueReady, 0, 0);
}

void enqueue(ActionQueue *userActions, Action *newAction) {
    MEM_ERROR(userActions, UNALLOC);
    MEM_ERROR(newAction, UNALLOC);
    newAction = newAction;
    newAction->nextAction = NULL;
    pthread_mutex_lock(userActions->queueLock);
    if(userActions->head == NULL) userActions->head = newAction; // Head is checked instead of tail because dequeue does not sync the tail if the head is null
    else userActions->tail->nextAction = newAction;
    userActions->tail = newAction;
    pthread_mutex_unlock(userActions->queueLock);
    sem_post(userActions->queueReady);
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

void dequeue(ActionQueue *userActions, Action **retAction) {
    MEM_ERROR(userActions, UNALLOC);
    MEM_ERROR(retAction, UNALLOC);
    if(userActions->head == NULL) sem_wait(userActions->queueReady);
    pthread_mutex_lock(userActions->queueLock);
    *retAction= userActions->head;

    userActions->head = userActions->head->nextAction;
    if(userActions->head == NULL) userActions->tail = NULL;
    pthread_mutex_unlock(userActions->queueLock);
}