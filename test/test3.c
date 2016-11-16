/*
 * ensures errors are appropriately handled within the library
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "sigsegv.h"
#include "clat.h"

#include <sys/mman.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main() 
{
    int fd, res;
    size_t mapping_size = 0x4000;
    void *addr;
    size_t size;
    off_t offset;

    // init and reserve memory
    clat_init();

    // ensure you cannot reserve 0 bytes
    addr = clat_reserve(NULL, 0);
    if(addr != (void *)-1) {
        printf("Test3 --> Failed (clat_reserve_zero != -1)\n");
        exit(EXIT_FAILURE);
    }

    // ensure you can reserve a normal amount
    addr = clat_reserve(NULL, mapping_size);
    if(addr == (void *)-1) {
        printf("Test3 --> Failed (clat_reserve == -1)\n");
        exit(EXIT_FAILURE);
    }

    fd = open("dat1.txt", O_RDONLY);

    offset = 0x1005;
    size = 0x1000;

    // should return -1 because offset is larger than size
    addr = clat_assign(fd, size, offset);
    if(addr != (void *)-1) {
        printf("Test3 --> Failed (clat_assign_bigger_offset != -1)\n");
        exit(EXIT_FAILURE);
    }

    // should be allowed
    offset = 0x0002;
    addr = clat_assign(fd, size, offset);
    if(addr == (void *)-1) {
        printf("Test3 --> Failed (clat_assign == -1)\n");
        exit(EXIT_FAILURE);
    }

    printf("Test3 --> Passed\n");
}



