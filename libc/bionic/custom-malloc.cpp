#include "custom-malloc.h"
#include "../upstream-dlmalloc/malloc.c"

mspace gMs = NULL;

extern "C" void init_malloc (void* base, size_t capacity) {
    if (!gMs)
        gMs = create_mspace_with_base(base, capacity, 1);
}

void* ut_malloc(size_t bytes) {
    if (!gMs) return NULL;
    return mspace_malloc(gMs, bytes);
}

void ut_free(void* mem) {
    if (!gMs) return;
    mspace_free(gMs, mem);
}

void* ut_calloc(size_t n_elements, size_t elem_size) {
    if (!gMs) return NULL;
    return mspace_calloc(gMs, n_elements, elem_size);
}

void* ut_realloc(void* oldMem, size_t bytes) {
    if (!gMs) return NULL;
    return mspace_realloc(gMs, oldMem, bytes);
}

void* ut_memalign(size_t alignment, size_t bytes) {
    if (!gMs) return NULL;
    return mspace_memalign(gMs, alignment, bytes);
}

size_t ut_malloc_usable_size(const void* mem) {
    if (!gMs) return 0;
    return mspace_usable_size(mem);
}

#ifndef PG_SZ
#define PG_SZ (1 << 20)
#endif
void* ut_valloc(size_t bytes) {
    if (!gMs) return NULL;
    return mspace_memalign(gMs, PG_SZ, bytes);
}

#ifndef SIZE_T_ONE
#define SIZE_T_ONE          ((size_t)1)
#endif
void* ut_pvalloc(size_t bytes) {
    if (!gMs) return NULL;
    return mspace_memalign(gMs, PG_SZ,
            (bytes + PG_SZ - SIZE_T_ONE) & ~(PG_SZ - SIZE_T_ONE));
}

int ut_posix_memalign(void** memptr, size_t alignment, size_t bytes) {
    if (!gMs) return 1;
    if (!memptr) return 1;
    *memptr = mspace_memalign(gMs, alignment, bytes);
    if (!*memptr) return 1;
    return 0;
}

struct mallinfo ut_mallinfo() {
    return mspace_mallinfo(gMs);
}
