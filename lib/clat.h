#ifndef CLIB_H_
#define CLIB_H_

#define PROT_READ_WRITE (PROT_READ | PROT_WRITE)

#include <sys/types.h>

static struct global_info {
    int fd;
    int page_size;
    size_t page_multiple;
    void* map_addr;
    void* fd_offset_addr;
    size_t assigned_size;
    off_t fd_offset;
} ginf;

int clat_init();
int clat_handler(void *fault_address, int serious);

void* clat_reserve(void* addr_hint, size_t map_size);
void* clat_assign(int fd, size_t size, off_t offset);

#endif
