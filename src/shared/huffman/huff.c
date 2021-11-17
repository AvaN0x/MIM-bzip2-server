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

void encodeHuffman(char * file)
{
	// Create the list
	list_t *list = emptyListCons();
	list_t *lHead = emptyListCons();

	symbol_t S;
	frequence_t F;

	FILE *openFile = fopen(file, "r");
	free(file);
	assert(openFile != NULL); /// Préférer l'utilisation des assertions.

	/**
	 * fscanf returns the number of arguments it's scanned
	 * if none, it returns -1 (known as EOF)
	 */
	int T;
	while( (T= fscanf(openFile," %c %d",&S,&F)) == 2 )
	{
		// Print the value read
		printf(YELLOW " %d :: Caractere %c de frequence %u \n" RESET, T, S, F);

		/** construction du noeud */
		node_t *node = consNode(S, F);

		/** insertion du noeud dans la liste */
		if (list->n == NULL) {
			setNode(list, node);
			lHead->n = node;
		}
		else {
			list = insNode(node, list);
			if (lHead->suc == NULL)
				lHead->suc = list;
		}

		// printNode(node);
		// printList(list);
	}
	fclose(openFile);
	printf("%d == %d\n", T, EOF);


	// Free all datas of the list
	while(lHead->suc != NULL)
		lHead = destroyList(lHead);
	// Destroy the last one
	destroyList(lHead);

	
}
