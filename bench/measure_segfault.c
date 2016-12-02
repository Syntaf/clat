// measures time it takes to enter seg fault and exit, amount read is zero bytes

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "clat.h"
#include "clock.h"

#include <sys/mman.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void create_segfault(void *fd_addr, int fd, char c);

unsigned long seg_fault_ns;
unsigned long pread_ns;

int main()
{
    int fd, i;
    size_t mapping_size = 0x4000;
    size_t size;
    off_t offset;
    struct stat sb;
    void *fd_addr;
    char c;
    volatile int j;

    seg_fault_ns = pread_ns = 0;

    printf("Measuring seg fault handler time\n");

    clat_init();
    clat_reserve(NULL, mapping_size);

    fd = open("/mnt/ramdisk/dat1.txt", O_RDONLY);

    if(fstat(fd, &sb) == -1)
        handle_error("fstat failed");

    //size = sb.st_size;
    size = 0;
    offset = 0;

    fd_addr = clat_assign(fd, size, offset);
    if(fd_addr == MAP_FAILED) {
        handle_error("fd mapping failed");
    }

    create_segfault(fd_addr, fd, c);
    clat_clear();

    printf("%lu\n", pread_ns);
    printf("Time taken to handle seg fault: %luns\n\n", seg_fault_ns);
   
    seg_fault_ns = pread_ns = 0;
    create_segfault(fd_addr, fd, c);
    clat_clear();

    printf("%lu\n", pread_ns);
    printf("Time taken to handle seg fault: %luns\n\n", seg_fault_ns);
    
    seg_fault_ns = pread_ns = 0;
    create_segfault(fd_addr, fd, c);
    clat_clear();

    printf("%lu\n", pread_ns);
    printf("Time taken to handle seg fault: %luns\n\n", seg_fault_ns);

    for(j = 0; j < 10; j++) {
        create_segfault(fd_addr, fd, c);
        clat_clear();
    }

    pread_ns /= 10;
    seg_fault_ns /= 10;

    printf("%lu\n", pread_ns);
    printf("Time taken to handle seg fault: %luns\n", seg_fault_ns);
}

void create_segfault(void *fd_addr, int fd, char c) 
{
    struct timespec time1, time2, res;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    c = *(char *)(fd_addr); 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    res = clock_difference(time1, time2);
    seg_fault_ns += res.tv_nsec;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    pread(fd, c, 0, 0);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    res = clock_difference(time1, time2);
    pread_ns += res.tv_nsec;
}
