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
    // if fault address is outside the assigned memory region, we do not want to handle it
    if((unsigned long)fault_address < (unsigned long)ginf.map_addr ||
       (unsigned long)fault_address > (unsigned long)(ginf.map_addr + ginf.fd_size)) {
        write(STDOUT_FILENO, "fault_address lies beyond assigned memory...\n", 45);
        return 0;
    }

    // unprotect memory at aligned offset, assigned_size is a page aligned value
    if(mprotect(ginf.map_addr, ginf.fd_aligned_size, PROT_READ_WRITE) == 0) {
        pread(ginf.fd, ginf.map_addr, ginf.fd_aligned_size, ginf.fd_offset);
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
    if(map_size == 0) {
        return (void *)-1;
    }

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

    // protect entire region of memory
    if(mprotect(ginf.map_addr, ginf.page_multiple, PROT_NONE) < 0) {
        return (void *) -1;
    }

    return ginf.map_addr;
}

void* clat_assign(int fd, size_t size, off_t offset)
{ 
    struct stat sb;
    long int res;
    size_t aligned_size;
    off_t aligned_offset;

    // error checks
    if(size == 0) return (void *)-1;
    if(size > ginf.page_multiple) return (void *)-1;
    if(fstat(fd, &sb) == -1) return (void *)-1;
    if(offset >= sb.st_size) return (void *)-1;

    aligned_size = ((size - 1) & ~(ginf.page_size - 1)) + ginf.page_size;
    //aligned_offset = (offset) & ~(ginf.page_size - 1);

    // store file descriptor, aligned size, and file offset
    ginf.fd = fd;
    ginf.fd_aligned_size = aligned_size;
    ginf.fd_size = size;
    ginf.fd_offset = offset;
    
    // only return beginning of reserved space
    return ginf.map_addr;

}

int clat_clear()
{
    // protect entire region of memory
    if(mprotect(ginf.map_addr, ginf.page_multiple, PROT_NONE) < 0) {
        return -1;
    }

    return 0;
}

