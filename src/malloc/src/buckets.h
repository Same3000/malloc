#ifndef BUCKETS_H
#define BUCKETS_H

#define _GNU_SOURCE
#include <stddef.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
struct free_list
{
    struct free_list *next; // next free block
};

struct bucket
{
    size_t bucket_size;
    void *free;
    struct bucket *next;
};

void *my_malloc(size_t size);
void my_free(void *ptr);
size_t get_size(void *ptr);
#endif /* BUCKETS_H */
