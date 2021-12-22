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
        // tmp TODO RLE
        int dico[128] = {};
        for (int i = 0; i < 128; i++)
        {
            switch (i)
            {
            case 0:
                dico[i] = 3;
                break;
            case 1:
                dico[i] = 2;
                break;
            case 3:
            case 76:
            case 79:
            case 80:
            case 83:
                dico[i] = 1;
                break;
            }
        }

        char *HuffmanDico[128] = {};
        // Build the code for each caracter where freq > 0
        buildCodeHuffman(dico, HuffmanDico);

        // Print values
        // printf("in server\n");
        // for (int i = 0; i < 128; i++)
        //     if (HuffmanDico[i] != NULL)
        //         printf("%d|%c : %s\n", i, i, HuffmanDico[i]);

        unsigned char *encoded = encodeHuffman("OPLS", HuffmanDico);

        printf(FONT_YELLOW "\nencoded is " FONT_DEFAULT);
        for (int i = 0; i < strlen((char *)encoded); i++)
            printf("%u ", encoded[i]);
        printf("\n");

        char *decoded = decodeHuffman(encoded, HuffmanDico);
        printf(FONT_YELLOW "\ndecoded is %s\n" FONT_DEFAULT, decoded);

        free(encoded);
        encoded = NULL;
        free(decoded);
        decoded = NULL;
        for (int i = 0; i < 128; i++)
        {
            free(HuffmanDico[i]);
            HuffmanDico[i] = NULL;
        }
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
