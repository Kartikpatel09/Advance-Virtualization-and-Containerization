#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kartik");
MODULE_DESCRIPTION("Physical Address space and Virtual Address space");

static int pid;
module_param(pid, int, 0); 
struct task_struct *find_task_by_pid(int pid)
{
    struct task_struct *task;
    task =pid_task(find_get_pid(pid),PIDTYPE_PID);
    return task;
}


static int startingfn(void)
{   struct task_struct *pcb;
    pcb=find_task_by_pid(pid);
    printk("[LKM4] Virtual Memory Size: %ld kiB",pcb->mm->total_vm*4);
   unsigned long paddr = atomic_long_read(&pcb->mm->rss_stat.count[MM_FILEPAGES]) 
                    + atomic_long_read(&pcb->mm->rss_stat.count[MM_ANONPAGES]) 
                    + atomic_long_read(&pcb->mm->rss_stat.count[MM_SWAPENTS]) 
                    + atomic_long_read(&pcb->mm->rss_stat.count[MM_SHMEMPAGES]);

    printk("[LKM4] Physical Memory Size: %ld kiB\n",paddr*4);
    
    return 0;
}


static void endingfn(void){
    printk("[LKM4] Module LKM4 Unloaded\n");
}

module_init(startingfn);
module_exit(endingfn);