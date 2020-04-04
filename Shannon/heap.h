#ifndef _HEAP_H_
#define _HEAP_H_
#include "utilities.h"
#include <stdlib.h>

#define DEEP_COLLECTION 0
#define WEAK_COLLECTION 1

struct heap
{
    /// DATA CONTAINED
    void ** elements;
    unsigned int length;
    unsigned int element_size;
    unsigned int element_count;
    unsigned int container_size;

    /// BIT FIELD: TRUE / FALSE VALUES
    unsigned char weak_pointer : WEAK_COLLECTION;

    /// HEAP-OPERATIONS
    int (*push_weak)(struct heap * const heap, void ** const heap_node, void * const data);
    int (*compare)(const void * const first, const void * const second);
    int (*push)(struct heap * const heap, void * const data);
    int (*pop)(struct heap * const heap, void ** const data);
    int (*print_heap)(struct heap* const heap);
    int (*pop_free)(struct heap * const heap);
    int (*print)(const void * const data);
};

int print(const void * const data);

int clean_heap(struct heap * heap);

int pop_free(struct heap * const heap);

int print_heap(struct heap* const heap);

int pop(struct heap * const heap, void ** const data);

int push(struct heap * const heap, void * const data);

int compare_weights(const void * const first, const void * const second);

int push_weak(struct heap * const heap, void ** const heap_node, void * const data);

int create_heap(struct heap * heap, const unsigned int element_size, const unsigned int weak_pointer, int (*print)(const void * const data), int (*compare)(const void * const first, const void * const second));

#endif // _HEAP_H_
