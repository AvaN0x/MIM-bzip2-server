#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include "bwt.h"

int inc(int i, int max)
{
    return (i + 1) % max;
}

int dec(int i, int max)
{
    return i == 0 ? (max - 1) : (i - 1);
}

int traiterSousChaines(char *S, int k, int j)
{
    int len = strlen(S);

    int ij = inc(j, len);
    int ik = inc(k, len);
    while (S[ij] == S[ik])
    {
        ij = inc(ij, len);
        ik = inc(ik, len);
    }

    return (S[ij] < S[ik]) ? j : k;
}

int encodeBWT(char *S, char *L)
{
    int idx, j, k;
    int len = strlen(S);
    bool M[len];
    for (int i = 0; i < len; i++)
        M[i] = false;

    for (int i = 0; i < len; i++)
    {
        k = i - 1;
        do
        {
            k = inc(k, len);
        } while (M[k]);

        j = inc(k, len);

        while (j != i)
        {
            if (!M[j] && S[j] < S[k])
                k = j;
            else if (!M[j] && S[j] == S[k])
                k = traiterSousChaines(S, k, j);
            j = inc(j, len);
        }

        L[i] = S[dec(k, len)];

        if (k == 0)
            idx = i;

        M[k] = true;
    }
    L[len] = '\0';
    return idx;
}

int qsortString(const void *a, const void *b)
{
    return *(char *)a - *(char *)b;
}

void decodeBWT(char *SL, int len, int idx, char *outS)
{
    //#region create F column (sorted SL)
    char F[len + 1];
    strcpy(F, SL);
    qsort(F, len, sizeof(char), qsortString);
    //#endregion create F column (sorted SL)

    //#region create T column
    int T[len];
    // Create ascii_T which contains T value for chars in ASCII table
    int ascii_T[128];
    // Init ascii_T at -1
    for (int i = 0; i < 128; i++)
        ascii_T[i] = -1;

    for (int i = 0; i < len; i++)
    {
        // If is -1, then we haven't found this char yet
        if (ascii_T[(int)SL[i]] == -1)
        {
            // Get position of first SL[i] char in F
            ascii_T[(int)SL[i]] = (strchr(F, SL[i]) - F) / sizeof(char);
        }

        // Set the value to T[i]
        T[i] = ascii_T[(int)SL[i]];

        // Increment in ascii_T for the next time we will find this char
        ascii_T[(int)SL[i]]++;
    }
    //#endregion create T column

    // Fill outS with decoded BWT
    // K is initialised to the T value of the idx
    int k = T[idx];
    // Start filling outS from the end, because decoding BWT give us the string backwards
    for (int i = len - 1; i >= 0; i--)
    {
        // Reminder : F is the sorted string
        outS[i] = F[k];
        k = T[k];
    }
    outS[len] = '\0';

    // For debug
    // printf("   | F |  T | L\n");
    // printf("---|---|----|---\n");
    // for (int i = 0; i < len; i++)
    //     printf("%2d | %c | %2d | %c\n", i, F[i], T[i], SL[i]);
}

// int main(int argc, char const *argv[])
// {
//     //! Don't use char that are not in the ASCII table !!
//     char *S = "lorem ipsum dolor sit amet";
//     // char *S = "MISSISSIPPI";
//     int len = strlen(S);

//     char L[len + 1];
//     int idx = encodeBWT(S, L);

//     printf("%s\n", S);
//     printf("\033[0;33mENCODE BWT\033[0m\n");
//     printf("idx = %d\n", idx);

//     printf("%s\n", L);

//     printf("\033[0;33mDECODE BWT\033[0m\n");
//     char Sdecoded[len + 1];
//     decodeBWT(L, len, idx, Sdecoded);
//     printf("%s\n", Sdecoded);

//     return EXIT_SUCCESS;
// }
