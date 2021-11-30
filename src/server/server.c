#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "process.h"
#include "../shared/font_colors.h"
#include "../shared/huffman/huff.h"
#include "../shared/bwt/bwt.h"
#include "../shared/m2f/m2f.h"

int main(int argc, char const *argv[])
{
    // TODO when the full bzip2 encode and decode is working
    // if (0)
    {
        // processFile("lorem_ipsum.txt");
        processFile("mississippi.txt");
    }
    //----------------------//
    // Read content of file //
    //----------------------//
    if (0)
    {
        char *fileName = "mississippi.txt";

        char *tmpFileName = malloc(sizeof(char) * (strlen(fileName) + 4));
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
    // Test M2F //
    //----------//
    if (0)
    {
        char *S = "PSSMIPISSII";
        // char *S = "mississippi";
        int len = strlen(S);

        int *shifts = (int *)calloc(len, sizeof(int));
        encodeM2F(S, shifts);

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
    if (0)
    {
        // Execute Huffman
        char *fileName = "FreqCours.txt";

        char *tmpFileName = malloc(sizeof(char) * (strlen(fileName) + 4));
        strcpy(tmpFileName, "res/");
        strcat(tmpFileName, fileName);

        printf("path = %s\n", tmpFileName);
        FILE *encodedHuffman = encodeHuffman(tmpFileName);
        free(tmpFileName);
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
