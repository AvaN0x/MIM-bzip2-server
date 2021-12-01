#ifndef M2F_H
#define M2F_H

void encodeM2F(char *S, int len, char *shifts);

void decodeM2F(char *shifts, int len, char *result);

#endif
