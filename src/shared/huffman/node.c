#include "node.h"
#include "TA.h"

node_t *emptyNode()
{
    node_t *node = malloc(sizeof(node_t));
    return node;
}

node_t *consNode(symbol_t Sy, frequence_t Freq)
{
    node_t *node = malloc(sizeof(node_t));
    node->S = Sy;
    node->F = Freq;
    node->down = NULL;
    node->up = NULL;
    node->code = NULL;

    return node;
}

char *toStringNode(node_t *N)
{
    if (N == NULL)
        return "Noeud (NULL)";
    char *buffer = malloc(sizeof(char) * 100);
    sprintf(buffer, "Noeud [Symbole : %d(%c) | Frequence : %d | Code : %s]", N->S, (N->S == '\0' ? ' ' : N->S), N->F, N->code);
    return buffer;
}

void printNode(node_t *N)
{
    printf("%s\n", toStringNode(N));
}
