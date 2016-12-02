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

#define SIZE 8192

double average_initial_read = 0;
double average_post_read = 0;
double average_sys_read = 0;
double average_total_sys_read = 0;
double average_total_clat_read = 0;

char clat_data[SIZE];
char read_data[SIZE];

struct timespec time1, time2, res;

void verify_data(char* a, char* b) 
{
    int i;
    for(i = 0; i < SIZE; i++) {
        if(a[i] != b[i]) {
            printf("VERIFY DATA ERROR\n");
            exit(0);
        }
    }
}

void test_read(int bytes_per_read, char* fd_addr, int fd) 
{
    int i, j;
    // measure first initial read    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    clat_data[0] = *(char *)(fd_addr);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    res = clock_difference(time1, time2);
    average_initial_read += res.tv_nsec;
    average_total_clat_read += res.tv_nsec;

    // measure singe read operation of bytes_per_read from read
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    read(fd, read_data, 1);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    res = clock_difference(time1, time2);
    average_sys_read += res.tv_nsec;
    average_total_sys_read += res.tv_nsec;

    // measure single read operation of bytes_per_read from clat interface
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    for(i = 1; i <= bytes_per_read; i++) {
        clat_data[i] = *(fd_addr + i);
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    res = clock_difference(time1, time2);
    average_post_read += res.tv_nsec;

    // measure overall performance
    for(i = 0; i < (SIZE / bytes_per_read); i++) {
        // perform read()    
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
        pread(fd, read_data + (i * bytes_per_read), bytes_per_read, (i * bytes_per_read));
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
        res = clock_difference(time1, time2);
        average_total_sys_read += res.tv_nsec;

        // perform clat read
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
        for(j = i; j < (i + bytes_per_read); j++) {
            clat_data[j] = *(fd_addr + j);
        }
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
        res = clock_difference(time1, time2);
        average_total_clat_read += res.tv_nsec;
    }
    verify_data(clat_data, read_data);
}

int main()
{
    int fd, i;
    size_t mapping_size = 0x4000;
    size_t size;
    off_t offset;
    struct stat sb;
    void *fd_addr;

    int test_size = 1;

    printf("Measuring large read of 8192 bytes, clat vs read()\n");

    clat_init();
    clat_reserve(NULL, mapping_size);

    fd = open("dat1.txt", O_RDONLY);

    if(fstat(fd, &sb) == -1)
        handle_error("fstat failed");

    size = 0x2000;  // read 8192 bytes
    offset = 0;

    fd_addr = clat_assign(fd, size, offset);
    if(fd_addr == MAP_FAILED) {
        handle_error("fd mapping failed");
    }

    for(i = 0; i < test_size; i++) {
        test_read(1, fd_addr, fd);
        //clat_clear();
    }

    average_initial_read /= test_size;
    average_post_read /= test_size;
    average_sys_read /= test_size;
    average_total_sys_read /= test_size;
    average_total_clat_read /= test_size;

    test_read(1, fd_addr, fd);
    printf("avg initial read(clat): %lf\navg post read(clat): %lf\navg sys read: %lf\ntotal sys read time: %lf\ntotal clat read time: %lf\n", average_initial_read, average_post_read, average_sys_read, average_total_sys_read, average_total_clat_read);
}
