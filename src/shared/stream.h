#ifndef DEF_UTILS
#define DEF_UTILS

#include <stdint.h>
#include <stdlib.h>
#include "buffer.h"

#define PORT 6000

#define STREAM_SIZE BUFFER_SIZE + 128

typedef struct
{
    uint8_t type;
    void *content;
} stream_t;

enum
{
    // content : NULL
    NULL_CONTENT,
    END_CONNECTION,
    FILE_DO_NOT_EXIST,
    // content : int
    INT_CONTENT,
    // content : string
    STRING_CONTENT,
    SEND_FILE_NAME
};

stream_t create_stream();
void init_stream(stream_t *, uint8_t);
void set_content(stream_t *, void *);
void destroy_stream(stream_t *);

size_t serialize_stream(stream_t *, void *);
void unserialize_stream(void *, stream_t *);

#endif
