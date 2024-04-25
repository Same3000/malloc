#include "page_begin.h"

void *page_begin(void *ptr, size_t page_size)
{
    size_t ptr_size_t = (size_t)ptr;
    size_t aligned_address = ptr_size_t & ~(page_size - 1);
    size_t offset = ptr_size_t - aligned_address;
    char *ret = ptr;
    return ret - offset;
}
