#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_
#include "node.h"

struct avl_tree
{
    /// THE ROOT OF THE TREE
    struct node* root;

    unsigned int total;

};

/**
*   @PARAMS
*   tree    - The tree in which the element will be searched
*   element - Element to be found
*   length  - In bytes
*   compare - Comparison function
*
*   @RETURN
*    NULL - No node was found with the same value as element
*   !NULL - Node containing the element is found
*/
struct node * const find_by_value(const struct avl_tree * const tree, const void * const element, const unsigned int length, int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int));

/**
*   @PARAMS
*   node    - Root of the tree
*   element - Element to be found
*   length  - In bytes
*   compare - Comparison function
*
*   @RETURN
*    NULL - No node was found with the same value as element
*   !NULL - Node containing the element is found
*/
struct node * const find_element(struct node * const node, const void * const element, const unsigned int length, int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int));

/**
*   @PARAMS
*   tree    - Memory address of the tree
*   element - New element to be added
*   length  - In bytes
*   compare - Comparison function
*
*   @RETURN
*   BAD_MEMORY_ALLOC  - Could not allocate memory
*   NULL_ARGUMENT     - Comparison function is NULL
*   NULL_ARGUMENT     - Element is NULL
*   NULL_ARGUMENT     - Tree is NULL
*   STATUS_SUCCESS    - Element successfully added
*/
int add_to_tree(struct avl_tree * tree, const void * const element, const unsigned int length, int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int));

/**
*   @PARAMS
*   node    - Memory address of the pointer to node/root
*   element - New element to be added
*   length  - In bytes
*   compare - Comparison function
*
*   @RETURN
*   BAD_MEMORY_ALLOC - Could not allocate memory for a new node
*   NULL_ARGUMENT    - Pointer to node/root is NULL
*   STATUS_SUCCESS   - Element successfully added
*/
int add_node(struct node ** node, const void * const element, const unsigned int length, int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int));

/**
*   @PARAMS
*   tree    - Memory address of the tree
*   index   - Shows the index/hash-code of the tree
*   printer - Pointer to function which prints a node
*/
void print_tree(const struct avl_tree * const tree, const unsigned int index, void (*printer)(const struct node * const));

/**
*   @PARAMS
*   node    - Memory address of the node/root
*   printer - Pointer to function which prints a node
*/
void print_nodes(const struct node * const node, void (*printer)(const struct node * const));

/**
*   @PARAMS
*   tree    - Memory address of the tree
*
*   @RETURN
*   NULL_ARGUMENT  - Address of tree is NULL
*   STATUS_SUCCESS - Tree was successfully destroyed.
*/
int clean_tree(struct avl_tree* tree);

/**
*   @PARAMS
*   node    - Pointer to the address of the node/root
*
*   @RETURN
*   NULL_ARGUMENT  - Address of node is NULL
*   STATUS_SUCCESS - Nodes were successfully destroyed
*/
int clean_nodes(struct node ** node);

/**
*   @PARAMS
*   node    - Memory address of the node/root
*/
void print_nodes_default(const struct node * const node);

#endif // _AVL_TREE_H_
