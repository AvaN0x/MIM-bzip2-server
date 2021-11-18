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
    list->pred = NULL;
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

/**
 * @brief Get the 2 min nodes
 *
 * @param L list_t* The list to search the mins
 * @return minNodes_t
 */
minNodes_t getMin(list_t *L)
{
    minNodes_t mins;
    mins.min1 = emptyNode();
    mins.min2 = emptyNode();

    while (L->n != NULL)
    {
        // Get the min
        if (mins.min1 == NULL)
            mins.min1 = L->n;
        else if (mins.min2 == NULL)
            mins.min2 = L->n;
        else if (L->n < mins.min1)
        {
            if (mins.min1 < mins.min2)
                mins.min2 = mins.min1;
            mins.min1 = L->n;
        }
        else if (L->n < mins.min2)
            mins.min2 = L->n;

        // Go to next
        if (L->suc != NULL)
            L = L->suc;
        else
            break;
    }

    return mins;
}

void setNode(list_t *L, node_t *N)
{
    L->n = N;
    L->pred = NULL;
    L->suc = NULL;
}

bool isEmptyList(list_t *L)
{
    return (L->suc == NULL && L->n == NULL && L->pred == NULL);
}

node_t *listHead(list_t *L)
{
    return (L->n);
}

list_t *listRest(list_t *L)
{
    return (L->suc);
}

/**
 * @brief Insert node N into new list element. Link new element as successor of list element given
 *
 * @param N node_t* The node to insert
 * @param L list_t* The list to add the new successor
 * @return list_t* The new list element
 */
list_t *insNode(node_t *N, list_t *L)
{
    list_t *newElement = malloc(sizeof(list_t));
    newElement->n = N;
    newElement->pred = L;
    newElement->suc = NULL;

    L->suc = newElement;

    return newElement;
}

/**
 * @brief Remove the 2 min nodes of the list
 *
 * @param L The list to search in
 * @return list_t* The new Head of the list
 */
list_t *removeMins(list_t *L, minNodes_t *minNodes, node_t *nNode)
{
    list_t *head = listCons(L->n, L->suc);
    list_t *newHead = listCons(nNode, L->suc);

    int removed = 0;

    // Get the min nodes out of the flux
    while (removed < 2)
    {
        list_t *cpy = listCons(L->n, L->suc);
        cpy->pred = L->pred;

        if (L->n == minNodes->min1 || L->n == minNodes->min2)
        {
            if (L->n != head->n)
            {
                // Connect pred and suc together
                (L->pred)->suc = L->suc;
                (L->suc)->pred = L->pred;
            }
            else
            {
                (L->suc)->pred = newHead;
            }
            removed++;
        }
        L = L->suc;
        cpy->suc = NULL;
        cpy->pred = NULL;
    }

    return newHead;
}

list_t *destroyList(list_t *L)
{
    if (L->n != NULL)
        free(L->n);

    list_t *next = L->suc;
    free(L);
    return next;
}

char *toStringList(list_t *L)
{
    char *buffer = malloc(sizeof(char) * 100);
    sprintf(buffer, "Liste : [\n\t%s\n]", toStringNode(L->n));
    return buffer;
}

void printList(list_t *L)
{
    printf("%s\n", toStringList(L));
}