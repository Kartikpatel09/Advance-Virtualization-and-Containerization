#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kartik");
MODULE_DESCRIPTION("Second Module");

static int startingfn(void){
    struct task_struct *pcb=current;
    printk("[LKM1] Runnable Processes:");
    printk("[LKM1] PID\t\tPROC");
    for_each_process(pcb){
        if(pcb->__state==TASK_RUNNING ){
            printk("[LKM1] %d\t\t%s\n",pcb->pid,pcb->comm);
        }
    }
    return 0;
}
static void endingfn(void){
    printk("Module LKM1 Unloaded\n");
}

module_init(startingfn);
module_exit(endingfn);