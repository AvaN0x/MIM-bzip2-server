#ifndef PROCESS_H
#define PROCESS_H

#include "../shared/stream.h"

void processFile(char *filePath);
void processFileForClient(char *filePath, int communicationID, stream_t *stream, char *serStream);

void encodeDecodeBufferBZIP2(char *buffer, int size);
void encodeBufferForClient(char *buffer, int size, int communicationID, stream_t *stream, char *serStream);

#endif
