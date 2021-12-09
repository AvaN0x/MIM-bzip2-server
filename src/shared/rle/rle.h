#ifndef RLE_H
#define RLE_H

void encodeRLE(char *S, int len, char **res, int *resLen);

void decodeRLE(char *S, int len, char **res, int *resLen);

#endif
