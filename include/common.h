#ifndef COMMON_H
#define COMMON_H

// POSIX headers
#include <sys/socket.h> // TODO: Reorganize dependencies
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

// C standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define IN_PORT 9002

#define SADDR struct sockaddr
#define SADDR_IN struct sockaddr_in

#define FILENAME(_Path) ((strrchr(_Path, '/') != NULL) ? strrchr(_Path, '/') + 1 : _Path)
#define MEM_ERROR(_ObjPtr, _Warning) ((_ObjPtr) == NULL) ? \
                              fprintf(stderr, "MEM ERROR(%s: %d): %s\n", FILENAME(__FILE__), __LINE__, (_Warning)), exit(1) : 0
#define ALLOC_ERR "Allocation error"
#define UNALLOC "Attempted to use unallocated object"
#define UTIL_CHECK(_Util, _ErrVal, _Warning) ((_Util) == (_ErrVal)) ? \
                                             perror((_Warning)), exit(_ErrVal) : 0
#define CLOSE_NOW(_Sock) printf("\nSIGINT detected: Exiting gracefully\n"); \
                         close((_Sock));

#define HANDLE_SIGINT(_Mask, _OrigMask) sigemptyset(&_Mask); \
                                        sigaddset(&_Mask, SIGINT); \
                                        signal(SIGINT, graceful_close); \
                                        sigprocmask(SIG_BLOCK, &_Mask, &_OrigMask)

#define DEFAULT_WAIT_TIME 3
#define DEFAULT_WAIT_TIME_U 0

#define HEARTBEAT_INTERVAL 100000

#define FRAME_SIZE 4

#define HEARTBEAT_HEADER "KEEP"
#define HEARTBEAT_FOOTER "BEAT"
#define CNN_ALIVE '\n'
#define CNN_DEAD '\0'
#define BEAT_SIZE 1

#define USERDATA_HEADER "USER"
#define USERDATA_FOOTER "ENDU"

#define FORCE_INLINE __attribute((always_inline)) inline

_Bool check_SIGINT(void);
void graceful_close(int signum);
void set_sock_timeout(int socket, int waitTime, int uwaitTime);

#endif