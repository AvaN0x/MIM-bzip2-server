#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "TA.h"
#include "node.h"
#include <unistd.h>

list_t *listCpy(list_t *L)
{
    list_t *list = malloc(sizeof(list_t));
    list->n = L->n;
    list->suc = L->suc;
    return list;
}

list_t *listConstruct(node_t *N)
{
    list_t *list = malloc(sizeof(list_t));
    list->n = N;
    list->suc = NULL;
    return list;
}

list_t *emptyListCons()
{
    list_t *list = malloc(sizeof(list_t));
    list->n = NULL;
    list->suc = NULL;
    return list;
}

minNodes_t getMin(list_t *L)
{
    list_t *tmp = listCpy(L);
    minNodes_t minNodes;
    minNodes.min1 = emptyNode();
    minNodes.min1->F = 99999;
    minNodes.min2 = emptyNode();
    minNodes.min2->F = 99999;

    if (tmp->suc->suc == NULL)
    {
        minNodes.min1 = tmp->n;
        minNodes.min2 = tmp->suc->n;

        return minNodes;
    }

    while (tmp->n != NULL)
    {
        // Get the min
        if (minNodes.min1->F == 99999)
            minNodes.min1 = tmp->n;
        else if (minNodes.min2->F == 99999)
            minNodes.min2 = tmp->n;
        else if (tmp->n->F <= minNodes.min1->F)
        {
            if (minNodes.min1->F <= minNodes.min2->F)
                minNodes.min2 = minNodes.min1;
            minNodes.min1 = tmp->n;
        }

        if (tmp->suc == NULL)
            break;
        tmp = tmp->suc;
    }

    return minNodes;
}

list_t *removeMins(list_t *L, minNodes_t *minNodes, node_t *nNode)
{
    // Create a new element in first position
    list_t *newElement = listConstruct(nNode);
    // Define first element as newElement
    list_t *head = listCpy(L);
    list_t *pred = NULL;

    int removed = 0;

    while (removed < 2 && L != NULL)
    {
        // If current list element is one of the mins
        // In this condition, we just remove the min and reconnect nodes for each situation
        // values of pred, head, L will be updated after
        if (L->n == minNodes->min1 || L->n == minNodes->min2)
        {
            // If it's not the head of the list
            if (L->n != head->n)
            {
                /*
                 * If it's the first value to be removed
                 * insert the new node at this place
                 * and connect pred and suc to new node
                 */
                if (removed == 0)
                {
                    if (L->suc != NULL)
                    {
                        // Connect new node to suc
                        newElement->suc = L->suc;
                        pred->suc = newElement;
                    }
                }
                /*
                 * If it's not the first value to be removed
                 * just make pred point to next value
                 */
                else
                {
                    pred->suc = L->suc;
                }
            }
            // If it's the head of the list
            else
            {
                if (L->suc != NULL)
                {
                    /*
                     * If it's the first value to be removed
                     * Insert new node and connect new node to suc
                     * update head
                     */
                    if (removed == 0)
                    {
                        newElement->suc = L->suc;
                        head = newElement;
                    }
                    /*
                     * If it's not the first value to be removed
                     * just declare head->suc as new head
                     * update head to next element
                     */
                    else
                    {
                        head = L->suc;
                    }
                }
                // If no suc just declare new head as new node
                else
                    head = newElement;
            }
            removed++;
        }

        /*
         * If we are not on the head of the list (pred != NULL)
         * And pred is on the head, update the new head (suc may have change)
         */
        if (pred != NULL && pred->n == head->n)
            head = pred;

        // If no values have been removed, go to next value
        if (removed == 0)
        {
            pred = L;
            pred->suc = L->suc;
        }
        /*
         * If the first value have been removed,
         * then newElement has taken his place so pred is now newElement
         */
        if (removed == 1)
        {
            pred = newElement;
            pred->suc = newElement->suc;
        }

        // Go to next value (L->suc or pred->suc)
        L = pred == NULL ? L->suc : pred->suc;
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

    char *strSuc = (L->suc == NULL ? "[null]" : toStringNode(L->suc->n));
    char *strNode = toStringNode(L->n);

    char *buffer = (char *)malloc(sizeof(char) * (50 + strlen(strNode) + strlen(strSuc)));
    sprintf(buffer, "Liste : [%s]\n\tsuc is : %s", strNode, strSuc);
    return buffer;
}

void printListElement(list_t *L)
{
    printf("%s\n", toStringList(L));
}

void printList(list_t *L)
{
    list_t *tmp = listCpy(L);
    while (tmp->n != NULL)
    {
        printf("%s\n", toStringList(tmp));
        if (tmp->suc == NULL)
            break;
        tmp = tmp->suc;
    }
}

void getCode(node_t *node, char **huffmanDico)
{
    if (node->up != NULL)
    {
        node->up->code = (char *)realloc(node->up->code, 1 + sizeof(char) * strlen(node->code));
        node->up->code = strcat(node->up->code, node->code);
        getCode(node->up, huffmanDico);
    }
    if (node->down != NULL)
    {
        node->down->code = (char *)realloc(node->down->code, 1 + sizeof(char) * strlen(node->code));
        node->down->code = strcat(node->down->code, node->code);
        getCode(node->down, huffmanDico);
    }

    // Only print the base of the tree
    if (node->down == NULL && node->up == NULL)
    {
        huffmanDico[node->S] = (char *)malloc(sizeof(char) * strlen(node->code));
        sprintf(huffmanDico[node->S], "%s", reverseCode(node->code));
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
