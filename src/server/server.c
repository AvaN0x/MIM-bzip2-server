#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../shared/huffman/huff.h"

int main(int argc, char const *argv[])
{
    // TODO when the full bzip2 encode and decode is working
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
    
    // Execute Huffman
    char *fileName2 = "Freq.txt";

    char *tmpFileName2 = malloc(sizeof(char) * (strlen(fileName2) + 4));
    strcpy(tmpFileName2, "res/");
    strcat(tmpFileName2, fileName2);

    // char * buffer = malloc(sizeof(char) * 20);
    // sprintf(buffer, "res/%s", "Freq.txt");
	printf("path = %s\n", tmpFileName2);
    encodeHuffman(tmpFileName2);
    
    return 0;
}
