#include <stdlib.h>
#include <stdio.h>

#include "sigsegv.h"
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#define PROT_READ_WRITE (PROT_READ|PROT_WRITE)
#define map_flags       MAP_FILE | MAP_PRIVATE

unsigned long page;

volatile int handler_called = 0;

int handler(void *fault_address, int serious)
{
    handler_called++;
    if(handler_called > 10)
        abort();
    if(fault_address != (void *)((page + 0x678) & ~(SIGSEGV_FAULT_ADDRESS_ALIGNMENT - 1)))
        abort();
    if(mprotect((void *)page, 0x4000, PROT_READ_WRITE) == 0)
        return 1;
    return 0;
}

void crasher(unsigned long p)
{
    *(volatile int *) (p + 0x678) = 42;
}

int main() 
{
    int prot_unwritable;
    void *p;

    int zero_fd = open("/dev/zero", O_RDONLY, 0644);

    prot_unwritable = PROT_READ;

    // mmap_zeromap( (void *) 0x12340000, 0x4000)
    p = (void *) mmap( (void *)0x12340000, 0x4000, PROT_READ_WRITE, map_flags, zero_fd, 0);

    if(p == (void *)(-1)) {
        fprintf(stderr, "mmap failed.\n");
        exit(2);
    }

    page = (unsigned long) p;

    if (mprotect((void *) page, 0x4000, prot_unwritable) < 0) {
        fprintf(stderr, "mprotect failed.\n");
        exit(2);
    }

    if(mprotect((void *)page, 0x4000, PROT_READ_WRITE) < 0 ||
        mprotect((void *)page, 0x4000, prot_unwritable) < 0) {
        fprintf(stderr, "mprotect failed.\n");
        exit(2);
    }

    sigsegv_install_handler(&handler);

    crasher(page);

    crasher(page);

    if (handler_called != 1)
        exit(1);

    printf("Test passed.\n");
    return 0;
}



