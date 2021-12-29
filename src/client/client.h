#ifndef DEF_CLIENT
#define DEF_CLIENT

#include <stdbool.h>
#include "../shared/stream.h"

void connectedToServer(int);
void clearBuffer();
int promptString(char *, int);
int promptInt(char *, int, int, int);

bool askForFile(int, stream_t *, char *, char *);

#endif
