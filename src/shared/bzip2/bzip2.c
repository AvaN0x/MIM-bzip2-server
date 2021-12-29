#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../buffer.h"

#include "../font_colors.h"
#include "../huffman/huff.h"
#include "../bwt/bwt.h"
#include "../m2f/m2f.h"
#include "../rle/rle.h"

#define DEBUG_BZIP2 1

int encodeBZIP2(char *string, int size, int charFrequences[128], unsigned char **out, int *outSize)
{
    // Max value of size is BUFFER_SIZE - 1

#ifdef DEBUG_BZIP2
    printf(FONT_YELLOW "ENCODE BWT\n" FONT_DEFAULT);
#endif

    char str_bwt[size + 1];
    int idx = encodeBWT(string, str_bwt);
    // string will not be used anymore

#ifdef DEBUG_BZIP2
    printf("idx = %d\n", idx);

    printf("\"%s\"\n", str_bwt);

    printf(FONT_YELLOW "ENCODE M2F\n" FONT_DEFAULT);
#endif

    char *shifts = (char *)calloc(size, sizeof(char));
    encodeM2F(str_bwt, size, shifts);
    // str_bwt will not be used anymore

#ifdef DEBUG_BZIP2
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
#endif

    char *S_rle;
    int rle_len;

    encodeRLE(shifts, size, &S_rle, &rle_len);
    // shifts will not be used anymore
    free(shifts);
    shifts = NULL;

#ifdef DEBUG_BZIP2
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
#endif

    // Frequences array
    for (int i = 0; i < rle_len; i++)
        charFrequences[(int)S_rle[i]]++;

    char *HuffmanDico[128] = {};
    buildCodeHuffman(charFrequences, HuffmanDico);

#ifdef DEBUG_BZIP2
    for (int i = 0; i < 128; i++)
        if (HuffmanDico[i] != NULL)
            printf("%d|%c : %s\n", i, i, HuffmanDico[i]);
#endif

    *out = encodeHuffman(S_rle, rle_len, HuffmanDico, outSize);
    // S_rle will not be used anymore
    free(S_rle);
    S_rle = NULL;

#ifdef DEBUG_BZIP2
    printf("(%d) \"", *outSize);
    for (int i = 0; i < *outSize; i++)
        printf("%u ", (*out)[i]);
    printf("\"\n");
#endif

    // Free Huffman dictionary
    for (int i = 0; i < 128; i++)
    {
        if (HuffmanDico[i] != NULL)
        {
            free(HuffmanDico[i]);
            HuffmanDico[i] = NULL;
        }
    }

    return idx;
}

void decodeBZIP2(unsigned char *encodedHuffman, int encodedHuffmanSize, int idxBWT, int charFrequences[128], char **out, int *outSize)
{
    // Build Huffman dictionary
    char *HuffmanDico[128] = {};
    buildCodeHuffman(charFrequences, HuffmanDico);

#ifdef DEBUG_BZIP2
    for (int i = 0; i < 128; i++)
        if (HuffmanDico[i] != NULL)
            printf("%d|%c : %s\n", i, i, HuffmanDico[i]);

    printf(FONT_YELLOW "DECODE HUFFMAN\n" FONT_DEFAULT);
#endif

    char *decodedHuffman;
    int decodedHuffmanSize;
    decodedHuffman = decodeHuffman(encodedHuffman, encodedHuffmanSize, HuffmanDico, &decodedHuffmanSize);
    // encodedHuffman will not be used anymore

#ifdef DEBUG_BZIP2
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
#endif

    char *decodedRLE;

    decodeRLE(decodedHuffman, decodedHuffmanSize, &decodedRLE, outSize);
    // decodedHuffman will not be used anymore
    free(decodedHuffman);
    decodedHuffman = NULL;

#ifdef DEBUG_BZIP2
    printf("(%d) \"", *outSize);
    for (int i = 0; i < *outSize; i++)
    {
        if (decodedRLE[i] == 0)
            printf("\\0");
        else
            printf("%c", decodedRLE[i]);
    }
    printf("\"\n");

    printf(FONT_YELLOW "DECODE M2F\n" FONT_DEFAULT);
#endif

    char decodedM2F[*outSize];

    decodeM2F(decodedRLE, *outSize, decodedM2F);
    // decodedRLE will not be used anymore
    free(decodedRLE);
    decodedRLE = NULL;

#ifdef DEBUG_BZIP2
    printf("\"%s\"\n", decodedM2F);

    printf(FONT_YELLOW "DECODE BWT\n" FONT_DEFAULT);
#endif

    *out = malloc(*outSize + 1);
    decodeBWT(decodedM2F, *outSize, idxBWT, *out);

#ifdef DEBUG_BZIP2
    printf("\"%s\"\n", *out);
#endif

    // Free Huffman dictionary
    for (int i = 0; i < 128; i++)
    {
        if (HuffmanDico[i] != NULL)
        {
            free(HuffmanDico[i]);
            HuffmanDico[i] = NULL;
        }
    }
}
