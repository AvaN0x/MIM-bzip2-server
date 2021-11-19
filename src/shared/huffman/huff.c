#include <stdio.h>
#include <assert.h>

#include "TA.h"
#include "node.h"
#include "list.h"
#include "huff.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define YELLOW "\x1B[33m"
#define RESET "\x1B[0m"

void encodeHuffman(char *file)
{
	// Create the list
	list_t *list = emptyListCons();
	list_t *lHead = emptyListCons();
	lHead->pred = NULL;

	symbol_t S;
	frequence_t F;

	FILE *openFile = fopen(file, "r");
	free(file);
	assert(openFile != NULL);

	// Création de la liste
	int T;
	while ((T = fscanf(openFile, " %c %d", &S, &F)) == 2)
	{
		// Print the value read
		printf(YELLOW " %d :: Caractere %c de frequence %u \n" RESET, T, S, F);

		/** construction du noeud */
		node_t *node = consNode(S, F);

		/** insertion du noeud dans la liste */
		if (list->n == NULL)
		{
			setNode(list, node);
			lHead->n = node;
		}
		else
		{
			list = insNode(node, list);
			if (lHead->suc == NULL)
				lHead->suc = list;
		}
	}
	fclose(openFile);
	printf("%d == %d\n", T, EOF);

	printf("lHead = ");
	printList(lHead);

	// Get the too lower nodes
	// if |A| == 1 stop
	while (lHead->suc != NULL)
	{
		// Create a copy of the head of the list to search min nodes
		list_t *toGetMin = listCons(lHead->n, lHead->suc);
		printf("toGetMin = ");
		printList(toGetMin);
		// Get the 2 min nodes
		minNodes_t minNodes = getMin(toGetMin);

		printf("min1 = %s | min2 = %s\n", toStringNode(minNodes.min1), toStringNode(minNodes.min2));

		// Save the binary code for encode later
		if (minNodes.min1->code == NULL)
			minNodes.min1->code = "0";
		else
			sprintf(minNodes.min1->code, "0");

		if (minNodes.min2->code == NULL)
			minNodes.min2->code = "1";
		else
			sprintf(minNodes.min2->code, "1");

		printf("min1.code = %s\n", minNodes.min1->code);
		printf("min2.code = %s\n", minNodes.min2->code);

		// Create the new node from the 2 min nodes
		node_t *nNode = consNode('a', minNodes.min1->F + minNodes.min2->F);
		printf("New node created = ");
		printNode(nNode);

		// Save the new node node as father of min nodes
		nNode->down = minNodes.min1;
		nNode->up = minNodes.min2;
		printf("Connecting nNode with children :\n");
		printNode(nNode->down);
		printNode(nNode->up);
		printf("\n");

		// Remove the 2 mins of the list
		lHead = removeMins(lHead, &minNodes, nNode);
		printf("New list head = ");
		printList(lHead);

		printf(YELLOW "\n\nPrint all values of the list : \n" RESET);
		list_t *toMap = listCons(lHead->n, lHead->suc);
		while (toMap->n != NULL)
		{
			printList(toMap);
			if (toMap->suc == NULL)
				break;
			toMap = toMap->suc;
		}
	}
	// lHead->n->code = ".";
	// getCode(lHead->n);

	// Free all datas of the list
	while (lHead->n != NULL)
	{
		if (lHead->suc == NULL)
			break;
		lHead = destroyList(lHead);
	}
}
