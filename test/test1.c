/*
 * The following test will performs the most basic function of the clat library.
 * First, the library will be initialized and ~16K bytes will be reserved for use.
 * Then, a file (dat1.txt) is assigned to the reserved memory region with no offset.
 * The program then compares the actual contents of the file (via read) with the
 * contents read in using clat's I/O process. If both are equal, the test succeeds
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

int verify_data(char *d1, char *d2, size_t size) {
    int i;

    for(i = 0; i < size; i++) {
        if(d1[i] != d2[i]) return -1;
    }

    return 0;
}

int main() 
{
    int fd, res;
    size_t mapping_size = 0x4000;
    size_t size;
    off_t offset;
    struct stat sb;
    void *fd_addr;
    char *fd_data;

    // init and reserve memory
    clat_init();
    clat_reserve(NULL, mapping_size);

    fd = open("dat1.txt", O_RDONLY);

    if(fstat(fd, &sb) == -1)
        handle_error("fstat failed");

    size = sb.st_size;
    offset = 0;

    // get address assigned to fd, NOTE** no I/O 
    // has taken place yet, memory is still protected
    fd_addr = clat_assign(fd, size, offset);

    if(fd_addr == MAP_FAILED) {
        handle_error("fd mapping failed");
    }

    // read data from file the normal way
    fd_data = (char*)malloc(size * sizeof(char));
    read(fd, fd_data, size);

    // compare read data with fd_addr, upon the first access,
    // SIGSEGV will be sent which then performs file I/O
    if(verify_data(fd_addr, fd_data, size) == -1) {
        printf("Test2 --> Failed\n");
    }

    printf("Test1 --> Passed\n");

    free(fd_data);
}



