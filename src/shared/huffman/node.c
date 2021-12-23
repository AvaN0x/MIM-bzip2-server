#include "node.h"
#include "TA.h"

#include "count.h"

node_t *consNode(symbol_t Sy, frequence_t Freq)
{
    node_t *node = (node_t *)malloc(sizeof(node_t));
    globalCounter++;
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
    char *buffer = (char *)malloc(sizeof(char) * 100);
    globalCounter++;
    sprintf(buffer, "Noeud [Symbole : %d(%c) | Frequence : %d | Code : %s]", N->S, (N->S == '\0' ? ' ' : N->S), N->F, N->code);
    return buffer;
}

void printNode(node_t *N)
{
    char *str = toStringNode(N);
    printf("%s\n", str);
    free(str);
    str = NULL;
    globalCounter--;
}
