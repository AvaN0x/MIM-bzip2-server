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

#define YELLOW "\x1B[33m"
#define RESET "\x1B[0m"

// TODO TO FREE : EACH ELEMENT OF THE LIST

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
				if (newElement->n->F < newElement->suc->n->F)
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

		// printf(YELLOW "mins retrieved : \n" RESET);
		// printf("min1 : %c|%d\n", minNodes.min1->S, minNodes.min1->F);
		// printf("min2 : %c|%d\n", minNodes.min2->S, minNodes.min2->F);

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

	// Print all values of the list
	if (0)
	{
		printf(YELLOW "\n\nPrint all values of the list : \n" RESET);
		printList(lHead);
	}

	// CONSTRUCT THE CODE FOR EACH NODE
	lHead->n->code = (char *)malloc(sizeof(char));
	lHead->n->code = "";

	getCode(lHead->n, HuffmanDico);

	printf(YELLOW "\nFree all datas of the list : \n" RESET);
	// All values have already been freed but the head
	freeNodes(lHead->n);
	free(lHead);
}

unsigned char *encodeHuffman(char *str, char **HuffmanDico)
{
	printf("in encode\n");
	for (int i = 0; i < 128; i++)
	{
		if (HuffmanDico[i] != NULL)
			printf("%d|%c : %s\n", i, i, HuffmanDico[i]);
	}

	unsigned char *res = (unsigned char *)malloc(sizeof(unsigned char));
	res[0] = 0;
	int size = 0;
	// printf("String given is : %s\n", str);
	// The byte that will be saved
	unsigned char toSend = 0;
	int count = 0;
	FILE *binFile = fopen("res/huffmanEncoded.bin", "wb");
	assert(binFile != NULL);

	for (int i = 0; i < strlen(str); i++)
	{
		// get the code of the current caracter of the string
		char *code = HuffmanDico[(int)str[i]];
		// printf("Code of %c(%d) is %s\n", str[i], str[i], code);

		for (int j = 0; j < strlen(code); j++)
		{
			// printf("toSend was : %d\n", toSend);
			int myBitInt = code[j] - '0'; // Code is composed of '0' and '1' so we transform that in bit
			// printf("current bit is %d\n", myBitInt);
			toSend <<= 1;
			toSend |= myBitInt;
			// printf("toSend is now : %d(%c)\n", toSend, toSend);
			count++;
			if (count == 8)
			{
				count = 0;
				// printf("count = 8, write toSend into file\n");
				assert(fwrite(&toSend, sizeof(unsigned char), 1, binFile) == 1);

				res = (unsigned char *)realloc(res, (1 + size) * sizeof(unsigned char));
				res[size++] = toSend;
				toSend = 0;
			}
			// printf("\n");
		}
		// printf("\n");
	}
	if (count != 0)
	{
		// printf("count is %d\n", count);
		for (int i = 0; i < count; i++)
			toSend <<= 1;
		// printf("count != 0, write toSend into file\n");
		assert(fwrite(&toSend, sizeof(unsigned char), 1, binFile) == 1);

		res = (unsigned char *)realloc(res, (1 + size) * sizeof(unsigned char));
		res[size++] = toSend;
	}
	assert(fclose(binFile) == 0);
	return res;
}

char *decodeHuffman(char *todo, char **HuffmanDico)
{
	FILE *binFile = fopen("res/huffmanEncoded.bin", "rb");
	assert(binFile != NULL);

	char *res = NULL;

	// All caracters are initially candidates
	int candidatList[128];
	for (int z = 0; z < 128; z++)
		candidatList[z] = 1;

	int nbCandidat = 128;
	int candidatIdx = 8128; // Sum of 0 to 127
	int cursor = -1;

	unsigned char buffer = 0;
	while (fread(&buffer, sizeof(unsigned char), 1, binFile) == 1)
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
				if (HuffmanDico[j] != NULL)
				{
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
				// 	printf("%d\n", j);

				if (nbCandidat == 1)
				{
					if (res == NULL)
					{
						res = (char *)malloc(sizeof(char));
						res[0] = (char)candidatIdx;
					}
					else
					{
						// printf("strlen(res) is : %ld\n", strlen(res));
						res = realloc(res, (strlen(res) + 1) * sizeof(char));
						res[strlen(res)] = (char)candidatIdx;
						// printf("strlen(res) is : %ld\n", strlen(res));
					}

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
	return res;
}