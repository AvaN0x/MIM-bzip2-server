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
#include <math.h>

#include "count.h"

#define YELLOW "\x1B[33m"
#define RESET "\x1B[0m"

void buildCodeHuffman(int *frequencies, char **HuffmanDico)
{
	// Create the list
	list_t *lHead = NULL;
	list_t *pred = NULL;

	symbol_t S;
	frequence_t F;

	// Get all values from dictionnary
	for (int i = 0; i < 128; i++)
	{
		if (frequencies[i] == 0)
			continue;

		// printf(YELLOW "Caractere (%c|%d) de frequence %u \n" RESET, i, i, frequencies[i]);

		S = i;
		F = frequencies[i];
		node_t *node = consNode(S, F);
		node->lvl = 0;
		list_t *newElement = listConstruct(node);

		// Insert newElement at the begining
		if (lHead == NULL)
			lHead = newElement;
		else
		{
			// Insert newElement at the begining
			newElement->suc = lHead;
			lHead = newElement;

			// Sort the list
			while (newElement->suc != NULL)
			{
				// Swap if freq is lower than the next one
				if (newElement->n->F > newElement->suc->n->F || (newElement->n->S > newElement->suc->n->S && newElement->n->F >= newElement->suc->n->F))
				{
					list_t *next = newElement->suc;

					// link the pred with the suc of newElement
					if (pred != NULL)
						pred->suc = next;

					// link newElement with the suc of his suc
					newElement->suc = next->suc;

					// link the suc of newElement to newElement
					next->suc = newElement;

					// Condition to update lHead only 1 time
					if (newElement->n == lHead->n)
						lHead = next;

					// Update the new pred
					pred = next;
				}
				else
					break;
			}
			// Reset pred for the next insertion
			pred = NULL;
		}

		// Print all values of the list after each insertion
		if (0)
		{
			printf(YELLOW "\n\nPrint all values of the list : \n" RESET);
			printList(lHead);
			// sleep(5);
		}
	}

	// Print all values of the list
	if (0)
	{
		printf(YELLOW "\n\nPrint all values of the list : \n" RESET);
		printList(lHead);
	}

	// CREATION OF THE HUFFMAN TREE
	// if |A| == 1 stop
	while (lHead->suc != NULL)
	{
		// Get the 2 min nodes
		minNodes_t minNodes = getMin(lHead);

		if (0)
		{
			printf(YELLOW "mins to remove : \n" RESET);
			printf("min1 : %c|%d\n", minNodes.min1->S, minNodes.min1->F);
			printf("\t");
			printNode(minNodes.min1->up);
			printf("\t");
			printNode(minNodes.min1->down);
			printf("min2 : %c|%d\n", minNodes.min2->S, minNodes.min2->F);
			printf("\t");
			printNode(minNodes.min2->up);
			printf("\t");
			printNode(minNodes.min2->down);
		}

		// Give to mins a binary value (part of their code)
		// Malloc the code because we don't already know the future code's lenght of each node
		minNodes.min1->code = (char *)malloc(sizeof(char));
		globalCounter++;
		sprintf(minNodes.min1->code, "0");

		minNodes.min2->code = (char *)malloc(sizeof(char));
		globalCounter++;
		sprintf(minNodes.min2->code, "1");

		// Create the new node from the 2 min nodes. We don't care about the symbol given
		node_t *nNode = consNode('\0', minNodes.min1->F + minNodes.min2->F);

		// Save the new node node as father of min nodes
		nNode->down = minNodes.min1;
		nNode->up = minNodes.min2;
		nNode->lvl = fmax((int)nNode->down->lvl, (int)nNode->up->lvl) + 1;

		if (0)
		{
			printf("New node is : ");
			printNode(nNode);
		}

		// Remove the 2 mins of the list
		// printf("going to remove mins\n");
		lHead = removeMins(lHead, &minNodes);
		// printf(YELLOW "Mins have been removed, new list is : \n" RESET);
		// printList(lHead);

		list_t *newElement = listConstruct(nNode);
		// Save newElement as the head of the list

		// printf("Before lHead is : ");
		// printListElement(lHead);
		newElement->suc = lHead;
		lHead = newElement;

		// printf("New lHead is : ");
		// printListElement(lHead);
		// sleep(5);
		pred = NULL;
		// Insert newElement in the list
		while (newElement != NULL && newElement->suc != NULL)
		{
			// Swap if freq is lower than the next one
			if (newElement->n->F > newElement->suc->n->F || (newElement->n->S == newElement->suc->n->S && newElement->n->F >= newElement->suc->n->F))
			{
				list_t *next = newElement->suc;

				// link the pred with the suc of newElement
				if (pred != NULL)
					pred->suc = next;

				// link newElement with the suc of his suc
				newElement->suc = next->suc;

				// link the suc of newElement to newElement
				next->suc = newElement;

				// If newElement is the head, change the new Head
				if (newElement->n == lHead->n)
					lHead = next;

				// Update the new pred
				pred = next;
			}
			else
				break;
		}

		// Print all values of the list
		if (0)
		{
			printf(YELLOW "\n\nPrint all values of the list : \n" RESET);
			printList(lHead);
			// sleep(2);
		}
	}

	// Print all values of the list
	if (0)
	{
		printf(YELLOW "\n\nPrint all values of the list : \n" RESET);
		printList(lHead);
	}

	// CONSTRUCT THE CODE FOR EACH NODE
	lHead->n->code = "";

	getCode(lHead->n, HuffmanDico);

	// printf(YELLOW "\nFree all datas of the list : \n" RESET);
	// All values have already been freed but the head
	freeNodes(lHead->n);
	free(lHead);
	lHead = NULL;
	globalCounter--;
}

unsigned char *encodeHuffman(const char *str, char **HuffmanDico)
{
	const int usize = sizeof(unsigned char);
	int resSize = 0;

	unsigned char *res = (unsigned char *)malloc(usize * (resSize + 1));
	globalCounter++;
	res[resSize] = 0;
	int count = 0;

	FILE *binFile = fopen("res/huffmanEncoded.bin", "wb");
	assert(binFile != NULL);

	int lengthStr = strlen(str);
	for (int i = 0; i < lengthStr; i++)
	{
		// get the code of the current caracter of the string
		char *code = HuffmanDico[(int)str[i]];
		// printf("Code of %c(%d) is %s\n", str[i], str[i], code);

		int lengthCode = strlen(code);
		for (int j = 0; j < lengthCode; j++)
		{
			// Code is composed of '0' and '1' so we transform that in bit
			unsigned char myBitInt = code[j] - '0';

			res[resSize] <<= 1;
			res[resSize] |= myBitInt;

			count++;
			if (count == 8)
			{
				// printf(YELLOW "toSend is %u\n" RESET, res[resSize]);
				assert(fwrite(&res[resSize], usize, 1, binFile) == 1);

				res = (unsigned char *)realloc(res, usize * (++resSize + 1));
				res[resSize] = 0;

				count = 0;
			}
		}
	}

	if (count != 0)
	{
		// We want that the bits are high ordered and non low ordered
		for (int i = 0; i < 8 - count; i++)
			res[resSize] <<= 1;

		assert(fwrite(&res[resSize], usize, 1, binFile) == 1);
	}

	assert(fclose(binFile) == 0);

	// add \0 at the end
	res = (unsigned char *)realloc(res, usize * (++resSize + 1));
	res[resSize] = 0;

	return res;
}

char *decodeHuffman(unsigned char *str, char **HuffmanDico)
{
	FILE *binFile = fopen("res/huffmanEncoded.bin", "rb");
	// TODO USE PARAMETER STR
	assert(binFile != NULL);

	char *res = NULL;
	int resSize = 0;

	// All caracters are initially candidates
	int candidatList[128];
	for (int z = 0; z < 128; z++)
		candidatList[z] = 1;

	int nbCandidat = 128;
	int candidatIdx = 8128; // Sum of 0 to 127
	int cursor = -1;

	unsigned char buffer = 0;
	int usize = sizeof(unsigned char);
	int csize = sizeof(char);
	while (fread(&buffer, usize, 1, binFile) == 1)
	{
		// printf(YELLOW "\nbuffer is %d\n" RESET, buffer);
		for (int i = 7; i >= 0; i--)
		{
			// printf(YELLOW "I = %d\n" RESET, i);
			unsigned char mask = (unsigned char)pow(2.0, (double)i);
			// printf("Mask is %d\n", mask);
			unsigned char codeBit = (buffer & mask) >> i;
			// printf("codeBit is %d\n", codeBit);
			cursor++;
			// printf(YELLOW "cursor is %d\n" RESET, cursor);

			for (int j = 0; j < 128; j++)
			{
				// If char has a code
				if (HuffmanDico[j] != NULL)
				{
					// If char is candidate and his code is correct
					if (candidatList[j] == 1 && (HuffmanDico[j][cursor] - '0') != codeBit)
					{
						candidatList[j] = 0;
						nbCandidat--;
						candidatIdx -= j;
					}
				}
				else
				{
					if (candidatList[j])
					{
						candidatList[j] = 0;
						nbCandidat--;
						candidatIdx -= j;
					}
				}

				// debug
				// if (candidatList[j] == 1)
				// 	printf("%d|%c\n", j, j);

				if (nbCandidat == 1)
				{
					// printf("candidatIdx is %d(%c)\n", candidatIdx, candidatIdx);
					if (res == NULL)
					{
						res = (char *)malloc(csize);
						globalCounter++;
						res[resSize++] = (char)candidatIdx;
					}
					else
					{
						res = realloc(res, csize * (resSize + 1));
						res[resSize++] = (char)candidatIdx;
					}

					// reset all values necessary
					for (int z = 0; z < 128; z++)
						candidatList[z] = 1;
					nbCandidat = 128;
					candidatIdx = 8128;
					cursor = -1;

					// printf(YELLOW "res is now : %s\n" RESET, res);
					break;
				}
			}
		}
	}

	assert(fclose(binFile) == 0);

	res = realloc(res, csize * (resSize + 1));
	res[resSize] = '\0';

	return res;
}