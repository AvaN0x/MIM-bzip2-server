#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "list.h"
#include "TA.h"
#include "node.h"
#include <unistd.h>

#include "count.h"

list_t *listConstruct(node_t *N)
{
    list_t *list = (list_t *)malloc(sizeof(list_t));
    globalCounter++;
    list->n = N;
    list->suc = NULL;
    return list;
}

list_t *emptyListCons()
{
    list_t *list = (list_t *)malloc(sizeof(list_t));
    globalCounter++;
    list->n = NULL;
    list->suc = NULL;
    return list;
}

minNodes_t getMin(list_t *L)
{
    list_t *tmp = L;

    minNodes_t minNodes;
    minNodes.min1 = NULL;
    minNodes.min2 = NULL;

    // If only 2 values left
    if (tmp->suc->suc == NULL)
    {
        minNodes.min1 = tmp->n;
        minNodes.min2 = tmp->suc->n;

        return minNodes;
    }

    while (tmp != NULL && tmp->n != NULL)
    {
        if (minNodes.min1 == NULL)
            minNodes.min1 = tmp->n;
        else if (minNodes.min2 == NULL)
            minNodes.min2 = tmp->n;
        else if (tmp->n->F <= minNodes.min1->F)
        {
            if (minNodes.min1->F <= minNodes.min2->F)
                minNodes.min2 = minNodes.min1;
            minNodes.min1 = tmp->n;
        }

        tmp = tmp->suc;
    }

    return minNodes;
}

list_t *removeMins(list_t *current, minNodes_t *minNodes, node_t *nNode)
{
    // Create a new element in first position
    list_t *newElement = listConstruct(nNode);
    // Define first element as newElement
    list_t *head = current;
    list_t *pred = NULL;

    int removed = 0;

    while (removed < 2 && current != NULL)
    {
        if (0)
        {
            printf("\nHead is : ");
            printListElement(head);

            printf("\nCurrent value is : ");
            printListElement(current);

            printf("\npred is : ");
            printListElement(pred);

            sleep(1);
        }

        // If current list element is one of the mins
        // In this condition, we just remove the min and reconnect nodes for each situation
        // values of pred, head, current will be updated after
        if (current->n == minNodes->min1 || current->n == minNodes->min2)
        {
            // printf("I'm a min\n");
            // If it's not the head of the list
            if (current->n != head->n)
            {
                /*
                 * If it's the first value to be removed
                 * insert the new node at this place
                 * and connect pred and suc to new node
                 */
                if (removed == 0)
                {
                    if (current->suc != NULL)
                    {
                        // Connect new node to suc
                        newElement->suc = current->suc;
                        pred->suc = newElement;
                    }
                }
                /*
                 * If it's not the first value to be removed
                 * just make pred point to next value
                 */
                else
                    pred->suc = current->suc;
            }
            // If it's the head of the list
            else
            {
                if (current->suc != NULL)
                {
                    /*
                     * If it's the first value to be removed
                     * Insert new node and connect new node to suc
                     * update head
                     */
                    if (removed == 0)
                    {
                        newElement->suc = current->suc;
                        head = newElement;
                    }
                    /*
                     * If it's not the first value to be removed
                     * just declare head->suc as new head
                     * update head to next element
                     */
                    else
                        head = current->suc;
                }
                // If no suc just declare new head as new node
                else
                    head = newElement;
            }
            removed++;
            free(current);
            current = NULL;
            globalCounter--;
        }

        if (removed == 2)
            break;

        if (pred == NULL)
            if (removed == 0)
                pred = current;
            else
                pred = newElement;
        else
            pred = pred->suc;

        current = pred->suc;
    }

    // Check values after each iteration
    if (0)
    {
        printf("Print all values\n");
        printList(head);
        sleep(1);
    }
    return head;
}

void freeNodes(node_t *node)
{

    if (node->up != NULL)
    {
        freeNodes(node->up);
        node->up = NULL;
    }
    if (node->down != NULL)
    {
        freeNodes(node->down);
        node->down = NULL;
    }

    // printNode(node);
    if (strcmp(node->code, "") != 0)
    {
        free(node->code);
        node->code = NULL;
        globalCounter--;
    }
    free(node);
    node = NULL;
    globalCounter--;
}

char *toStringList(list_t *L)
{
    if (L == NULL)
        return "Liste : [null]";

    char *strSuc = (L->suc == NULL || L->suc->n == NULL ? "[null]" : toStringNode(L->suc->n));
    char *strNode = toStringNode(L->n);

    char *buffer = (char *)malloc(sizeof(char) * (50 + strlen(strNode) + strlen(strSuc) + 1));
    globalCounter++;
    sprintf(buffer, "Liste : [%s]\n\tsuc is : %s", strNode, strSuc);
    return buffer;
}

void printListElement(list_t *L)
{
    char *str = toStringList(L);
    printf("%s\n", str);
    if (strcmp(str, "Liste : [null]") != 0)
    {
        free(str);
        str = NULL;
        globalCounter--;
    }
}

void printList(list_t *L)
{
    list_t *tmp = L;
    while (tmp != NULL)
    {
        char *str = toStringList(tmp);
        printf("%s\n", str);
        if (strcmp(str, "Liste : [null]") != 0)
        {
            free(str);
            str = NULL;
            globalCounter--;
        }
        tmp = tmp->suc;
    }
}

void getCode(node_t *node, char **huffmanDico)
{
    int lengthCode = strlen(node->code);
    if (node->up != NULL)
    {
        node->up->code = (char *)realloc(node->up->code, 1 + sizeof(char) * lengthCode);
        node->up->code = strcat(node->up->code, node->code);
        getCode(node->up, huffmanDico);
    }
    if (node->down != NULL)
    {
        node->down->code = (char *)realloc(node->down->code, 1 + sizeof(char) * lengthCode);
        node->down->code = strcat(node->down->code, node->code);
        getCode(node->down, huffmanDico);
    }

    if (node->down == NULL && node->up == NULL)
    {
        huffmanDico[node->S] = (char *)malloc(sizeof(char) * lengthCode);
        globalCounter++;
        strncpy(huffmanDico[node->S], reverseCode(node->code), lengthCode);
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
