#include "beware_overflow.h"

void *beware_overflow(void *ptr, size_t nmemb, size_t size)
{
    size_t tmp = 0;
    if (__builtin_umull_overflow(size, nmemb, &tmp))
        return NULL;
    char *res = ptr;
    res += tmp;
    return res;
}
