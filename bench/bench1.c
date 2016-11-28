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

int main()
{
    int fd, i;
    size_t mapping_size = 0x4000;
    size_t size;
    off_t offset;
    struct stat sb;
    void *fd_addr;
    struct timespec time1, time2, res;
    char c[5];

    printf("Measuring single read of 5 bytes, clat vs read()\n");

    clat_init();
    clat_reserve(NULL, mapping_size);

    fd = open("dat1.txt", O_RDONLY);

    if(fstat(fd, &sb) == -1)
        handle_error("fstat failed");

    size = sb.st_size;
    offset = 0;

    fd_addr = clat_assign(fd, size, offset);
    if(fd_addr == MAP_FAILED) {
        handle_error("fd mapping failed");
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    for(i = 0; i < 5; i++) {
        c[i] = *(char *)(fd_addr + i);
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);

    res = clock_difference(time1, time2);
    fprintf(stdout, "clat: %lus %luns\n", res.tv_sec, res.tv_nsec);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    read(fd, c, 5);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);

    res = clock_difference(time1, time2);
    fprintf(stdout, "read: %lus %luns\n", res.tv_sec, res.tv_nsec);
}
