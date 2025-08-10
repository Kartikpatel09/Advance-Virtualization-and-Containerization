#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>
#include <sys/syscall.h>
#include <fcntl.h>

#define errExit(msg)        \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

#define CHILD_STACK_SIZE 0x800000

int child_function(void *arg)
{

    int *pipefd = (int *)arg;

    close(pipefd[0]);

    const char *hostname = "Child1Hostname";
    char hostname_buf[32];
    if (sethostname(hostname, strlen(hostname)) == -1)
    {
        errExit("sethostname");
    }
    printf("Child1 Process PID: %d\n", getpid());
    gethostname(hostname_buf, 32);
    printf("Child1 Hostname: %s\n", hostname_buf);

    write(pipefd[1], "1", 1);
    close(pipefd[1]);

    while (1)
    {
        sleep(1);
    }

    return 0;
}

int child2_function()
{

    char hostname_buf[32];
    printf("Child2 Process PID: %d\n", getpid());
    gethostname(hostname_buf, 32);
    printf("Child2 Hostname: %s\n", hostname_buf);
    return 0;
}
int joinNamespace(char *file)
{
    int fd = open(file, O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening namespace file");
        return -1;
    }

    if (setns(fd, 0) == -1)
    {
        perror("Error while changing namespace");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
int main()
{

    char hostname_buf[32];
    char buf[32];

    void *child_stack = malloc(CHILD_STACK_SIZE);

    int pipefd[2];

    pid_t child_pid;

    if (pipe(pipefd) == -1)
    {
        errExit("pipe");
    }

    if (!child_stack)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    printf("----------------------------------------\n");
    printf("Parent Process PID: %d\n", getpid());
    gethostname(hostname_buf, 32);
    printf("Parent Hostname: %s\n", hostname_buf);
    printf("----------------------------------------\n");

    /**
     * 1. Create a new child process that runs child1_function
     * 2. The child process will have its own UTS and PID namespace
     * 3. You should pass the pointer to the pipefd array as an argument to the child1_function
     * 4. PID of child1 should be assigned to child_pid variable
     */

    // ------------------ WRITE CODE HERE ------------------

    child_pid = clone(child_function, child_stack + CHILD_STACK_SIZE, CLONE_NEWPID | CLONE_NEWUTS, pipefd);
    // printf("Child %d\n", child_pid);
    // -----------------------------------------------------

    close(pipefd[1]);
    read(pipefd[0], buf, 1);
    close(pipefd[0]);

    /**
     * You can write any code here as per your requirement
     * Note: PID namespace of a process will only change the PID namespace of its subsequent children, not the process itself.
     * You are allowed to make modifications to the parent process such that PID namespace of child2 is same as that of child1
     */

    // ------------------ WRITE CODE HERE ------------------
    /*Issue: setns() on a PID Namespace Affects Only Future Children
When you use setns() to join a PID namespace, it does not change the namespace of the calling process. Instead, it only ensures that future children of the calling process are created inside the new namespace.*/
    char file[25];
    sprintf(file, "/proc/%d/ns/pid", child_pid);
    if (joinNamespace(file) == -1)
    {
        return 0;
    }

    // -----------------------------------------------------

    printf("----------------------------------------\n");
    printf("Parent Process PID: %d\n", getpid());
    gethostname(hostname_buf, 32);
    printf("Parent Hostname: %s\n", hostname_buf);
    printf("----------------------------------------\n");

    if (fork() == 0)
    {

        sprintf(file, "/proc/%d/ns/uts", child_pid);
        if (joinNamespace(file) == -1)
        {
            return 0;
        }

        /**
         * 1. Join the existing UTS namespace and PID namespace
         */

        // ------------------ WRITE CODE HERE ------------------

        // -----------------------------------------------------

        child2_function();
        exit(0);
    }

    wait(NULL);
    kill(child_pid, SIGKILL);
    wait(NULL);

    printf("----------------------------------------\n");
    printf("Parent Process PID: %d\n", getpid());
    gethostname(hostname_buf, 32);
    printf("Parent Hostname: %s\n", hostname_buf);
    printf("----------------------------------------\n");

    free(child_stack);
    return 0;
}
