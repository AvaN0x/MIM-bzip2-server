#include <string.h>
#include "stream.h"

/**
 * Create a stream, initialize it, and return it
 * @return the stream
 */
stream_t create_stream()
{
    stream_t s;
    s.content = NULL;
    s.type = -1;

    return s;
}

/**
 * Reinitialize a stream with a type, and the content set to null
 * @param s the stream to reset
 * @param type the new type
 */
void init_stream(stream_t *s, uint8_t type)
{
    if (s->content != NULL) // free content allocation if not NULL
        free(s->content);

    s->content = NULL;
    s->type = type;
}

/**
 * Se the content of a stream
 * @param s the stream
 * @param content the new content
 */
void set_content(stream_t *s, void *content)
{
    if (s->content != NULL) // free content allocation if not NULL
        free(s->content);

    size_t len;
    switch (s->type)
    {
    // if content is an int
    case INT_CONTENT:
        s->content = malloc(sizeof(int8_t));
        memcpy(s->content, content, 1);
        break;

    // if content is a string
    case STRING_CONTENT:
    case SEND_FILE_NAME:
        len = strlen((char *)content);           // get the length of the string
        s->content = malloc(len * sizeof(char)); // allocate the memory for the string
        memcpy(s->content, content, len);        // copy content
        ((char *)s->content)[len] = '\0';        // set the last char as '\0' to end the string
        break;

    case NULL_CONTENT:
    case END_CONNECTION:
    case FILE_DO_NOT_EXIST:
    default:
        s->content = NULL;
    }
}

/**
 * Free the memory used by the content of a stream
 * @param s the stream
 */
void destroy_stream(stream_t *s)
{
    if (s->content != NULL) // free content allocation if not NULL
        free(s->content);
}

/**
 * Serialize a stream into a buffer
 * @param s the stream
 * @param buffer the buffer to fill in
 * @return the size of the buffer
 */
size_t serialize_stream(stream_t *s, void *buffer)
{
    *((uint8_t *)buffer) = s->type;
    buffer += sizeof(uint8_t); // move in the buffer of the size of the type

    size_t len;
    switch (s->type)
    {
    // if content is NULL
    case NULL_CONTENT:
    case END_CONNECTION:
    case FILE_DO_NOT_EXIST:
        return sizeof(uint8_t);

    // if content is an int
    case INT_CONTENT:
        memcpy(buffer, s->content, 1); // copy the int
        return sizeof(uint8_t) + sizeof(uint8_t);

    // if content is a string
    case STRING_CONTENT:
    case SEND_FILE_NAME:
        len = strlen((char *)s->content); // get the length of the string
        *((uint64_t *)buffer) = len;      // add the length to the buffer as int64_t
        buffer += sizeof(uint64_t);       // move in the buffer
        memcpy(buffer, s->content, len);  // copy the string
        return sizeof(uint8_t) + sizeof(uint64_t) + len;

    default:
        return 0;
    }
}

/**
 * Unserialize a buffer into a stream
 * @param buffer the buffer
 * @param s the stream to fill in
 */
void unserialize_stream(void *buffer, stream_t *s)
{
    init_stream(s, *((uint8_t *)buffer)); // re init the stream

    buffer += sizeof(uint8_t); // move in the buffer of the size of the type
    size_t len;
    switch (s->type)
    {
    // if content is an int
    case INT_CONTENT:
        s->content = malloc(sizeof(int8_t)); // allocate the size of an int
        memcpy(s->content, buffer, 1);       // copy the int
        break;

    // if content is a string
    case STRING_CONTENT:
    case SEND_FILE_NAME:
        len = *((uint64_t *)buffer);                   // get the length of the string
        buffer += sizeof(uint64_t);                    // move is the buffer
        s->content = malloc((len + 1) * sizeof(char)); // allocate the size of the string
        memcpy(s->content, buffer, len);               // copy content
        ((char *)s->content)[len] = '\0';              // set the last char as '\0' to end the string
        break;

    case NULL_CONTENT:
    case FILE_DO_NOT_EXIST:
    case END_CONNECTION:
    default:
        break;
    }
}