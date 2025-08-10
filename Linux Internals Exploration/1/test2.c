#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>  

int main(int argc, char *argv[]) {
    int allocation_size = 198 * 1024 * 1024;  // 198 MB

   
    char *memory = mmap(NULL, allocation_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
    if (memory == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    
    memset(memory, 0, allocation_size);

    printf("Pid is %d\n", getpid());
    printf("Allocated %d Kib of memory\n", allocation_size / 1024);

    
    getchar();

    
    munmap(memory, allocation_size);
    return EXIT_SUCCESS;
}
