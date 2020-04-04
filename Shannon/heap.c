#include "avl_tree.h"
#include <string.h>
#include <stdint.h>
#include "node.h"
#include "heap.h"

#define SWAP_VOID(a, b) \
(*(a)) = (void *)((uintptr_t)*(a) ^ (uintptr_t)*(b));\
(*(b)) = (void *)((uintptr_t)*(a) ^ (uintptr_t)*(b));\
(*(a)) = (void *)((uintptr_t)*(a) ^ (uintptr_t)*(b))

int create_heap(struct heap * heap, const unsigned int element_size, const unsigned int weak_pointer, int (*print)(const void * const data), int (*compare)(const void * const first, const void * const second))
{
    if (heap == NULL)
    {
        return NULL_ARGUMENT;
    }

    /// INIT DATA
    heap->elements = NULL;
    heap->element_count = 0;
    heap->container_size = 0;
    heap->weak_pointer = weak_pointer;
    heap->element_size = element_size;

    /// INIT OPERATIONS
    heap->pop = &pop;
    heap->push = &push;
    heap->print = print;
    heap->compare = compare;
    heap->pop_free = &pop_free;
    heap->push_weak = &push_weak;
    heap->print_heap = &print_heap;

    return STATUS_SUCCESS;
}

int print(const void * const data)
{
    if (data != NULL)
    {
        srd((const struct node * const)data, print_node);
    }
    return STATUS_SUCCESS;
}

int print_heap(struct heap* const heap)
{
    if (heap != NULL && heap->elements != NULL)
    {
        int result;
        for (unsigned int i = 0; i < heap->element_count; ++i)
        {
            if ((result = heap->print(*(heap->elements + i))) != STATUS_SUCCESS)
            {
                return result;
            }
        }
    }
    return STATUS_SUCCESS;
}

int push_weak(struct heap * const heap, void ** const heap_node, void * const data)
{
    if (heap->weak_pointer == DEEP_COLLECTION)
    {
        if ((*heap_node = (void *)malloc(heap->element_size)) == NULL)
        {
            return BAD_MEMORY_ALLOC;
        }

        *heap_node = (void *)memcpy(*heap_node, data, heap->element_size);
    }
    else
    {
        *heap_node = data;
    }
    return STATUS_SUCCESS;
}

int push(struct heap * const heap, void * const data)
{
    if (heap == NULL || data == NULL)
    {
        return NULL_ARGUMENT;
    }

    if (heap->elements == NULL)
    {
        if ((heap->elements = (void **)malloc(sizeof(void *))) == NULL)
        {
            return BAD_MEMORY_ALLOC;
        }

        int result = heap->push_weak(heap, heap->elements, data);

        if (result != STATUS_SUCCESS)
        {
            return result;
        }

        ++heap->element_count;
        ++heap->container_size;

        return STATUS_SUCCESS;
    }
    else
    {
        if (heap->element_count == heap->container_size && (heap->elements = (void **)realloc(heap->elements, (heap->container_size <<= 1) * sizeof(void *))) == NULL)
        {
            return BAD_MEMORY_ALLOC;
        }

        int result = heap->push_weak(heap, heap->elements + heap->element_count, data);

        if (result != STATUS_SUCCESS)
        {
            return result;
        }

        ++heap->element_count;

        for (int i = heap->element_count - 1; i >= 0; i = (i - 1) / 2)
        {
            void **  child = heap->elements + i;
            void ** parent = heap->elements + (i - 1) / 2;

            if (heap->compare(*child, *parent) < 0)
            {
                SWAP_VOID(child, parent);
            }
            else
            {
                break;
            }
        }

        return STATUS_SUCCESS;
    }
}

int pop_free(struct heap * const heap)
{
    void * destroy;
    int result = heap->pop(heap, &destroy);

    if (result != STATUS_SUCCESS)
    {
        return result;
    }
    else if (heap->weak_pointer == DEEP_COLLECTION)
    {
        free(destroy);
    }

    return STATUS_SUCCESS;
}

int pop(struct heap * const heap, void ** const data)
{
    if (heap == NULL || heap->elements == NULL  || data == NULL)
    {
        return NULL_ARGUMENT;
    }

    if (heap->element_count == 1)
    {
        heap->container_size = 0;
        heap->element_count = 0;

        *data = *heap->elements;

        free(heap->elements);
        heap->elements = NULL;
    }
    else
    {
        /// SWAP THE FIRST ELEMENT WITH THE LAST
        {
            void ** last = heap->elements + heap->element_count - 1;
            {
                SWAP_VOID(heap->elements, last);
            }

            *data = *last;
            *last = NULL;
        }

        --heap->element_count;

        /// REALLOCATE MEMORY
        if (heap->element_count < heap->container_size / 4)
        {
            heap->elements = (void **)realloc(heap->elements, (heap->container_size >>= 1) * sizeof(void *));

            if (heap->elements == NULL)
            {
                return BAD_MEMORY_ALLOC;
            }
        }

        for (unsigned int i = 0; i < heap->element_count;)
        {
            void ** curre = heap->elements + i;
            void ** first = heap->elements + i * 2 + 1;

            if (i * 2 + 1 >= heap->element_count)
            {
                break;
            }
            else
            {
                if (i * 2 + 2 >= heap->element_count)
                {
                    if (heap->compare(*curre, *first) > 0)
                    {
                        SWAP_VOID(curre, first);
                        i = i * 2 + 1;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    if (heap->compare(*first, *(first + 1)) < 0)
                    {
                        if (heap->compare(*curre, *first) > 0)
                        {
                            SWAP_VOID(curre, first);
                            i = 2 * i + 1;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        if (heap->compare(*curre, *(first + 1)) > 0)
                        {
                            SWAP_VOID(curre, first + 1);
                            i = 2 * i + 2;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
        }
    }

    return STATUS_SUCCESS;
}

int clean_heap(struct heap * heap)
{
    if (heap != NULL)
    {
        if (heap->elements != NULL)
        {
            if (heap->weak_pointer == DEEP_COLLECTION)
            {
                for (unsigned int i = 0; i < heap->container_size; ++i)
                {
                    free(*(heap->elements + i));
                }
            }

            free(heap->elements);
            heap->elements = NULL;
        }

        heap->element_count = 0;
        heap->container_size = 0;
    }
    return STATUS_SUCCESS;
}

int compare_weights(const void * const first, const void * const second)
{
    if (first == NULL || second == NULL)
    {
        return NULL_ARGUMENT;
    }

    return ((struct node *)first)->info.total - (int) ((struct node *)second)->info.total;
}
