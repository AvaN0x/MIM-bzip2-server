#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m2f.h"

void encodeM2F(char *S, char *shifts)
{
    int len = strlen(S);
    char alphabet[128];
    // Init alphabet to ASCII table
    for (int i = 0; i < 128; i++)
        alphabet[i] = (char)i;

    // Loop for each char in S
    for (int i = 0; i < len; i++)
    {
        shifts[i] = 0;
        // Seak for char in alphabet to get shift
        for (int j = 0; j < 128; j++)
        {
            // If char is found
            if (alphabet[j] == S[i])
            {
                shifts[i] = j;
                break;
            }
        }

        // If shift is above 0, then we need to bring it to the front
        if (shifts[i] > 0)
        {
            // Move every chars one index to the right from start to shift post
            for (int j = shifts[i] - 1; j >= 0; j--)
                alphabet[j + 1] = alphabet[j];

            // Set first char to seaked char
            alphabet[0] = S[i];
        }
    }
}

void decodeM2F(char *shifts, int len, char *result)
{
    char alphabet[128];
    // Init alphabet to ASCII table
    for (int i = 0; i < 128; i++)
        alphabet[i] = (char)i;

    char c;
    // Loop for each char in S
    for (int i = 0; i < len; i++)
    {
        // result[i] = 'a';
        c = alphabet[(int)shifts[i]];

        int shift = 0;
        //#region seak for shift
        // Seak for char in alphabet to get shift
        for (int j = 0; j < 128; j++)
        {
            // If char is found
            if (alphabet[j] == c)
            {
                shift = j;
                break;
            }
        }

        // If shift is above 0, then we need to bring it to the front
        if (shift > 0)
        {
            // Move every chars one index to the right from start to shift post
            for (int j = shift - 1; j >= 0; j--)
                alphabet[j + 1] = alphabet[j];

            // Set first char to seaked char
            alphabet[0] = c;
        }
        //#endregion seak for shift

        if (shifts[i] != shift)
            printf("ERROR\n");
        result[i] = c;
    }
    result[len] = '\0';
}

// int main(int argc, char const *argv[])
// {
//     // char *S = "PSSMIPISSII";
//     char *S = "mississippi";
//     int len = strlen(S);

//     char *shifts = (char *)calloc(len, sizeof(char));
//     encodeM2F(S, shifts);

//     printf("\033[0;33mENCODE\033[0m\n");
//     printf("%s\n", S);
//     for (int i = 0; i < len; i++)
//         printf("%c (%2d) = %d\n", S[i], i, shifts[i]);
//     for (int i = 0; i < len; i++)
//         printf("%c", shifts[i]);
//     printf("\n");

//     printf("\033[0;33mDECODE\033[0m\n");
//     char Sdecoded[len];
//     decodeM2F(shifts, len, Sdecoded);
//     printf("%s\n", Sdecoded);

//     free(shifts);
//     return EXIT_SUCCESS;
// }
