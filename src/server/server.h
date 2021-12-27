#ifndef DEF_SERVER
#define DEF_SERVER

#include <netinet/in.h>

typedef struct
{
    int communicationID;
    struct sockaddr_in connectedAddr;
} connectionStruct;

void *connectionThread(void *);
void clientConnected(int);
void disconnectUser(int, stream_t *, char *);
int sendString(int, stream_t *, char *, char *, bool, const char *, ...);
void promptUser(int, stream_t *, char *, int);

#endif
