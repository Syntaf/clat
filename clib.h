#ifndef CLIB_H_
#define CLIB_H_

#define PROT_READ_WRITE (PROT_READ | PROT_WRITE)
#define map_flags MAP_FILE | MAP_PRIVATE

static struct global_info {
    int fd;
    int page_size;
    size_t page_multiple;
} ginf;

int handler(void *fault_address, int serious);
void reserve(size_t addr_size, int fd, void* addr);
void request(size_t offset, char* buff);

#endif
