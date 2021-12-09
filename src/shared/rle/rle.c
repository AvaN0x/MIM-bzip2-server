#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rle.h"
#include "../buffer.h"

void encodeRLE(char *S, int len, char **res, int *resLen)
{
    int shift = 0;
    int size = 4;

    *res = malloc(size * sizeof(char));

    char numberString[10];
    int thisCharCount = 0;
    // For every char of S
    for (int i = 0; i < len; i++)
    {
        // Res is longer than S, which is not a compression, so we realloc res and init it with the same values as S
        if (shift > len)
        {
            // Abort process because size is to big
            realloc(*res, len * sizeof(char));
            for (int j = 0; j < len; j++)
            {
                (*res)[j] = S[j];
                *resLen = len;
            }
            return;
        }

        // Resize res if needed
        if (shift > size)
        {
            // Will add 32 to the actual size, and if new size is above BUFFERSIZE, then we set it to BUFFERSIZE
            size = size + 32;
            if (size > len)
                size = len;
            // Realloc res to new size
            realloc(*res, size * sizeof(char));
        }

        // Last char of the string, or next char is different
        // We do not want thisCharCount to be longer than 5 digit
        if (i == (len - 1) || S[i] != S[i + 1] || thisCharCount > (99999 - 2))
        {
            if (thisCharCount > 4)
            {
                // Format is ~|%d|%c with %d being max 5 digit
                (*res)[shift] = '~';
                (*res)[shift + 1] = '|';
                shift += 2;

                // Set numberString to thisCharCount to add each char from numberString to res
                sprintf(numberString, "%d", thisCharCount + 1);
                for (int j = 0; j < strlen(numberString); j++)
                {
                    (*res)[shift] = numberString[j];
                    shift++;
                }
                (*res)[shift] = '|';
                shift++;

                thisCharCount = 0;
            }
            else if (thisCharCount > 0)
            {
                for (int j = 0; j < thisCharCount; j++)
                {
                    (*res)[shift] = S[i];
                    shift++;
                }
                thisCharCount = 0;
            }
            // Add actual char to res
            (*res)[shift] = S[i];
            shift++;
        }
        else
        {
            thisCharCount++;
        }
    }
    (*res)[shift] = '\0';
    *resLen = shift;
    // Realloc res to the exact size of the result
    realloc(*res, shift * sizeof(char));
}

void decodeRLE(char *S, int len, char **res, int *resLen)
{
    int shift = 0;
    // Size can only be bigger than len
    int size = len;

    *res = malloc(size * sizeof(char));

    // For every char of S
    for (int i = 0; i < len; i++)
    {
        // Resize res if needed
        if (shift > size)
        {
            // Will add 32 to the actual size, and if new size is above BUFFERSIZE, then we set it to BUFFERSIZE
            size = size + 32;
            if (size > BUFFERSIZE)
                size = len;
            // Realloc res to new size
            realloc(*res, size * sizeof(char));
        }

        // RLE
        // Last char of the string, or next char is different
        if (S[i] == '~' && S[i + 1] == '|')
        {
            // Wil correspond to the position of the last char of the pattern ('|')
            int endOfPatternShift = -1;
            for (int j = 3; j <= 7; j++)
            {
                // Check if char is not valid, or if this is the good char
                // i + j is bigger than array length
                if ((i + j) >= len)
                {
                    break;
                }
                if (S[i + j] == '|')
                {
                    // If char is valid and is |
                    endOfPatternShift = j;
                    break;
                }
                else if (S[i + j] < '0' || S[i + j] > '9')
                {
                    // Char is not a number
                    break;
                }
            }
            if (endOfPatternShift > -1)
            {
                int count = 0;
                // Calculate the count
                // Example : "123" will be : ((0 * 10 + 1) * 10 + 2) * 10 + 3 = 123
                for (int j = 2; j < endOfPatternShift; j++)
                {
                    count *= 10;
                    count += S[i + j] - '0';
                }
                // Set new value for i with the shift of the pattern
                i += endOfPatternShift + 1;
                // Add the count of char to the string
                for (int j = 0; j < count; j++)
                {
                    (*res)[shift] = S[i];
                    shift++;
                }

                // Continue to prevent the addition of the actual char from below
                continue;
            }
        }
        // Only happen if the actual char and future ones do no have the pattern : ~|%d|%c
        (*res)[shift] = S[i];
        shift++;
    }
    (*res)[shift] = '\0';
    *resLen = shift;
    // Realloc res to the exact size of the result
    realloc(*res, shift * sizeof(char));
}

int main(int argc, char const *argv[])
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

    return EXIT_SUCCESS;
}