// measures time it takes to enter seg fault and exit, amount read is zero bytes

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include "clat.h"
#include "clock.h"

#include <sys/mman.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void create_segfault(void *fd_addr, int fd, void *c);

unsigned long seg_fault_ns;
unsigned long pread_ns;

int main(int argc, char** argv)
{
    int fd, i;
    size_t mapping_size = 0x4000;
    size_t size;
    off_t offset;
    struct stat sb;
    void *fd_addr;
    char c[1];
    char cline_buff[13];
    int j;

    int test_size = 100;

    if(argc == 1) {
        printf("Usage: ./measure_segfault --test-size=<size>\n");
        return 0;
    } else if (argc == 2) {
        strncpy(cline_buff, argv[1], 12);
        cline_buff[12] = '\0';
        printf("%s\n", cline_buff);
        if(strcmp(cline_buff, "--test-size=") == 0) {
            strncpy(cline_buff, argv[1] + 12, (strlen(argv[1])-12));
            cline_buff[strlen(argv[1])-12] = '\0';
            test_size = atoi(cline_buff);
        } else {
            printf("Invalid argument \"%s\"\n", cline_buff);
            return 0;
        }
    } else {
        printf("Too many arguments supplied\n");
        return 0;
    }

    printf("Running with test size: %d...\n", test_size);

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

    for(j = 0; j < test_size; j++) {
        create_segfault(fd_addr, fd, (void *)c);
        clat_clear();
    }

    pread_ns /= test_size;
    seg_fault_ns /= test_size;

    printf("Average time of a read call: %luns\n", pread_ns);
    printf("Average time taken to handle seg fault: %luns\n", seg_fault_ns);
}

void create_segfault(void *fd_addr, int fd, void *c) 
{
    struct timespec time1, time2, res;
    char tmp;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    tmp = *(char *)(fd_addr); 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    res = clock_difference(time1, time2);
    seg_fault_ns += res.tv_nsec;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    pread(fd, c, 0, 0);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    res = clock_difference(time1, time2);
    pread_ns += res.tv_nsec;
}
