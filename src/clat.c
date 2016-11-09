#include <stdlib.h>
#include <stdio.h>

#include "clat.h"

#include "sigsegv.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int clat_handler(void *fault_address, int serious)
{ 
    printf("handler procd\n");
    // if memory correctly released to be read/write, return 1
    if(mprotect(ginf.fd_mapped_addr, ginf.fd_page_multiple, PROT_READ_WRITE) == 0) {
        return 1;
    }
    return 0;
}

int clat_init()
{
    sigsegv_install_handler(&clat_handler);
    ginf.page_size = getpagesize();
    return 0;
}

void* clat_reserve(void* addr_hint, size_t map_size)
{
    unsigned long page, res;

    if(map_size % ginf.page_size != 0) {
        if(map_size < ginf.page_size) {
            ginf.page_multiple = ginf.page_size;
        } else {
            ginf.page_multiple = ginf.page_size * ((map_size / ginf.page_size) + 1);
        }
    } else {
        ginf.page_multiple = map_size;
    }

    // initially do not map any fd, mapping will occur in clat_map
    ginf.map_addr = (void *) 
        mmap(addr_hint, ginf.page_multiple, PROT_READ_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    page = (unsigned long) ginf.map_addr;

    // protect entire region of memory
    if((res = mprotect((void *) page, ginf.page_multiple, PROT_NONE)) < 0) {
        return (void *) res;
    }

    return ginf.map_addr;
}

void* clat_map(int fd, size_t size, off_t offset)
{ 
    struct stat sb;
    long int res;
    off_t pagealigned_offset;

    ginf.fd = fd;

    res = fstat(fd, &sb);
    // get stat information on fd
    if(res <= -1) {
        return (void *)res;
    }

    // get page aligned offset of file
    pagealigned_offset = offset & ~(ginf.page_size - 1);
    // if the aligned offset is greater than the filesize, error
    if(pagealigned_offset >= sb.st_size) {
        return (void *)-1;
    }
    // get page aligned size of file mapping
    if(size % ginf.page_size != 0) {
        if(size < ginf.page_size) {
            ginf.fd_page_multiple = ginf.page_size;
        } else {
            ginf.fd_page_multiple = ginf.page_size * ((size / ginf.page_size) + 1);
        }
    } else {
        ginf.fd_page_multiple = size;
    }

    ginf.fd_mapped_addr = (void *)
        mmap(ginf.map_addr, ginf.fd_page_multiple, PROT_READ_WRITE, 
             MAP_PRIVATE | MAP_FIXED, fd, pagealigned_offset); 

    if((res = mprotect(ginf.fd_mapped_addr, ginf.fd_page_multiple, PROT_NONE)) < 0) {
        return (void *) res;
    }

    return ginf.fd_mapped_addr;
}

