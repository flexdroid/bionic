#ifndef _CUSTOM_MALLOC_H
#define _CUSTOM_MALLOC_H

/* Configure dlmalloc. */
#define ONLY_MSPACES 1

/* Include the proper definitions. */
#include "../upstream-dlmalloc/malloc.h"

void* ut_malloc(size_t bytes);
void ut_free(void* mem);
void* ut_calloc(size_t n_elements, size_t elem_size);
void* ut_realloc(void* oldMem, size_t bytes);
void* ut_memalign(size_t alignment, size_t bytes);
size_t ut_malloc_usable_size(const void* mem);
void* ut_valloc(size_t bytes);
void* ut_pvalloc(size_t bytes);
int ut_posix_memalign(void** memptr, size_t alignment, size_t size);
struct mallinfo ut_mallinfo();

#endif  // _CUSTOM_MALLOC_H
