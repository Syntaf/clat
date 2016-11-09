#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "clib.h"

#include <sys/mman.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main() 
{
    int fd;
    size_t mapping_size = 0x4000;
    size_t size;
    off_t offset;
    struct stat sb;
    void* map_addr;
    void* fd_addr;

    // init and reserve memory
    clib_init();
    map_addr = clib_reserve(NULL, mapping_size);

    fd = open("dat.txt", O_RDONLY);

    if(fstat(fd, &sb) == -1)
        handle_error("fstat");

    size = sb.st_size;
    offset = 0;

    fd_addr = clib_map(fd, size, offset);

    if(fd_addr == MAP_FAILED) {
        fprintf(stderr, "Mapping failed!\n");
    }
        
    fprintf(stdout, "map_addr: %p \nfd_addr: %p \n", map_addr, fd_addr);

    write(STDOUT_FILENO, fd_addr, size);
}



