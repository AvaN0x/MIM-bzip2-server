#ifndef HUFF_H
#define HUFF_H

#include <stdio.h>

/**
 * @brief Construct the code for each caracter with more than 1 appearance buy building Huffman tree
 *
 * @param frequencies The frequency of each caracter in ASCII table from a string
 * @param HuffmanDico The dictionnary than will be updated with the code of each caracter as a string
 */
void buildCodeHuffman(int *frequencies, char **HuffmanDico);

/**
 * @brief Encode a string with a dictionnary
 *
 * @param str The string to encode
 * @param HuffmanDico The dictionnary
 *
 * @return char* The encoded string
 */
unsigned char *encodeHuffman(char *str, char **HuffmanDico);

/**
 * @brief Decode a string with a dictionnary
 *
 * @param str The string to decode
 * @param HuffmanDico The dictionnary
 *
 * @return char* The decoded string
 */
char *decodeHuffman(char *str, char **HuffmanDico);

#endif
