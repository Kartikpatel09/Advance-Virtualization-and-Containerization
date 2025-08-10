#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>

// Include header or define the IOCTL call interface and devide name
typedef struct
{
    pid_t pid;
    pid_t ppid;
} info;
info processInfo;
#define UPDATE _IOWR('x', 'a', info)
#define DEVICE_PATH "/dev/dr_bloom"

//**************************************************

int open_driver(const char *driver_name)
{

    int fd_driver = open(driver_name, O_RDWR);
    if (fd_driver == -1)
    {
        perror("ERROR: could not open driver");
    }

    return fd_driver;
}

void close_driver(const char *driver_name, int fd_driver)
{

    int result = close(fd_driver);
    if (result == -1)
    {
        perror("ERROR: could not close driver");
    }
}

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        printf("Usage: %s <parent_pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t parent_pid = atoi(argv[1]);

    // open ioctl driver
    int fd = open_driver(DEVICE_PATH);
    processInfo.pid = getpid();
    processInfo.ppid = parent_pid;
    // call ioctl with parent pid as argument to change the parent
    printf("[CHILD]: soldier %d changing its parent\n",getpid());
    if (ioctl(fd, UPDATE, &processInfo))
    {
        perror("ioctl failed");
        close(fd);

        return -1;
    }
    

    // close ioctl driver
    close_driver(DEVICE_PATH, fd);
    while (1)
    {
        sleep(1);
    }

    return EXIT_SUCCESS;
}
