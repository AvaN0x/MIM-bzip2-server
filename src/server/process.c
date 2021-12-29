#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stddef.h>

#include "../shared/buffer.h"
#include "process.h"

#include "../shared/font_colors.h"
// #include "../shared/huffman/huff.h"
// #include "../shared/bwt/bwt.h"
// #include "../shared/m2f/m2f.h"
// #include "../shared/rle/rle.h"
#include "../shared/bzip2/bzip2.h"

/**
 * Open a file, split the content into chunks of maximum size BUFFER_SIZE, and send them to encodeDecodeBufferBZIP2
 *
 * @param filePath
 */
void processFile(char *filePath)
{
    FILE *file = fopen(filePath, "r");

    if (!file)
    {
        fprintf(stderr, "File not found\n");
        return;
    }

    printf("File content :\n");
    {
        char buffer[BUFFER_SIZE];
        int count = 0;
        char c;
        while ((c = fgetc(file)) != EOF)
        {
            // Only allow ascii chars
            if (c < 0 || c > 127)
                continue;

            buffer[count++] = c; // Use actual count value and increment it after

            if (count > (BUFFER_SIZE - 2))
            {
                buffer[count] = '\0';
                encodeDecodeBufferBZIP2(buffer, count);
                count = 0;
            }
        }
        if (count > 0)
        {
            buffer[count] = '\0';
            encodeDecodeBufferBZIP2(buffer, count);
            count = 0;
        }
        printf("\n");
    }

    fclose(file);
}

void encodeDecodeBufferBZIP2(char *buffer, int size)
{
    // int size = strlen(buffer);
    // Max value of size is BUFFER_SIZE - 1
    char S[size + 1];
    strcpy(S, buffer);
    // printf(FONT_YELLOW "%d: \"" FONT_DEFAULT "%s" FONT_YELLOW "\"\n" FONT_DEFAULT, size, S);

    // encode bzip2
    int charFrequences[128] = {};
    unsigned char *huffmanEncoded;
    int huffmanEncodedSize;
    int idxBWT = encodeBZIP2(S, size, charFrequences, &huffmanEncoded, &huffmanEncodedSize);
    // S will be freed by the function

    char *decodedString;
    int decodedStringSize;
    decodeBZIP2(huffmanEncoded, huffmanEncodedSize, idxBWT, charFrequences, &decodedString, &decodedStringSize);

    printf(FONT_YELLOW "BEFORE ENCODE " FONT_DEFAULT "\"%s\"\n", S);
    printf(FONT_YELLOW "AFTER  DECODE " FONT_DEFAULT "\"%s\"\n\n", decodedString);
    free(huffmanEncoded);
    free(decodedString);
}

// TODO I kept the other functions above while we are not done, but I think we will delete them
/**
 * Open a file, split the content into chunks of maximum size BUFFER_SIZE, and send them to encodeBufferAndSendToClient
 *
 * @param filePath
 * @param communicationID socket
 * @param stream
 * @param serStream buffer used to send data to the client
 */
void processFileForClient(char *filePath, int communicationID, stream_t *stream, char *serStream)
{
    FILE *file = fopen(filePath, "r");

    if (!file)
    {
        fprintf(stderr, "File not found\n");
        return;
    }

    printf("File content :\n");
    {
        char buffer[BUFFER_SIZE];
        int count = 0;
        char c;
        while ((c = fgetc(file)) != EOF)
        {
            // Only allow ascii chars
            if (c < 0 || c > 127)
                continue;

            buffer[count++] = c; // Use actual count value and increment it after

            if (count > (BUFFER_SIZE - 2))
            {
                buffer[count] = '\0';
                encodeBufferForClient(buffer, count, communicationID, stream, serStream);
                count = 0;
            }
        }
        if (count > 0)
        {
            buffer[count] = '\0';
            encodeBufferForClient(buffer, count, communicationID, stream, serStream);
            count = 0;
        }
        printf("\n");
    }

    fclose(file);
}

/**
 * Encode the buffer and send it to the client
 *
 * @param buffer original string
 * @param size size of the buffer
 * @param communicationID socket
 * @param stream
 * @param serStream buffer used to send data to the client
 */
void encodeBufferForClient(char *buffer, int size, int communicationID, stream_t *stream, char *serStream)
{
    size_t serStreamSize;

    // Max value of size is BUFFER_SIZE - 1
    char S[size + 1];
    strcpy(S, buffer);
    printf(FONT_YELLOW "%d: \"" FONT_DEFAULT "%s" FONT_YELLOW "\"\n\n" FONT_DEFAULT, size, S);

    // encode bzip2
    int charFrequences[128] = {};
    unsigned char *huffmanEncoded;
    int huffmanEncodedSize;
    int idxBWT = encodeBZIP2(S, size, charFrequences, &huffmanEncoded, &huffmanEncodedSize);
    // S will be freed by the function

    printf("(%d) \"", huffmanEncodedSize);
    for (int i = 0; i < huffmanEncodedSize; i++)
        printf("%u ", huffmanEncoded[i]);
    printf("\"\n");

    printf("Send idx (%d)\n", idxBWT);
    init_stream(stream, INT_CONTENT);
    set_content(stream, &idxBWT, 0);
    serStreamSize = serialize_stream(stream, serStream);
    send(communicationID, serStream, serStreamSize, 0); // send buffer to client
    printf("serStreamSize: %zu\n", serStreamSize);

    // for (int i = 0; i < 128; i++)
    // {
    //     if (charFrequences[i] > 0)
    //         printf("char(%d) %c : %d\n ", i, i, charFrequences[i]);
    // }

    printf("Send char frequences\n");
    init_stream(stream, SEND_CHAR_FREQUENCES);
    set_content(stream, charFrequences, 128);
    serStreamSize = serialize_stream(stream, serStream);
    send(communicationID, serStream, serStreamSize, 0); // send buffer to client
    printf("serStreamSize: %zu\n", serStreamSize);

    printf("Send huffman\n");
    init_stream(stream, SEND_GZIP2_STRING);
    set_content(stream, huffmanEncoded, huffmanEncodedSize);
    serStreamSize = serialize_stream(stream, serStream);
    send(communicationID, serStream, serStreamSize, 0); // send buffer to client
    printf("serStreamSize: %zu\n", serStreamSize);
}
