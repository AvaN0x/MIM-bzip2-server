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

#include "../shared/huffman/count.h"

int main(int argc, char const *argv[])
{
    // TODO when the full bzip2 encode and decode is working
    if (0)
    {
        processFile("lorem_ipsum.txt");
        processFile("mississippi.txt");
    }
    //----------------------//
    // Read content of file //
    //----------------------//
    if (0)
    {
        char *fileName = "mississippi.txt";

        char *tmpFileName = (char *)malloc(sizeof(char) * (strlen(fileName) + 4));
        globalCounter++;
        strcpy(tmpFileName, "res/");
        strcat(tmpFileName, fileName);

        printf("tmpFileName: %s\n", tmpFileName);
        FILE *file = fopen(tmpFileName, "r");
        free(tmpFileName);

        if (!file)
        {
            fprintf(stderr, "File not found\n");
            exit(0); // TODO a bit too harsh, but it's a start
        }
        char c;

        printf("File content :\n");
        while ((c = fgetc(file)) != EOF)
        {
            printf("%c", c);
        }
        printf("\n");
        fclose(file);
    }
    printf("\n");

    //----------//
    // Test BWT //
    //----------//
    if (0)
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

    //----------//
    // Test RLE //
    //----------//
    if (0)
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

    //----------//
    // Test M2F //
    //----------//
    if (0)
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

    //--------------//
    // Test huffman //
    //--------------//
    // if (0)
    {
        printf(FONT_YELLOW "ENCODE HUFFMAN\n" FONT_DEFAULT);
        // const char *S = "exemple,de,codage,de,huffman";
        const char *S = "Bonjour je m'appelle Luc et je veux tester mes fonctions parce que sinon je ne sais pas si cela fonctionne correctment.";

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
        if (0)
        {
            for (int i = 0; i < 128; i++)
                if (HuffmanDico[i] != NULL)
                    printf("%d|%c : %s\n", i, i, HuffmanDico[i]);
        }

        // ------ ENCODE THE STRING ------ //
        unsigned char *encoded = encodeHuffman(S, HuffmanDico);

        // memory management debug
        // printf(FONT_YELLOW "AFTER ENCODE GLOBAL COUNT IS %d\n" FONT_DEFAULT, globalCounter);

        /*
         * If you want to see the hexa code, open powershell terminal and type:
         * format-hex path_to_file/huffmanEncoded.bin
         * file is in the res folder
         */
        // Print decimal values of encoded string
        printf(FONT_YELLOW "\nencoded is " FONT_DEFAULT);
        int lgthEncoded = strlen((char *)encoded);
        for (int i = 0; i < lgthEncoded; i++)
            printf("%u ", encoded[i]);
        printf("\n");

        // ------ DECODE THE STRING ------ //
        char *decoded = decodeHuffman(encoded, HuffmanDico);
        printf(FONT_YELLOW "\ndecoded is %s\n" FONT_DEFAULT, decoded);

        // ------ FREE MEMORY ------ //
        free(encoded);
        globalCounter--;
        encoded = NULL;

        free(decoded);
        globalCounter--;
        decoded = NULL;

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
        printf("Encoded string was occupying %ld bytes\n", lgthEncoded * sizeof(unsigned char));
        printf("Ration of compression is %f%%\n", (float)lgthEncoded / (float)len * 100);
    }

    if (0)
    {
        char test[] = "eéèe";
        printf("%s (len=%d)\n", test, (int)strlen(test));
        for (int i = 0; i < strlen(test); i++)
        {
            printf("%d: %c (%d)", i, test[i], test[i]);
            if (test[i] >= 0 && test[i] <= 127)
                printf(" and is valid");
            printf("\n");
        }
        printf("\n");
    }

    return 0;
}
