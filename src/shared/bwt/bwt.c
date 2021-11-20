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

int encodeBWT(char *S, int *L)
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

        L[i] = dec(k, len);

        if (k == 0)
            idx = i;

        M[k] = true;
    }

    return idx;
}

// TODO decode

// int main(int argc, char const *argv[])
// {
//     char *S = "MISSISSIPPI";
//     int len = strlen(S);

//     int L[len];
//     int idx = encodeBWT(S, L);

//     printf("idx = %d\n", idx);

//     for (int i = 0; i < len; i++)
//         printf("L[%2d] = %2d (%c)\n", i, L[i], S[L[i]]);

//     return EXIT_SUCCESS;
// }
