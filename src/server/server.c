#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    // TODO when the full bzip2 encode and decode is working
    char *fileName = "mississippi.txt";

    //#region snippet to read file in res/ directory
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
    //#endregion
    return 0;
}
