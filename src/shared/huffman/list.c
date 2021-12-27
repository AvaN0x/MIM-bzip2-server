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

    while (tmp != NULL && tmp->n != NULL)
    {
        if (minNodes.min1 != NULL && minNodes.min1->down == NULL && minNodes.min2 != NULL && minNodes.min2->down == NULL)
            break;

        if (minNodes.min1 == NULL)
            minNodes.min1 = tmp->n;
        else
        {
            if (minNodes.min2 == NULL)
            {
                minNodes.min2 = tmp->n;
            }
            else if (tmp->n->F == minNodes.min2->F && tmp->n->lvl < minNodes.min2->lvl)
                minNodes.min2 = tmp->n;
            else if (tmp->n->F == minNodes.min1->F && tmp->n->lvl < minNodes.min1->lvl)
            {
                minNodes.min1 = minNodes.min2;
                minNodes.min2 = tmp->n;
            }
        }

        if (0)
        {
            if (minNodes.min1 != NULL)
            {
                printf("min1 : %c|%d\n", minNodes.min1->S, minNodes.min1->F);
                printf("\t");
                printNode(minNodes.min1->up);
                printf("\t");
                printNode(minNodes.min1->down);
            }
            else
                printf("min1 is NULL\n");
            if (minNodes.min2 != NULL)
            {
                printf("min2 : %c|%d\n", minNodes.min2->S, minNodes.min2->F);
                printf("\t");
                printNode(minNodes.min2->up);
                printf("\t");
                printNode(minNodes.min2->down);
            }
            else
                printf("min2 is NULL\n");
        }

        tmp = tmp->suc;
    }

    if (minNodes.min1->lvl < minNodes.min2->lvl)
    {
        node_t *tmp = minNodes.min1;
        minNodes.min1 = minNodes.min2;
        minNodes.min2 = tmp;
    }

    return minNodes;
}

list_t *removeMins(list_t *current, minNodes_t *minNodes)
{
    // Define first element as newElement
    list_t *head = current;
    list_t *pred = NULL;

    int removed = 0;

    if (head->suc->suc == NULL)
    {
        free(head->suc);
        head->suc = NULL;
        globalCounter--;
        free(head);
        head = NULL;
        globalCounter--;

        return head;
    }

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

        if (current->n == minNodes->min1 || current->n == minNodes->min2)
        {
            if (0)
                printf("I'm a min !\n");

            // Remove current from the list
            if (current->n == head->n)
                head = current->suc;
            else
                pred->suc = current->suc;

            removed++;
            free(current);
            current = NULL;
            globalCounter--;
        }

        if (removed == 2)
            break;

        if (pred == NULL)
        {
            if (removed == 0)
                pred = head;
            else if (current != NULL)
                pred = head;
            else
                current = head;
        }
        else if (current != NULL)
            pred = pred->suc;

        if (pred != NULL)
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
    char *buffer = (char *)malloc(sizeof(char) * (1000));
    globalCounter++;
    if (L == NULL)
        sprintf(buffer, "Liste : [null]");
    else
    {
        char *strSuc = (char *)malloc(sizeof(char) * 200);
        if (L->suc == NULL)
        {
            sprintf(strSuc, "[null]");
            globalCounter++;
        }
        else
            strSuc = toStringNode(L->suc->n);
        char *strNode = toStringNode(L->n);
        char *strNode2 = toStringNode(L->n->down);
        char *strNode3 = toStringNode(L->n->up);

        sprintf(buffer, "Liste : [%s\n\t%s\n\t%s]\n\tsuc is : %s", strNode, strNode2, strNode3, strSuc);

        free(strSuc);
        strSuc = NULL;
        globalCounter--;

        free(strNode);
        strNode = NULL;
        globalCounter--;

        free(strNode2);
        strNode2 = NULL;
        globalCounter--;

        free(strNode3);
        strNode3 = NULL;
        globalCounter--;
    }

    return buffer;
}

void printListElement(list_t *L)
{
    char *str = toStringList(L);
    printf("%s\n", str);

    free(str);
    str = NULL;
    globalCounter--;
}

void printList(list_t *L)
{
    list_t *tmp = L;
    while (tmp != NULL)
    {
        char *str = toStringList(tmp);
        printf("%s\n", str);

        free(str);
        str = NULL;
        globalCounter--;

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
        char *reversedCode = reverseCode(node->code);
        strncpy(huffmanDico[node->S], reversedCode, lengthCode);
    }
}

char *reverseCode(char *code)
{
    int lgth = strlen(code) - 1;
    for (int i = 0; i <= lgth / 2; i++)
    {
        char tmp = code[i];
        code[i] = code[lgth - i];
        code[lgth - i] = tmp;
    }
    return code;
}
