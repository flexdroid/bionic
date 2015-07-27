/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>

#include "private/ErrnoRestorer.h"

// mmap2(2) is like mmap(2), but the offset is in 4096-byte blocks, not bytes.
extern "C" void*  __mmap2(void*, size_t, int, int, int, size_t);

#define MMAP2_SHIFT 12 // 2**12 == 4096

#ifdef CUSTOMIZED_MMAP
#define SECTION_SIZE (1 << 20) // 1 MB
#define CUSTOMIZED_MMAP_SIZE (128 * SECTION_SIZE)
#define MODULAR(ptr, size) ((unsigned long)(ptr) % size)
#define ROUND_UP(ptr, size) \
  (MODULAR(ptr, size) ? \
   (unsigned long)(ptr) - MODULAR(ptr, size) + size : (unsigned long)(ptr))
static void* __base = NULL;
static void* __end = NULL;
void* __init(void) {
  if (__base == NULL) {
    __base = __mmap2(NULL, CUSTOMIZED_MMAP_SIZE,
        PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    __end = (void*)ROUND_UP(__base, SECTION_SIZE);
    if (__end > __base)
      munmap(__base, (unsigned long)__end - (unsigned long)__base);

    /* because of mmap bug .. check allocation */
    for (size_t i = 0; i < 127; ++i) {
      *(int*)((size_t)__end+i*SECTION_SIZE) = 3;
    }

#if defined(__arm__)
    asm volatile(
        "push {r0, r7}\n"
        "mov r0, %[end]\n"
        "ldr r7, =0x17e\n"
        "svc #0\n"
        "pop {r0, r7}\n"
        : : [end] "r" (__end));
#endif
  }
  return __end;
}

void* mmap(void *addr, size_t length, int prot, int flags,
    int fd, off_t offset)
{
  if (!__end) __init();

  void* ret = __end;
  if (!(flags & MAP_FIXED))
    __end = (void*)((size_t)__end + ROUND_UP(length, PAGE_SIZE));

  if (addr) {
    assert((unsigned long)addr % PAGE_SIZE == 0);
    if (fd > 2 || (flags & MAP_FIXED)) {
      /* file mapped memory */
      ret = __mmap2(addr, length, prot, MAP_FIXED | flags, fd, offset);
      assert(ret == addr);
    } else {
      assert(!mprotect(addr, length, prot));
      ret = addr;
    }
  } else {
    if (fd > 2) {
      /* file mapped memory */
      void* ptr = __mmap2(ret, length, prot, MAP_FIXED | flags, fd, offset);
      assert(ptr == ret);
    } else {
      assert(!mprotect(ret, length, prot));
    }
  }
  return ret;
}
#else /* CUSTOMIZED_MMAP */
void* mmap(void* addr, size_t size, int prot, int flags, int fd, off_t offset) {
  if (offset & ((1UL << MMAP2_SHIFT)-1)) {
    errno = EINVAL;
    return MAP_FAILED;
  }

  size_t unsigned_offset = static_cast<size_t>(offset); // To avoid sign extension.
  void* result = __mmap2(addr, size, prot, flags, fd, unsigned_offset >> MMAP2_SHIFT);

  if (result != MAP_FAILED && (flags & (MAP_PRIVATE | MAP_ANONYMOUS)) != 0) {
    ErrnoRestorer errno_restorer;
    madvise(result, size, MADV_MERGEABLE);
  }

  return result;
}
#endif /* CUSTOMIZED_MMAP */
