#include "allocator.h"

#include <err.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

struct blk_allocator *blka_new(void)
{
    struct blk_allocator *res = malloc(sizeof(struct blk_allocator));
    res->meta = NULL;
    return res;
}

struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size)
{
    long size_page = sysconf(_SC_PAGESIZE);
    if (size_page == -1)
        return NULL;
    size_t tmpsize = size + sizeof(struct blk_meta);
    size_t nb = tmpsize / (size_page + 1);
    nb++;
    void *res = mmap(NULL, size_page * nb, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (res == MAP_FAILED)
        return NULL;
    blka->meta = res;
    blka->meta->size = (size_page * nb) - sizeof(struct blk_meta);
    blka->meta->next = NULL;
    return blka->meta;
}

void blka_free(struct blk_meta *blk)
{
    if (blk != NULL)
    {
        size_t len = blk->size;
        if (munmap(blk, len) == -1)
            errx(1, "Error free");
    }
}

void blka_pop(struct blk_allocator *blka)
{
    if (blka != NULL && blka->meta != NULL)
    {
        struct blk_meta *tmp = blka->meta->next;
        blka_free(blka->meta);
        blka->meta = tmp;
    }
}
void blka_delete(struct blk_allocator *blka)
{
    if (blka->meta != NULL)
    {
        while (blka->meta != NULL)
        {
            blka_pop(blka);
        }
    }
    free(blka);
}
