#ifndef BZIP2_H
#define BZIP2_H

int encodeBZIP2(char *string, char **dictionary, char **out, int *outSize);
void decodeBZIP2(char *dictionary, char *in, int inSize, char *out, int *outSize);

#endif
