#ifndef LIST_H
#define LIST_H

#include "TA.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct elem_list_t
{
	node_t *n;
	struct elem_list_t *suc;
} list_t;

typedef struct
{
	node_t *min1;
	node_t *min2;
} minNodes_t;

/**
 * @brief Construct a list element from a node
 *
 * @param N The node
 * @return list_t* The list element created
 */
list_t *listConstruct(node_t *N);

/**
 * @brief Constructor of an empty list
 *
 * @return list_t*
 */
list_t *emptyListCons();

/**
 * @brief Free a node (+code) and all his children (+code)
 *
 * @param node The node to start the free
 */
void freeNodes(node_t *node);

/**
 * @brief Remove 2 mins from the list
 *
 * @param L list_t* The list where we remove the mins
 * @param minNodes minNodes_t* Structure containing the mins
 *
 * @return list_t* The new head of the list
 */
list_t *removeMins(list_t *L, minNodes_t *minNodes);

/**
 * @brief Get the 2 min nodes
 *
 * @param L list_t* The list to search the mins
 * @return minNodes_t
 */
minNodes_t getMin(list_t *L);

/**
 * @brief Create a tring from the value of the node + suc
 *
 * @param L The list element to be transformed
 * @return char* The string
 */
char *toStringList(list_t *N);

/**
 * @brief Print only the list element given
 *
 * @param L The list element to be printed
 */
void printListElement(list_t *N);

/**
 * @brief Print each value of the list from the element given
 *
 * @param L The first value to be printed
 */
void printList(list_t *N);

/**
 * @brief Recursiv function to build the code of every node of the tree
 *
 * @param node The root of the tree
 * @param huffmanDico The dictionnary to save the code
 */
void getCode(node_t *node, char **encodeFile);

/**
 * @brief Take a string to reverse it
 *
 * @param code The string to reverse
 * @return char* The reversed string
 */
char *reverseCode(char *code);

#endif
