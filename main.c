#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "sigsegv.h"
#include "clat.h"

#include <sys/mman.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int handler(void *fa, int s)
{
    g++;
    printf("here\n");
    return 0;
}

int main() 
{
    int fd;
    size_t mapping_size = 0x4000;
    size_t size;
    off_t offset;
    struct stat sb;
    void* map_addr;
    void* fd_addr;
    unsigned long t;

    // init and reserve memory
    clat_init();
    map_addr = clat_reserve(NULL, mapping_size);

    fd = open("dat.txt", O_RDONLY);

    if(fstat(fd, &sb) == -1)
        handle_error("fstat failed");

    size = sb.st_size;
    offset = 0;

    fd_addr = clat_map(fd, size, offset);

    if(fd_addr == MAP_FAILED) {
        handle_error("fd mapping failed");
    }
        
    fprintf(stdout, "map_addr: %p \nfd_addr: %p \n", map_addr, fd_addr);

    t = (unsigned long)fd_addr;

    size = (size_t)*(volatile int *)t;
    // write(STDOUT_FILENO, fd_addr, size);

    fprintf(stdout, "%d", g);
}



