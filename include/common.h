#ifndef COMMON_H
#define COMMON_H

// POSIX headers
#include <sys/socket.h>
#include <arpa/inet.h>

// C standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define BUFF_SIZE 256
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
                         close((_Sock)); \
                         exit(0)

#define HANDLE_SIGINT signal(SIGINT, graceful_close);

void graceful_close(int signum);
_Bool check_SIGINT(void);

#endif