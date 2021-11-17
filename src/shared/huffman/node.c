#include "node.h"
#include "TA.h"

node_t * emptyNode() {
    return malloc(sizeof(node_t));
}

node_t * consNode(symbol_t Sy, frequence_t Freq) {
    node_t * node = malloc(sizeof(node_t));
    node->S = Sy;
    node->F = Freq;
    node->down = NULL;
    node->up = NULL;

    return node;
}

char * toStringNode(node_t *N) {
    char * buffer = malloc(sizeof(char) * 100);
    sprintf(buffer, "Noeud [Symbole : %c | Frequence : %d]", N->S, N->F);
    return buffer;
}

void printNode(node_t *N) {
    printf("%s\n", toStringNode(N));
}