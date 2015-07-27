#define ONLY_MSPACES 1

#include "../upstream-dlmalloc/malloc.h"
#include "../upstream-dlmalloc/malloc.c"

mspace gMs = NULL;

extern "C" void init_malloc (void* base, size_t capacity) {
    if (!gMs)
        gMs = create_mspace_with_base(base, capacity, 1);
}

extern "C" void* malloc(size_t bytes) {
    if (!gMs) return NULL;
    return mspace_malloc(gMs, bytes);
}

extern "C" void free(void* mem) {
    if (!gMs) return;
    mspace_free(gMs, mem);
}

extern "C" void* calloc(size_t n_elements, size_t elem_size) {
    if (!gMs) return NULL;
    return mspace_calloc(gMs, n_elements, elem_size);
}

extern "C" void* realloc(void* oldMem, size_t bytes) {
    if (!gMs) return NULL;
    return mspace_realloc(gMs, oldMem, bytes);
}

extern "C" void* memalign(size_t alignment, size_t bytes) {
    if (!gMs) return NULL;
    return mspace_memalign(gMs, alignment, bytes);
}

extern "C" size_t malloc_usable_size(const void* mem) {
    if (!gMs) return 0;
    return mspace_usable_size(mem);
}

#ifndef PG_SZ
#define PG_SZ (1 << 20)
#endif
extern "C" void* valloc(size_t bytes) {
    if (!gMs) return NULL;
    return mspace_memalign(gMs, PG_SZ, bytes);
}

#ifndef SIZE_T_ONE
#define SIZE_T_ONE          ((size_t)1)
#endif
extern "C" void* pvalloc(size_t bytes) {
    if (!gMs) return NULL;
    return mspace_memalign(gMs, PG_SZ,
            (bytes + PG_SZ - SIZE_T_ONE) & ~(PG_SZ - SIZE_T_ONE));
}
