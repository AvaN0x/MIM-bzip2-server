#ifndef M2F_H
#define M2F_H

void encodeM2F(char *S, int *shifts);

void decodeM2F(int *shifts, int len, char *result);

#endif
