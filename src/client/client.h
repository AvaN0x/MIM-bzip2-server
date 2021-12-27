#ifndef DEF_CLIENT
#define DEF_CLIENT

#include "../shared/stream.h"

void connectedToServer(int);
void clearBuffer();
int promptString(char *, int);
int promptInt(char *, int, int, int);

void askForFile(int, stream_t *, char *, char *);

#endif
