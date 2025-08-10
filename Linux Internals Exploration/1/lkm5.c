#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/mm_types.h>
#include <linux/mmap_lock.h>
#include <linux/huge_mm.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A Kernel Module to Count HugePages of a Process");

static int pid = 0;
module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "PID of the process to check for hugepages");

static int count_huge_pages(struct task_struct *task)
{
    struct mm_struct *mm;
    struct vm_area_struct *vma;
    unsigned long huge_page_count = 0;
    unsigned long huge_page_size=0;

    if (!task)
        return 0;

    mm = task->mm;
    if (!mm)
        return 0;

    mmap_read_lock(mm); 
    VMA_ITERATOR(vmi, mm, 0);
    for_each_vma(vmi, vma)
    {
        for (unsigned long addr = vma->vm_start; addr < vma->vm_end;)
        {
            pmd_t *pmd;
            unsigned long page_size = PAGE_SIZE;
            pud_t *pud;
            pud=pud_offset(p4d_offset(pgd_offset(mm, addr), addr), addr);
            if(pud_present(*pud) && pud_trans_huge(*pud)){
                page_size=PUD_SIZE;
                huge_page_size+=PUD_SIZE;
                huge_page_count++;
            }
            else{
            pmd = pmd_offset(pud_offset(p4d_offset(pgd_offset(mm, addr), addr), addr), addr);
            if (pmd_present(*pmd) && pmd_trans_huge(*pmd))
            {
                page_size = PMD_SIZE; 
                huge_page_size+=PMD_SIZE;
                huge_page_count++;
            }
            }
            addr += page_size; 
        }
    }
    mmap_read_unlock(mm); 

    printk("[LKM5] THP Size: %lu KiB, THP count: %lu\n",huge_page_size,huge_page_count);
    return 0;
}

static int startingfn(void)
{
    struct task_struct *task;
    int huge_page_count;

    task = pid_task(find_get_pid(pid), PIDTYPE_PID);
    if (!task)
    {
        pr_err("Failed to find process with PID=%d\n", pid);
        return -ESRCH;
    }

    int status=count_huge_pages(task);
    

    return status;
}

static void endingfn(void)
{
    pr_info("HugePage Counting Kernel Module Unloaded\n");
}

module_init(startingfn);
module_exit(endingfn);
// if (is_vm_hugetlb_page(vma)) {
// 		if (ops->hugetlb_entry)
// 			err = walk_hugetlb_range(start, end, walk);
// 	} else
// 		err = walk_pgd_range(start, end, walk);

// 	if (ops->post_vma)
// 		ops->post_vma(walk);