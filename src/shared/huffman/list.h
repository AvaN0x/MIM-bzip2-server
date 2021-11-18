#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

#include "TA.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct elem_list_t
{
	node_t *n;
	struct elem_list_t *suc;
	struct elem_list_t *pred;
} list_t;

typedef struct
{
	node_t *min1;
	node_t *min2;
} minNodes_t;

list_t *
listCons(node_t *N, list_t *L);

list_t *emptyListCons();

void setNode(list_t *L, node_t *N);

bool isEmptyList(list_t *L);

node_t *listHead(list_t *L);

list_t *listRest(list_t *L);

list_t *insNode(node_t *N, list_t *L);

list_t *destroyList(list_t *L);

list_t *removeMins(list_t *L, minNodes_t *minNodes, node_t *nNode);

minNodes_t getMin(list_t *L);

char *toStringList(list_t *N);

void printList(list_t *N);

#endif
