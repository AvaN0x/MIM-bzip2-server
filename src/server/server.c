#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../shared/huffman/huff.h"
#include "../shared/bwt/bwt.h"
#include "../shared/m2f/m2f.h"

int main(int argc, char const *argv[])
{
    // TODO when the full bzip2 encode and decode is working
    //----------------------//
    // Read content of file //
    //----------------------//
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

    //----------//
    // Test BWT //
    //----------//

    {
        printf("\033[0;33mENCODE BWT\033[0m\n");

        char *S = "MISSISSIPPI";
        int len = strlen(S);

        int L[len];
        int idx = encodeBWT(S, L);
        char Sencoded[len + 1];

        printf("%s\n", S);
        printf("idx = %d\n", idx);

        for (int i = 0; i < len; i++)
        {
            Sencoded[i] = S[L[i]];
            // printf("L[%2d] = %2d (%c)\n", i, L[i], S[L[i]]);
        }
        Sencoded[len] = '\0';
        printf("%s\n", Sencoded);

        printf("\033[0;33mDECODE BWT\033[0m\n");
        char Sdecoded[len + 1];
        decodeBWT(Sencoded, len, idx, Sdecoded);
        printf("%s\n", Sdecoded);
    }

    //----------//
    // Test M2F //
    //----------//
    {
        char *S = "PSSMIPISSII";
        // char *S = "mississippi";
        int len = strlen(S);

        int *shifts = (int *)calloc(len, sizeof(int));
        encodeM2F(S, shifts);

        printf("\033[0;33mENCODE M2F\033[0m\n");
        printf("%s\n", S);
        for (int i = 0; i < len; i++)
            printf("%c (%2d) = %d\n", S[i], i, shifts[i]);

        printf("\033[0;33mDECODE M2F\033[0m\n");
        char Sdecoded[len];
        decodeM2F(shifts, len, Sdecoded);
        printf("%s\n", Sdecoded);

        free(shifts);
    }
    //--------------//
    // Test huffman //
    //--------------//
    {
        // Execute Huffman
        char *fileName2 = "FreqCours.txt";

        char *tmpFileName2 = malloc(sizeof(char) * (strlen(fileName2) + 4));
        strcpy(tmpFileName2, "res/");
        strcat(tmpFileName2, fileName2);

        // char * buffer = malloc(sizeof(char) * 20);
        // sprintf(buffer, "res/%s", "Freq.txt");
        printf("path = %s\n", tmpFileName2);
        encodeHuffman(tmpFileName2);
    }

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
    }

    return 0;
}
