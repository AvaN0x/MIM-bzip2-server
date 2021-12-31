#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "process.h"
#include "../shared/font_colors.h"
#include "../shared/bwt/bwt.h"
#include "../shared/m2f/m2f.h"
#include "../shared/rle/rle.h"
#include "../shared/huffman/huff.h"

#include "../shared/huffman/count.h" // FIXME: remove this

void testProcessFile()
{
    processFile("res/mississippi.txt");
    processFile("res/Text_de_base.txt");
}

void testRLE()
{
    // char *S = "mississippi";
    // char *S = "122333444455555666666777777788888888999999999";
    char *S = "misssssssssssssisssssipppi";

    int len = strlen(S);

    char *Sencoded;
    int encodedLen;

    printf("(%d) %s\n", len, S);
    printf("\033[0;33mENCODE\033[0m\n");
    encodeRLE(S, len, &Sencoded, &encodedLen);

    printf("(%d) ", encodedLen);
    for (int i = 0; i < encodedLen; i++)
        printf("%c", Sencoded[i]);
    printf("\n");

    printf("\033[0;33mDECODE\033[0m\n");
    char *Sdecoded;
    int decodedLen;

    decodeRLE(Sencoded, encodedLen, &Sdecoded, &decodedLen);
    printf("(%d) %s\n", decodedLen, Sdecoded);
}

void testM2F()
{
    char *S = "PSSMIPISSII";
    // char *S = "mississippi";
    int len = strlen(S);

    char *shifts = (char *)calloc(len, sizeof(char));
    encodeM2F(S, len, shifts);

    printf(FONT_YELLOW "ENCODE M2F\n" FONT_DEFAULT);
    printf("%s\n", S);
    for (int i = 0; i < len; i++)
        printf("%c (%2d) = %2d '%c'\n", S[i], i, shifts[i], shifts[i]);

    printf(FONT_YELLOW "DECODE M2F\n" FONT_DEFAULT);
    char Sdecoded[len];
    decodeM2F(shifts, len, Sdecoded);
    printf("%s\n", Sdecoded);

    free(shifts);
    printf("\n");
}

void testHuffman()
{
    printf(FONT_YELLOW "ENCODE HUFFMAN\n" FONT_DEFAULT);
    // const char *S = "exemple,de,codage,de,huffman";
    // const char *S = "Bonjour je m'appelle Luc et je veux tester mes fonctions parce que sinon je ne sais pas si cela fonctionne correctment.";
    const char *S = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras sed mauris non augue accumsan venenatis. Vivamus maximus luctus erat non vestibulum. Interdum et malesuada fames ac ante ipsum primis in faucibus. In vitae arcu congue, vehicula nulla nec, porta odio. Donec tempus nisi sed massa tempor, ut convallis lectus tempor. In lectus mauris, sollicitudin sed orci ut, eleifend faucibus diam. In condimentum dolor nec mauris pharetra tempor. In faucibus scelerisque eros eget bibendum. Maecenas pretium cursus";
    int dico[128] = {};
    char *HuffmanDico[128] = {};

    // ------ GET THE FREQUENCY OF EACH CARACTER ------ //
    int len = strlen(S);
    for (int i = 0; i < len; i++)
        dico[(int)S[i]]++;

    // ------ BUILD THE ODE FOR EACH CARACTER WHERE FREQ > 0 ------ //
    buildCodeHuffman(dico, HuffmanDico);

    // memory management debug
    // printf(FONT_YELLOW "AFTER BUILD HUFFMAN CODE GLOBAL COUNT IS %d\n" FONT_DEFAULT, globalCounter);

    // Print non null codes created
    printf("Huffman dictionnary\n");
    for (int i = 0; i < 128; i++)
        if (HuffmanDico[i] != NULL)
            printf("%d|%c : %s\n", i, i, HuffmanDico[i]);

    // ------ ENCODE THE STRING ------ //
    unsigned char *encodedHuffman;
    int encodedHuffmanSize;
    encodedHuffman = encodeHuffman(S, len, HuffmanDico, &encodedHuffmanSize);

    // memory management debug
    // printf(FONT_YELLOW "AFTER ENCODE GLOBAL COUNT IS %d\n" FONT_DEFAULT, globalCounter);

    /*
     * If you want to see the hexa code, open powershell terminal and type:
     * format-hex path_to_file/huffmanEncoded.bin
     * file is in the res folder
     */

    // Print decimal values of encoded string
    printf(FONT_YELLOW "\nencoded is \n" FONT_DEFAULT);
    for (int i = 0; i < encodedHuffmanSize; i++)
        printf("%u ", encodedHuffman[i]);
    printf("\n");

    // ------ DECODE THE STRING ------ //
    char *decodedHuffman;
    int decodedHuffmanSize;
    decodedHuffman = decodeHuffman(encodedHuffman, encodedHuffmanSize, HuffmanDico, &decodedHuffmanSize);
    printf(FONT_YELLOW "\ndecoded is \n" FONT_DEFAULT);
    for (int i = 0; i < decodedHuffmanSize; i++)
        printf("%c", decodedHuffman[i]);
    printf("\n");

    // ------ FREE MEMORY ------ //
    free(encodedHuffman);
    globalCounter--;
    encodedHuffman = NULL;

    free(decodedHuffman);
    globalCounter--;
    decodedHuffman = NULL;

    // Free codes in HuffmanDico
    for (int i = 0; i < 128; i++)
    {
        if (HuffmanDico[i] != NULL)
        {
            free(HuffmanDico[i]);
            globalCounter--;
            HuffmanDico[i] = NULL;
        }
    }

    // memory management debug
    // printf(FONT_YELLOW "AFTER DECODE GLOBAL COUNT IS %d\n" FONT_DEFAULT, globalCounter);

    // Stats
    printf(FONT_YELLOW "\nStats\n" FONT_DEFAULT);
    printf("Initial string was occupying %ld bytes\n", len * sizeof(char));
    printf("Encoded string was occupying %ld bytes\n", encodedHuffmanSize * sizeof(unsigned char));
    printf("Ratio of compression is %d%%\n", 100 * encodedHuffmanSize / len);
}

void testBWT()
{
    printf(FONT_YELLOW "ENCODE BWT\n" FONT_DEFAULT);

    //! Don't use char that are not in the ASCII table !!
    char *S = "MISSISSIPPI";
    int len = strlen(S);

    char L[len + 1];
    int idx = encodeBWT(S, L);

    printf("%s\n", S);
    printf("idx = %d\n", idx);

    printf("%s\n", L);

    printf(FONT_YELLOW "DECODE BWT\n" FONT_DEFAULT);
    char Sdecoded[len + 1];
    decodeBWT(L, len, idx, Sdecoded);
    printf("%s\n", Sdecoded);
    printf("\n");
}