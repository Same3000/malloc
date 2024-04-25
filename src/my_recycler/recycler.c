#include "recycler.h"

#include <stdlib.h>

struct recycler *recycler_create(size_t block_size, size_t total_size)
{
    if (block_size % sizeof(size_t) != 0)
        return NULL;
    if (block_size == 0 || total_size == 0)
        return NULL;
    if (total_size % block_size != 0)
        return NULL;
    struct recycler *res = malloc(sizeof(struct recycler));
    res->block_size = block_size;
    res->chunk = malloc(total_size);
    res->capacity = total_size / block_size;
    if (res->chunk == NULL)
        return NULL;
    res->free = res->chunk;
    struct free_list *start = res->free;
    struct free_list *tmp = start;
    for (size_t i = 1; i < res->capacity; i++)
    {
        tmp->next = start + (i * block_size) / sizeof(struct free_list);
        tmp = tmp->next;
    }
    tmp = NULL;
    return res;
}

void recycler_destroy(struct recycler *r)
{
    if (r != NULL)
    {
        r->capacity = 0;
        free(r->chunk);
        free(r);
    }
}

void *recycler_allocate(struct recycler *r)
{
    if (r == NULL || r->free == NULL)
        return NULL;
    struct free_list *res = r->free;
    r->free = res->next;
    return res;
}
void recycler_free(struct recycler *r, void *block)
{
    if (r != NULL && block != NULL)
    {
        struct free_list *tmp = r->free;
        r->free = block;
        struct free_list *tmp2 = r->free;
        tmp2->next = tmp;
        r->capacity--;
    }
}
