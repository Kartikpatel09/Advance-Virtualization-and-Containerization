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
MODULE_DESCRIPTION("Kernel Module to Convert Virtual Address to Physical Address");

static int pid;
static unsigned long vaddr;

module_param(pid, int, 0); 
module_param(vaddr, ulong, 0); 


unsigned long va_to_pa(struct task_struct *task, unsigned long vaddr)
{

    
    struct mm_struct *mm = task->mm;
    if (!mm)
        return 0;  
    
    pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte = NULL;

	pgd = pgd_offset(mm, vaddr);
	if (pgd_present(*pgd)) {
		p4d = p4d_offset(pgd, vaddr);
		if (p4d_present(*p4d)) {
			pud = pud_offset(p4d, vaddr);
			if (pud_present(*pud)) {
				pmd = pmd_offset(pud, vaddr);
				if (pmd_present(*pmd))
					pte = pte_offset_map(pmd, vaddr);
			}
		}
	}
    if(pte==NULL){return 0;}
    struct page *pg = pte_page(*pte);
    return (page_to_phys(pg)+(vaddr & (PAGE_SIZE-1)));
    }


struct task_struct *find_task_by_pid(int pid)
{
    struct task_struct *task;
    task =pid_task(find_get_pid(pid),PIDTYPE_PID);
    return task;
}


static int startingfn(void)
{
    struct task_struct *task;
    unsigned long physical_address;
    task = find_task_by_pid(pid);
    if (!task) {
        printk("LKM3: No such process with PID %d\n", pid);
        return -ESRCH; 
    }

    
    physical_address = va_to_pa(task, vaddr);

    
    if (physical_address == 0) {
        printk("LKM3: VA not mapped:\n");
    } else {
        printk("[LKM3] Virtual address: 0x%lx / %lu\n",vaddr,vaddr);
        printk("[LKM3] Physical address:0x%lx / %lu\n",physical_address,physical_address)
    }

    return 0;
}


static void endingfn(void)
{
    printk("[LKM3] Module LKM3 Unloaded\n");
}

module_init(startingfn);
module_exit(endingfn);