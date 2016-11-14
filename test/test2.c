/*
 * The following test will ensure simple file offsets work within clat. First, the
 * library will be initialized and ~16K bytes will be reserved for use. Afterwards,
 * a file (dat2.txt) is assigned to the reserved memory region with a specific offset.
 * The program then compares the actual contents of the file at that offset (via read) 
 * with the contents read in using clat's I/O process. If both are equal, the test succeeds
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
    size_t test_size;
    off_t offset;
    void *fd_addr;
    char *fd_data;

    // init and reserve memory
    clat_init();
    clat_reserve(NULL, mapping_size);

    fd = open("dat2.txt", O_RDONLY);

    offset = 15;

    // only test a set of 200 bytes
    test_size = 200;

    // get address assigned to fd, NOTE** no I/O 
    // has taken place yet, memory is still protected
    fd_addr = clat_assign(fd, test_size, offset);

    //*(char *)(fd_addr + 5) = 'T';

    if(fd_addr == MAP_FAILED) {
        printf("Test2 --> Failed (clat_assign)\n");
        exit(EXIT_FAILURE);
    }

    // read data from file the normal way
    fd_data = (char*)malloc(test_size * sizeof(char));
    pread(fd, fd_data, test_size, offset);

    // compare read data with fd_addr, upon the first access,
    // SIGSEGV will be sent which then performs file I/O
    if(verify_data(fd_addr, fd_data, test_size) == -1) {
        printf("Test2 --> Failed (verify_data)\n");
        exit(EXIT_FAILURE);
    }

    printf("Test2 --> Passed\n");

    free(fd_data);
}



