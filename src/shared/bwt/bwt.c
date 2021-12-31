#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include "bwt.h"

/**
 * Increment and int with a max value, if at max, will go back to 0
 *
 * @param i The int to increment
 * @param max The max value of the int
 * @return int The new value of the int
 */
int inc(int i, int max)
{
    return (i + 1) % max;
}

/**
 * Decrement and int with a max value, if at 0, will go to (max - 1)
 *
 * @param i The int to decrement
 * @param max The max value of the int
 * @return int The new value of the int
 */
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

/**
 * Encode a specified string with BWT
 *
 * @param S The string to encode
 * @param L The exit string containing the encoded string
 * @return int The index obtained by the BWT
 */
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

/**
 * Function to sort with qsort an array of char *
 *
 * @param a left value
 * @param b right value
 * @return int difference between the two char
 */
int qsortString(const void *a, const void *b)
{
    return *(char *)a - *(char *)b;
}

/**
 * Decode a specified string with BWT
 *
 * @param SL The string to decode
 * @param len The length of the string
 * @param idx The index defined with encode
 * @param outS The exit string containing the decoded string, need to be allocated before with the same length as the input string
 */
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
