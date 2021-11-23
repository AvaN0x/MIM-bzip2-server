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
    list->pred = NULL;
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

    if (L->suc->suc == NULL)
    {
        minNodes.min1 = L->n;
        minNodes.min2 = L->suc->n;

        return minNodes;
    }

    // Go to the end
    while (L->n != NULL)
    {
        if (L->suc == NULL)
            break;
        L = L->suc;
    }

    while (L->n != NULL)
    {
        // Get the min
        if (minNodes.min1->F == 99999)
            minNodes.min1 = L->n;
        else if (minNodes.min2->F == 99999)
            minNodes.min2 = L->n;
        else if (L->n->F <= minNodes.min1->F)
        {
            if (minNodes.min1->F <= minNodes.min2->F)
                minNodes.min2 = minNodes.min1;
            minNodes.min1 = L->n;
        }

        if (L->pred == NULL)
            break;
        L = L->pred;
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
    list_t *newElement = listCons(nNode, NULL);
    // Define first element as newElement
    list_t *head = L;

    int removed = 0;

    // Get the min nodes out of the flux
    while (removed < 2 && L != NULL)
    {
        // Copy of pointer to clean after getting suc
        list_t *cpy = listCons(L->n, L->suc);
        cpy->pred = L->pred;
        if (L->n == minNodes->min1 || L->n == minNodes->min2)
        {
            if (L->n != head->n)
            {
                if (removed == 0)
                {
                    L->pred->suc = newElement;
                    newElement->pred = L->pred;
                    if (L->suc != NULL)
                    {
                        L->suc->pred = newElement;
                        newElement->suc = L->suc;
                    }
                }
                else
                {
                    if (L->suc != NULL)
                    {
                        L->suc->pred = L->pred;
                        L->pred->suc = L->suc;
                    }
                    else
                    {
                        L->pred->suc = NULL;
                    }
                }
            }
            else
            {
                if (L->suc != NULL)
                {
                    // Insert new element as head
                    if (removed == 0)
                    {
                        newElement->suc = L->suc;
                        L->suc->pred = newElement;
                        head = newElement;
                    }
                    else
                    {
                        (L->suc)->pred = NULL;
                        head = L->suc;
                    }
                }
                else
                    head = newElement;
            }
            removed++;
        }

        L = L->suc;

        if (cpy->n == minNodes->min1 || cpy->n == minNodes->min2)
        {
            cpy->suc = NULL;
            cpy->pred = NULL;
        }
    }

    return head;
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
    if (L == NULL)
        return "Liste : [null]";
    char *buffer = malloc(sizeof(char) * 100);
    sprintf(buffer, "Liste : [%s]", toStringNode(L->n));
    return buffer;
}

void printList(list_t *L)
{
    printf("%s\n", toStringList(L));
}

/**
 * @brief Recursiv function to build the code of every node of the tree
 *
 * @param node node_t* The root of the tree
 */
void getCode(node_t *node, FILE *encodeFile)
{
    if (node->down != NULL)
    {
        node->down->code = (char *)realloc(node->down->code, 1 + sizeof(char) * strlen(node->code));
        node->down->code = strcat(node->down->code, node->code);
        getCode(node->down, encodeFile);
    }
    if (node->up != NULL)
    {
        node->up->code = (char *)realloc(node->up->code, 1 + sizeof(char) * strlen(node->code));
        node->up->code = strcat(node->up->code, node->code);
        getCode(node->up, encodeFile);
    }

    if (node->S != ' ')
    {
        char *toFile = malloc(sizeof(char) * (1 + 1 + 1 + strlen(node->code)));
        sprintf(toFile, "%c %s ", node->S, reverseCode(node->code));
        fwrite(toFile, sizeof(char), strlen(toFile), encodeFile);
        printNode(node);
    }
}

char *reverseCode(char *code)
{
    int lgth = strlen(code) - 1;
    for (int i = 0; i < lgth / 2; i++)
    {
        char tmp = code[i];
        code[i] = code[lgth - i];
        code[lgth - i] = tmp;
    }
    return code;
}