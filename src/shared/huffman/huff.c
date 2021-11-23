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
#include <ctype.h>

#define YELLOW "\x1B[33m"
#define RESET "\x1B[0m"

FILE *encodeHuffman(char *file)
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

	// CREATION OF THE INITIAL LIST
	int T;
	while ((T = fscanf(openFile, " %c %d", &S, &F)) == 2)
	{
		// Print the value read
		// printf(YELLOW " %d :: Caractere %c de frequence %u \n" RESET, T, S, F);

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
	// printf("%d == %d\n", T, EOF);

	// CREATION OF THE HUFFMAN TREE
	// if |A| == 1 stop
	while (lHead->suc != NULL)
	{
		// Create a copy of the head of the list to search min nodes
		list_t *toGetMin = listCons(lHead->n, lHead->suc);
		// Get the 2 min nodes
		minNodes_t minNodes = getMin(toGetMin);

		// Given to mins a binary value (part of their code)
		minNodes.min1->code = (char *)malloc(sizeof(char));
		sprintf(minNodes.min1->code, "0");

		minNodes.min2->code = (char *)malloc(sizeof(char));
		sprintf(minNodes.min2->code, "1");

		// Create the new node from the 2 min nodes. We don't care about the symbol given
		node_t *nNode = consNode(' ', minNodes.min1->F + minNodes.min2->F);

		// Save the new node node as father of min nodes
		nNode->down = minNodes.min1;
		nNode->up = minNodes.min2;

		// Remove the 2 mins of the list
		lHead = removeMins(lHead, &minNodes, nNode);

		// Print all values of the list
		if (0)
		{
			printf(YELLOW "\n\nPrint all values of the list : \n" RESET);
			// Create a copy of lHead to browse through the list
			list_t *toMap = listCons(lHead->n, lHead->suc);
			while (toMap->n != NULL)
			{
				printList(toMap);
				if (toMap->suc == NULL)
					break;
				toMap = toMap->suc;
			}
		}
	}

	// CONSTRUCT THE CODE OF EACH NODE
	lHead->n->code = (char *)malloc(sizeof(char));
	lHead->n->code = "";
	printf(YELLOW "\nHUFFMAN\n" RESET);

	FILE *encodeFile = fopen("res/encodedHuffman.txt", "w");
	assert(encodeFile != NULL);
	getCode(lHead->n, encodeFile);

	// Free all datas of the list
	while (lHead->n != NULL)
	{
		if (lHead->suc == NULL)
			break;
		lHead = destroyList(lHead);
	}

	return encodeFile;
}
