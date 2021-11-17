#include <stdbool.h>
#include <stdio.h>
#include "list.h"
#include "TA.h"
#include "node.h"

/**
 * @brief Constructor of a list
 * 
 * @param N node_t* A node 
 * @param L list_t* A pointer on next element
 * @return list_t* 
 */
list_t *listCons(node_t *N, list_t *L)
{
    list_t *list = malloc(sizeof(list_t));
    list->n = N;
    list->suc = L;
    return list;
}

/**
 * @brief Constructor of an empty list
 * 
 * @return list_t* 
 */
list_t *emptyListCons()
{
    list_t *list = malloc(sizeof(list_t));
    list->n = NULL;
    list->suc = NULL;
    return list;
}

void setNode(list_t *L, node_t *N) {
    L->n = N;
}

bool isEmptyList(list_t *L)
{
    return (L->suc == NULL && L->n == NULL);
}

node_t *listHead(list_t *L)
{
    return (L->n);
}

list_t *listRest(list_t *L)
{
    return (L->suc);
}

list_t *insNode(node_t *N, list_t *L)
{
    list_t *newElement = malloc(sizeof(list_t));
    newElement->n = N;
    newElement->suc = NULL;

    L->suc = newElement;

    return newElement;
}

list_t *destroyList(list_t *L) {
    if (!L->n)
        free(L->n);

    list_t * next = L->suc;
    free(L);
    return next;
}

char * toStringList(list_t *L) {
    char * buffer = malloc(sizeof(char) * 100);
    sprintf(buffer, "Liste : [\n\t%s\n]", toStringNode(L->n));
    return buffer;
}

void printList(list_t *L) {
    printf("%s\n", toStringList(L));
}