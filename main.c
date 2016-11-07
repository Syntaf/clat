#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "clib.h"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main() 
{
    int fd;
    size_t size;
    off_t offset;
    struct stat sb;
    void* map_addr;
    void* fd_addr;
    void* hint_addr = (void *)0x12340000;
    
    clib_init();
    map_addr = clib_reserve(hint_addr, 8192);

    fd = open("dat.txt", O_RDONLY);

    if(fstat(fd, &sb) == -1)
        handle_error("fstat");

    size = sb.st_size;
    offset = 0;

    fd_addr = clib_map(fd, offset, size);

    write(STDOUT_FILENO, fd_addr, size);
}



