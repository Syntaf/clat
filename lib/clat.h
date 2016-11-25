#ifndef CLIB_H_
#define CLIB_H_

#define PROT_READ_WRITE (PROT_READ | PROT_WRITE)

#include <sys/types.h>

static struct global_info {
    int fd;                     // file descriptor
    size_t fd_size;             // size of assigned memory passed by user
    off_t fd_offset;            // offset of file to perform I/O on
    size_t fd_aligned_size;     // page aligned size for use with mprotect 
    
    size_t page_multiple;       // page aligned size for total reserved memory
    void* map_addr;             // beginning address of reserved memory
    
    int page_size;              // constant filled at _init(), size of page
} ginf;

int clat_init();
int clat_handler(void *fault_address, int serious);

void* clat_reserve(void* addr_hint, size_t map_size);
void* clat_assign(int fd, size_t size, off_t offset);
int clat_clear();

#endif
