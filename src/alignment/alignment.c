#include "alignment.h"

size_t align(size_t size)
{
    size_t res = 0;
    size_t mod = size % sizeof(long double);
    if (mod == 0)
        return size;
    size_t i = sizeof(long double) - mod;
    if (__builtin_uaddl_overflow(i, size, &res))
        return 0;
    return res;
}
