#ifndef NODE_H
#define NODE_H

#include "TA.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct elem_node_t
{
	symbol_t S;
	frequence_t F;
	char *code;
	int lvl;
	struct elem_node_t *down, *up;
} node_t;

/**
 * @brief Create a new node with a symbol and a frequence
 *
 * @param Sy The symbol
 * @param Freq The frequency
 * @return node_t*
 */
node_t *consNode(symbol_t Sy, frequence_t Freq);

/**
 * @brief Create a sring from a node
 *
 * @param N The node
 * @return char*
 */
char *toStringNode(node_t *N);

/**
 * @brief Print a node
 *
 * @param N The node
 */
void printNode(node_t *N);

#endif