#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096 
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_pages> <stride>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_pages = atoi(argv[1]);  
    int stride = atoi(argv[2]); 
   // stride=stride*1024*1024;    

    
    int allocation_size = num_pages * PAGE_SIZE;
    char *memory = mmap(NULL, allocation_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (memory == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }
    printf("Pid is %d\n",getpid());
    fflush(stdout);
    printf("Allocated %d bytes of memory\n", allocation_size);

    
    for (int offset = 0; offset < allocation_size; offset += stride) {
        memory[offset] = 'J'; 
    }

    printf("Memory access completed with stride %d bytes\n", stride);
    getchar();
    munmap(memory, allocation_size);
    return EXIT_SUCCESS;
}
