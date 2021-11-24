#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "process.h"

#include "../shared/huffman/huff.h"
#include "../shared/bwt/bwt.h"
#include "../shared/m2f/m2f.h"

void processFile(char *fileName)
{
    char *tmpFileName = malloc(sizeof(char) * (strlen(fileName) + 4));
    strcpy(tmpFileName, "res/");
    strcat(tmpFileName, fileName);

    printf("tmpFileName: %s\n", tmpFileName);
    FILE *file = fopen(tmpFileName, "r");
    free(tmpFileName);

    if (!file)
    {
        fprintf(stderr, "File not found\n");
        return;
    }

    printf("File content :\n");
    {
        char buffer[BUFSIZ];
        int count = 0;
        char c;
        while ((c = fgetc(file)) != EOF)
        {
            // Only allow ascii chars
            if (c < 0 || c > 127)
                continue;

            buffer[count++] = c; // Use actual count value and increment it after

            if (count > (BUFSIZ - 2))
            {
                buffer[count] = '\0';
                processBuffer(buffer, count);
                count = 0;
            }
        }
        if (count > 0)
        {
            buffer[count] = '\0';
            processBuffer(buffer, count);
            count = 0;
        }
        printf("\n");
    }

    fclose(file);
}

void processBuffer(char *buffer, int size)
{
    // int size = strlen(buffer);
    printf("\x1B[33m%d: \"\x1B[0m%s\x1B[33m\"\x1B[0m\n\n", size, buffer);
}