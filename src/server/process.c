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
#include "../shared/huffman/huff.h"
#include "../shared/bwt/bwt.h"
#include "../shared/m2f/m2f.h"
#include "../shared/rle/rle.h"

#include "../shared/huffman/count.h"

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
    printf(FONT_YELLOW "%d: \"" FONT_DEFAULT "%s" FONT_YELLOW "\"\n\n" FONT_DEFAULT, size, S);

    printf(FONT_YELLOW "ENCODE BWT\n" FONT_DEFAULT);

    char L[size + 1];
    int idx = encodeBWT(S, L);

    printf("idx = %d\n", idx);

    printf("\"%s\"\n", L);

    char *shifts = (char *)calloc(size, sizeof(char));
    encodeM2F(L, size, shifts);

    printf(FONT_YELLOW "ENCODE M2F\n" FONT_DEFAULT);
    printf("\"");
    for (int i = 0; i < size; i++)
    {
        if (shifts[i] == 0)
            printf("\\0");
        else
            printf("%c", shifts[i]);
    }
    printf("\"\n");

    printf(FONT_YELLOW "ENCODE RLE\n" FONT_DEFAULT);

    char *S_rle;
    int rle_len;

    encodeRLE(shifts, size, &S_rle, &rle_len);

    printf("(%d) \"", rle_len);
    for (int i = 0; i < rle_len; i++)
    {
        if (S_rle[i] == 0)
            printf("\\0");
        else
            printf("%c", S_rle[i]);
    }
    printf("\"\n");

    printf(FONT_YELLOW "ENCODE HUFFMAN\n" FONT_DEFAULT);
    int dico[128] = {};
    char *HuffmanDico[128] = {};
    for (int i = 0; i < rle_len; i++)
        dico[(int)S_rle[i]]++;

    buildCodeHuffman(dico, HuffmanDico);
    // printf("GLOBAL COUNT IS %d\n", globalCounter);

    unsigned char *encodedHuffman;
    int encodedHuffmanSize;
    encodedHuffman = encodeHuffman(S_rle, rle_len, HuffmanDico, &encodedHuffmanSize);

    printf("(%d) \"", encodedHuffmanSize);
    for (int i = 0; i < encodedHuffmanSize; i++)
        printf("%u ", encodedHuffman[i]);
    printf("\"\n");

    printf("\n\n");

    printf(FONT_YELLOW "DECODE HUFFMAN\n" FONT_DEFAULT);
    char *decodedHuffman;
    int decodedHuffmanSize;
    decodedHuffman = decodeHuffman(encodedHuffman, encodedHuffmanSize, HuffmanDico, &decodedHuffmanSize);

    printf("(%d) \"", decodedHuffmanSize);
    for (int i = 0; i < decodedHuffmanSize; i++)
    {
        if (decodedHuffman[i] == 0)
            printf("\\0");
        else
            printf("%c", decodedHuffman[i]);
    }
    printf("\"\n");

    printf(FONT_YELLOW "DECODE RLE\n" FONT_DEFAULT);

    char *S_decoded_rle;
    int rle_decoded_len;

    decodeRLE(decodedHuffman, decodedHuffmanSize, &S_decoded_rle, &rle_decoded_len);
    printf("(%d) \"", rle_decoded_len);
    for (int i = 0; i < rle_decoded_len; i++)
    {
        if (S_decoded_rle[i] == 0)
            printf("\\0");
        else
            printf("%c", S_decoded_rle[i]);
    }
    printf("\"\n");

    printf(FONT_YELLOW "DECODE M2F\n" FONT_DEFAULT);
    char Sdecoded[rle_decoded_len];
    decodeM2F(S_decoded_rle, rle_decoded_len, Sdecoded);
    printf("\"%s\"\n", Sdecoded);

    // free(shifts);

    printf(FONT_YELLOW "DECODE BWT\n" FONT_DEFAULT);
    char bwtSdecoded[rle_decoded_len + 1];
    decodeBWT(Sdecoded, rle_decoded_len, idx, bwtSdecoded);
    printf("\"%s\"\n", bwtSdecoded);

    free(encodedHuffman);
    encodedHuffman = NULL;
    globalCounter--;

    free(decodedHuffman);
    decodedHuffman = NULL;
    globalCounter--;

    free(S_rle);
    S_rle = NULL;
    globalCounter--;

    free(S_decoded_rle);
    S_decoded_rle = NULL;
    globalCounter--;

    for (int i = 0; i < 128; i++)
        if (HuffmanDico[i] != NULL)
        {
            free(HuffmanDico[i]);
            HuffmanDico[i] = NULL;
            globalCounter--;
        }

    // printf("GLOBAL COUNT IS %d\n", globalCounter);
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
}
