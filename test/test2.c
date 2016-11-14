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

void process_data(char* data, size_t size) 
{
    int i;
    for(i = 0; i < size; i++) {
        printf("%c", *(data + i));
    }
}

int main() 
{
    int fd, res;
    size_t mapping_size = 0x4000;
    size_t size;
    off_t offset;
    struct stat sb;
    void* map_addr;
    void* fd_addr;
    unsigned long t;
    char *my_data;

    // init and reserve memory
    clat_init();
    clat_reserve(NULL, mapping_size);

    fd = open("dat2.txt", O_RDONLY);

    if(fstat(fd, &sb) == -1)
        handle_error("fstat failed");

    size = 55;
    offset = 5;

    fd_addr = clat_assign(fd, size, offset);
    fd_addr = clat_assign(fd, 4000, 4190);
    fd_addr = clat_assign(fd, 7912, 312);
    fd_addr = clat_assign(fd, 0x4000, 0x2000);
    if(fd_addr == MAP_FAILED) {
        handle_error("fd mapping failed");
    }
       
    // get beginning of mapped file
    my_data = (char*)fd_addr;

    // process data
    //process_data(my_data, size);
    
    //write(STDOUT_FILENO, fd_addr, size);
    //err = errno;
}



