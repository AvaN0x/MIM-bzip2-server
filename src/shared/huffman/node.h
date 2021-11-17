#ifndef NODE_H
#define NODE_H

#include "TA.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct elem_node_t {
	symbol_t S;
	frequence_t F;
	struct elem_node_t * down , * up;
} node_t;

/**
 * préférez les fonctions d'accès en consultation et modification
 * de manière à ne faire qu'un changement si l'on change de T.A.
 *
 * Plus tard, vous verrez les objets et leurs patrons (templates)
 */

node_t * emptyNode();

node_t * consNode(symbol_t Sy, frequence_t Freq);

char * toStringNode(node_t *N);

void printNode(node_t *N);

#endif
