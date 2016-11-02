#include <stdlib.h>
#include <stdio.h>

#include "clib.h"

#include "sigsegv.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

int handler(void *fault_address, int serious)
{
    // do stuff
    return 0;
}

int clib_init()
{
    sigsegv_install_handler(&handler);
    ginf.page_size = getpagesize();
    return 0;
}

int reserve(size_t addr_size, int fd, void* map_addr)
{
    unsigned long page;

    // check if reservation size request will be page aligned
    if(addr_size % ginf.page_size != 0) {
        if(addr_size < ginf.page_size) {
            ginf.page_multiple = ginf.page_size;
        } else {
            ginf.page_multiple = ginf.page_size * ((addr_size / ginf.page_size) + 1);
        }
    } else {
        // otherwise, we know addr_size is a page multiple already
        ginf.page_multiple = addr_size;
    }

    // map region of memory
    map_addr = (void *) mmap(NULL, ginf.page_multiple, PROT_READ_WRITE, map_flags, fd, 0);

    if(map_addr == (void *)(-1)) {
        return -1;
    }
    
    page = (unsigned long) map_addr;

    // protect memory so handler is always signaled upon request
    if(mprotect((void *) page, ginf.page_multiple, PROT_NONE) < 0) {
        return -1;
    }

    return 0;
}

void request(off_t offset, char* buff)
{
    off_t pagealigned_offset;

    pagealigned_offset = offset & ~(ginf.page_size - 1);

    fprintf(stdout, "%ld -> %ld\n", offset, pagealigned_offset);
}
