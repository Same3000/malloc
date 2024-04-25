#include <stddef.h>

#include "buckets.h"

__attribute__((visibility("default"))) void *malloc(size_t size)
{
    return my_malloc(size);
}

__attribute__((visibility("default"))) void free(void *ptr)
{
    my_free(ptr);
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    if (size == 0)
        return NULL;
    void *dst = my_malloc(size);
    size_t n = get_size(dst);
    if (n != 0 && ptr != NULL)
        memcpy(dst, ptr, n);
    my_free(ptr);
    return dst;
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    if (nmemb == 0 || size == 0)
        return NULL;
    size_t tmp_size = 0;
    if (__builtin_umull_overflow(size, nmemb, &tmp_size))
    {
        return NULL;
    }
    void *res = my_malloc(tmp_size);
    size_t n = get_size(res);
    memset(res, 0, n);
    return res;
}
