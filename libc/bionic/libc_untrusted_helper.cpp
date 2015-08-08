#include <asm/page.h>
#include <bionic_tls.h>
#include <elf.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/auxv.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#include "atexit.h"
#include "private/bionic_auxv.h"
#include "private/bionic_ssp.h"
#include "private/KernelArgumentBlock.h"
#include "pthread_internal.h"

#if defined(PTHREAD_UNTRUSTED)
extern "C" void ut_set_envp(char** e) {
    environ = e;
}

extern "C" void ut_set_auxv(void* a) {
    __libc_auxv = (Elf32_auxv_t*)a;
}
#else
extern "C" char** ut_get_envp(void) {
    return environ;
}

extern "C" unsigned long long* ut_get_auxv(void) {
    return (unsigned long long*) __libc_auxv;
}
#endif
