#ifndef CLIB_H_
#define CLIB_H_

#define PROT_READ_WRITE (PROT_READ | PROT_WRITE)
#define map_flags MAP_FILE | MAP_PRIVATE

#include <sys/types.h>

static struct global_info {
    int fd;
    int page_size;
    size_t page_multiple;
} ginf;

int clib_init();
int handler(void *fault_address, int serious);
int reserve(size_t addr_size, int fd, void* addr);
void request(off_t offset, char* buff);

#endif
