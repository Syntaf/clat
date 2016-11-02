#include <stdlib.h>
#include <stdio.h>

#include "sigsegv.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

int handler(void *fault_address, int serious)
{
    // do stuff
}

int clib_init()
{
    sigsegv_install_handler(&handler);
    ginf.page_size = getpagesize();
}

void reserve(size_t addr_size, int fd, void* map_addr)
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

    if(p == (void *)(-1)) {
        return -1;
    }
    
    page = (unsigned long) p;

    // protect memory so handler is always signaled upon request
    if(mprotect((void *) page, ginf.page_multiple, PROT_NONE) < 0) {
        return -1;
    }

}

void request(size_t offset, char* buff)
{

}
