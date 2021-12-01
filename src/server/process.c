#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "process.h"

#include "../shared/font_colors.h"
#include "../shared/huffman/huff.h"
#include "../shared/bwt/bwt.h"
#include "../shared/m2f/m2f.h"

#define BUFFERSIZE 512

void processFile(char *fileName)
{
    char *tmpFileName = malloc(sizeof(char) * (strlen(fileName) + 4));
    strcpy(tmpFileName, "res/");
    strcat(tmpFileName, fileName);

    printf("tmpFileName: %s\n", tmpFileName);
    FILE *file = fopen(tmpFileName, "r");
    free(tmpFileName);

    if (!file)
    {
        fprintf(stderr, "File not found\n");
        return;
    }

    printf("File content :\n");
    {
        char buffer[BUFFERSIZE];
        int count = 0;
        char c;
        while ((c = fgetc(file)) != EOF)
        {
            // Only allow ascii chars
            if (c < 0 || c > 127)
                continue;

            buffer[count++] = c; // Use actual count value and increment it after

            if (count > (BUFFERSIZE - 2))
            {
                buffer[count] = '\0';
                processBuffer(buffer, count);
                count = 0;
            }
        }
        if (count > 0)
        {
            buffer[count] = '\0';
            processBuffer(buffer, count);
            count = 0;
        }
        printf("\n");
    }

    fclose(file);
}

void processBuffer(char *buffer, int size)
{
    // int size = strlen(buffer);
    // Max value of size is BUFFERSIZE - 1
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
        printf("%c", shifts[i]);
    printf("\"\n");

    // |
    // |
    // |
    // |
    // |
    // |
    // |

    printf(FONT_YELLOW "DECODE M2F\n" FONT_DEFAULT);
    char Sdecoded[size];
    decodeM2F(shifts, size, Sdecoded);
    printf("\"%s\"\n", Sdecoded);

    free(shifts);

    printf(FONT_YELLOW "DECODE BWT\n" FONT_DEFAULT);
    char bwtSdecoded[size + 1];
    decodeBWT(Sdecoded, size, idx, bwtSdecoded);
    printf("\"%s\"\n", bwtSdecoded);
}