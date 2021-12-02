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

void encodeHuffman(int *dico, char **HuffmanDico)
{
	// Create the list
	list_t *lHead = emptyListCons();
	list_t *pred = emptyListCons();

	symbol_t S;
	frequence_t F;

	// Get all values from dictionnary
	for (int i = 0; i < 128; i++)
	{
		if (dico[i] == 0)
			continue;

		S = i;
		F = dico[i];
		node_t *node = consNode(S, F);
		// printf(YELLOW "Caractere (%c|%d) de frequence %u \n" RESET, i, i, dico[i]);
		// printNode(node);

		// Insert newElement at the begining
		if (lHead->n == NULL)
		{
			list_t *newElement = listConstruct(node);
			lHead = listCpy(newElement);
			pred = NULL;
		}
		else
		{
			list_t *newElement = listConstruct(node);
			// Insert newElement at the begining
			newElement->suc = lHead;

			lHead = listCpy(newElement);

			// Sort the list
			while (newElement->suc != NULL)
			{
				if (newElement->n->F < newElement->suc->n->F)
				{
					// Create a copy of the suc which will move
					list_t *tmp = listCpy(newElement->suc);

					// Link newElement and his suc to each other
					newElement->suc->suc = newElement;
					newElement->suc = tmp->suc;

					// Update tmp for head
					tmp->suc = newElement;
					if (newElement->n == lHead->n)
						lHead = tmp;

					// Link the pred with the new value (suc of newElement)
					if (pred != NULL)
						pred->suc = tmp;

					// Update the new pred
					pred = tmp;
					pred->suc = tmp->suc;
				}
				else
					break;
			}
			// Reset pred for next insertion
			pred = NULL;
		}
	}

	// CREATION OF THE HUFFMAN TREE
	// if |A| == 1 stop
	while (lHead->suc != NULL)
	{
		// Get the 2 min nodes
		minNodes_t minNodes = getMin(lHead);

		// Give to mins a binary value (part of their code)
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
			printList(lHead);
		}
	}

	// CONSTRUCT THE CODE FOR EACH NODE
	lHead->n->code = (char *)malloc(sizeof(char));
	lHead->n->code = "";

	getCode(lHead->n, HuffmanDico);

	// Free all datas of the list
	while (lHead->n != NULL)
	{
		if (lHead->suc == NULL)
			break;
		lHead = destroyList(lHead);
	}
}
