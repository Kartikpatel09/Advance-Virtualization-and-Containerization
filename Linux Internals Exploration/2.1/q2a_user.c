#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct {
    int pid;
    int* vaddr;
    unsigned long paddr;
} info;
typedef struct {
    int pid;
    unsigned long paddr;
    int value;
} info1;
#define CONVERT _IOWR('m', 'a', info)
#define UPDATE _IOWR('m', 'b', info1)

#define DEVICE_PATH "/dev/va2pa"

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        perror("Failed to open device");
        return -1;
    }

    info addrInfo[2];
    info1 addrInfo1[2];

    unsigned char *p = (unsigned char*)malloc(10 * sizeof(unsigned char));  
    if (!p) {
        perror("Memory allocation failed");
        close(fd);
        return -1;
    }

    for (int i = 0; i < 10; i++) {
        *(p + i) = 104 + i;
    }

    printf("Va: %p value: %d\n", (void*)p, *p);
    printf("Va: %p value: %d\n", (void*)(p + 1), *(p + 1));

    printf("VA to PA translation\n");
    for (int i = 0; i < 2; i++) {
        addrInfo[i].pid = getpid();
        addrInfo[i].vaddr = p + i;  

        if (ioctl(fd, CONVERT, &addrInfo[i]) == -1) {
            perror("ioctl failed");
            close(fd);
            free(p);
            return -1;
        }

        printf("VA: %p \t PA: 0x%lx\n", (void*)addrInfo[i].vaddr, addrInfo[i].paddr);
        addrInfo1[i].paddr=addrInfo[i].paddr;
    }

    printf("PA: 0x%lx \t Value: %d\n", addrInfo1[0].paddr, *p);
    printf("PA: 0x%lx \t Value: %d\n", addrInfo1[1].paddr, *(p + 1));

    for (int i = 0; i < 2; i++) {
        addrInfo1[i].value = 53 + i;  
        if (ioctl(fd, UPDATE, &addrInfo1[i]) == -1) {
            perror("ioctl failed");
            close(fd);
            free(p);
            return -1;
        }

        printf("VA: %p \t PA: 0x%lx \t Updated Value: %d\n",
               (void*)addrInfo[i].vaddr, addrInfo1[i].paddr, *(p + i));
    }

    free(p);
    close(fd);
    return 0;
}
