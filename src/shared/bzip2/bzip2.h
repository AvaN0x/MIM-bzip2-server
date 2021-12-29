#ifndef BZIP2_H
#define BZIP2_H

int32_t encodeBZIP2(char *string, int size, int32_t *charFrequences, unsigned char **out, int *outSize);
void decodeBZIP2(unsigned char *in, int inSize, int32_t idxBWT, int32_t *charFrequences, char **out, int *outSize);

#endif
