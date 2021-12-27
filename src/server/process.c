#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../shared/buffer.h"
#include "process.h"

#include "../shared/font_colors.h"
#include "../shared/huffman/huff.h"
#include "../shared/bwt/bwt.h"
#include "../shared/m2f/m2f.h"
#include "../shared/rle/rle.h"

void processFile(char *fileName)
{
    char *tmpFileName = (char *)malloc(sizeof(char) * (strlen(fileName) + 4));
    strcpy(tmpFileName, "res/");
    strcat(tmpFileName, fileName);

    printf("tmpFileName: " FONT_MAGENTA "%s\n" FONT_DEFAULT, tmpFileName);
    FILE *file = fopen(tmpFileName, "r");
    free(tmpFileName);

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
        printf("%c", shifts[i]);
    printf("\"\n");

    printf(FONT_YELLOW "ENCODE RLE\n" FONT_DEFAULT);

    char *S_rle;
    int rle_len;

    encodeRLE(shifts, size, &S_rle, &rle_len);

    printf("(%d) \"", rle_len);
    for (int i = 0; i < rle_len; i++)
        printf("%c", S_rle[i]);
    printf("\"\n");

    // |
    // |
    // |
    printf("\n\n");
    // |
    // |
    // |
    // |

    printf(FONT_YELLOW "DECODE RLE\n" FONT_DEFAULT);

    char *S_decoded_rle;
    int rle_decoded_len;

    decodeRLE(S_rle, rle_len, &S_decoded_rle, &rle_decoded_len);
    printf("(%d) \"%s\"\n", rle_decoded_len, S_decoded_rle);

    printf(FONT_YELLOW "DECODE M2F\n" FONT_DEFAULT);
    char Sdecoded[rle_decoded_len];
    decodeM2F(S_decoded_rle, rle_decoded_len, Sdecoded);
    printf("\"%s\"\n", Sdecoded);

    // free(shifts);

    printf(FONT_YELLOW "DECODE BWT\n" FONT_DEFAULT);
    char bwtSdecoded[rle_decoded_len + 1];
    decodeBWT(Sdecoded, rle_decoded_len, idx, bwtSdecoded);
    printf("\"%s\"\n", bwtSdecoded);
}