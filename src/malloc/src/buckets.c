#include "buckets.h"

struct bucket *buckets = NULL;

static void aux2(struct bucket *b)
{
    if (buckets == NULL)
        buckets = b;
    else
    {
        struct bucket *tmp = buckets;
        while (tmp->next != NULL)
            tmp = tmp->next;
        tmp->next = b;
    }
}
static void aux3(struct bucket *b)
{
    struct bucket *tmp = buckets;
    if (tmp == b)
        buckets = b->next;
    else
    {
        while (tmp->next != NULL && tmp->next != b)
            tmp = tmp->next;
        tmp->next = b->next;
    }
}
static size_t find_pow2(size_t size)
{
    size_t res = 1;
    while (res < size)
        res = res << 1;
    return res;
}
static size_t align(size_t size, size_t alignement)
{
    size_t mod = size % alignement;
    if (mod == 0)
        return size;
    size_t i = alignement - mod;
    return i + size;
}
static struct bucket *create_bucket(size_t size)
{
    size_t size_page = sysconf(_SC_PAGESIZE);
    size_t nec = sizeof(void *) + sizeof(size_t) + sizeof(struct bucket *);
    nec = find_pow2(nec);
    size_t size_tot = find_pow2(size + nec);
    size = find_pow2(size);
    if (size_tot > size_page)
        size_page = align(size_tot, size_page);
    void *void_res = mmap(NULL, size_page, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    struct bucket *res = void_res;
    res->next = NULL;
    res->bucket_size = size;
    char *res_char = void_res;
    res_char += nec;
    res->free = res_char; // + nec;
                          // char *start_char = res;
    struct free_list *f = res->free;
    for (size_t i = 1; i < (size_page - nec) / size; i++)
    {
        res_char += size;
        void *res_char_void = res_char;
        struct free_list *k = res_char_void;
        f->next = k;
        f = f->next;
    }
    f = NULL;
    aux2(res);
    return res;
}
static void *allocate(struct bucket *b)
{
    if (b == NULL)
        return NULL;
    struct free_list *res = b->free;
    b->free = res->next;
    return res;
}
static int is_full(struct bucket *b)
{
    if (b->free == NULL)
        return 1;
    return 0;
}
void *my_malloc(size_t size)
{
    if (size == 0)
        return NULL;
    size_t len = find_pow2(size);
    struct bucket *tmp = buckets;
    while (tmp != NULL)
    {
        if (tmp->bucket_size >= len && !is_full(tmp))
            return allocate(tmp);
        tmp = tmp->next;
    }
    void *res = create_bucket(size);
    return allocate(res);
}
static void *page_begin(void *ptr, size_t page_size)
{
    size_t ptr_size_t = (size_t)ptr;
    size_t aligned_address = ptr_size_t & ~(page_size - 1);
    size_t offset = ptr_size_t - aligned_address;
    char *ret = ptr;
    return ret - offset;
}

static void aux_free(struct bucket *b, void *block)
{
    if (b != NULL && block != NULL)
    {
        struct free_list *tmp = b->free;
        b->free = block;
        struct free_list *tmp2 = b->free;
        tmp2->next = tmp;
    }
}
static int check_to_destroy(struct bucket *b, size_t tot)
{
    size_t i = 0;
    struct free_list *f = b->free;
    size_t nec = sizeof(void *) + sizeof(size_t) + sizeof(struct bucket *);
    size_t len = (tot - nec) / b->bucket_size;
    while (i < len && f != NULL)
    {
        struct free_list *f2 = f->next;
        f = f2;
        i++;
    }
    if (i >= len)
        return 1;
    return 0;
}
void my_free(void *ptr)
{
    if (ptr == NULL)
        return;
    size_t size_page = sysconf(_SC_PAGESIZE);
    void *b = page_begin(ptr, size_page);
    struct bucket *buck = b;
    if (buck->bucket_size == 0)
        return;
    if (buck->bucket_size > size_page)
        size_page = buck->bucket_size;
    aux_free(buck, ptr);
    if (check_to_destroy(buck, size_page) == 1)
    {
        aux3(buck); // update buckets
        munmap(ptr, size_page);
    }
}
size_t get_size(void *ptr)
{
    if (ptr == NULL)
        return 0;
    size_t size_page = sysconf(_SC_PAGESIZE);
    void *b = page_begin(ptr, size_page);
    struct bucket *buck = b;
    return buck->bucket_size;
}
