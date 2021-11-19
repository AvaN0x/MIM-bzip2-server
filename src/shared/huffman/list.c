#include <stdbool.h>
#include <stdio.h>
#include <string.h>
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
    minNodes_t minNodes;
    minNodes.min1 = emptyNode();
    minNodes.min1->F = 99999;
    minNodes.min2 = emptyNode();
    minNodes.min2->F = 99999;

    while (L->n != NULL)
    {
        // printf("Current node = ");
        // printNode(L->n);
        // Get the min
        if (minNodes.min1 == NULL)
            minNodes.min1 = L->n;
        else if (minNodes.min2 == NULL)
            minNodes.min2 = L->n;
        else if (L->n->F < minNodes.min1->F)
        {
            if (minNodes.min1->F < minNodes.min2->F)
                minNodes.min2 = minNodes.min1;
            minNodes.min1 = L->n;
        }
        else if (L->n->F < minNodes.min2->F)
            minNodes.min2 = L->n;

        // printf("Current Mins = [");
        // printNode(minNodes.min1);
        // printNode(minNodes.min2);
        // printf("]\n");

        // Go to next
        if (L->suc != NULL)
            L = L->suc;
        else
            break;
    }

    return minNodes;
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
    // Create a new element in first position
    list_t *newElement = listCons(nNode, L);
    // Define first element as newElement
    list_t *head = newElement;
    L->pred = newElement;
    L = L->pred;

    int removed = 0;

    // Get the min nodes out of the flux
    while (removed < 2)
    {
        // Copy of pointer to clean after getting suc
        list_t *cpy = listCons(L->n, L->suc);
        cpy->pred = L->pred;
        if (L->n == minNodes->min1 || L->n == minNodes->min2)
        {
            if (L->n != head->n)
            {
                if (L->suc != NULL)
                    (L->suc)->pred = L->pred;
                (L->pred)->suc = L->suc;
            }
            else if (L->suc != NULL)
                head = L->suc;

            removed++;
        }
        L = L->suc;
        cpy->suc = NULL;
        cpy->pred = NULL;
    }

    return newElement;
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
    sprintf(buffer, "Liste : [%s]", toStringNode(L->n));
    return buffer;
}

void printList(list_t *L)
{
    printf("%s\n", toStringList(L));
}

void getCode(node_t *node)
{
    printNode(node);
    if (node->down != NULL)
    {
        printf("Hello down\n");
        printNode(node->down);
        node->down->code = realloc(node->down->code, sizeof(char) * (strlen(node->code) + 1));
        strcat(node->down->code, node->code);
        printf("sprintf fait\n");
        getCode(node->down);
    }
    if (node->up != NULL)
    {
        printf("Hello up\n");
        printNode(node->up);
        sprintf(node->up->code, "%s", node->code);
        printf("sprintf fait\n");
        getCode(node->up);
    }
    int lgth = strlen(node->code);
    for (int i = 0; i < lgth; i++)
        printf("%c", node->code[lgth - i]);
    printf("\n");
}