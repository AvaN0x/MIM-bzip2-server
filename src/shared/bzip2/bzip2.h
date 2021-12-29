#ifndef BZIP2_H
#define BZIP2_H

int encodeBZIP2(char *string, int size, int charFrequences[128], unsigned char **out, int *outSize);
void decodeBZIP2(unsigned char *in, int inSize, int idxBWT, int charFrequences[128], char **out, int *outSize);

#endif
