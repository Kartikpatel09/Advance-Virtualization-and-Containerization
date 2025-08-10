#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/moduleparam.h>
#include <linux/pid.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KARTIK");
MODULE_DESCRIPTION("Kernel Module to List Child Processes");
int pid = 0;
module_param(pid, int, S_IRUGO);
MODULE_PARM_DESC(pid, "PID of the process");
char* findstate(int state){
    if(state==0){
        return "TASK_RUNNING";
    }
    else if(state==1){
        return "TASK_INTERRUPTIBLE";
    }
    else if(state==2){
        return "TASK_UNINTERRUPTIBLE";
    }
    else if(state==4){
        return "__TASK_STOPPED	";
    }
    else if(state==8){
        return "__TASK_TRACED";
    }
    else{
        return "Not found";
    }
}
static int startingfn(void) {
    struct task_struct *pcb=current;
    for_each_process(pcb){
        if(pid==pcb->parent->pid){
            printk("[LKM2] Child Process PID: %d, State: %s\n", pcb->pid, findstate(pcb->__state));

        }
    }
    return 0;
}

static void endingfn(void) {
    printk("Module LKM2 unloaded.\n");
}

module_init(startingfn);
module_exit(endingfn);