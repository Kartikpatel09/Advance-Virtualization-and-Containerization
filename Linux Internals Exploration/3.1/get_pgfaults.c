#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/vmstat.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/debugfs.h>
#include <linux/sched.h>
#include <linux/math64.h>
#include <linux/writeback.h>
#include <linux/compaction.h>
#include <linux/mm_inline.h>
#include <linux/page_ext.h>
#include <linux/page_owner.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kartik");
MODULE_DESCRIPTION("Kernel module for page faults.");

static int get_pgfaults_show(struct seq_file *m, void *v)
{
    int cpu;

	unsigned long pageFault=0;

	for_each_online_cpu(cpu) {
		struct vm_event_state *this = &per_cpu(vm_event_states, cpu);

		pageFault+= this->event[PGFAULT];
	}
    seq_printf(m,"Number of page fault is %ld\n",pageFault);
    return 0;
}

static int get_pgfaults_open(struct inode *inode, struct file *file)
{
    return single_open(file, get_pgfaults_show, NULL);
}
static const struct proc_ops get_pgfaults_fops = {
    .proc_open = get_pgfaults_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int startingfn(void)
{
    proc_create("get_pgfaults", 0, NULL, &get_pgfaults_fops);
    printk("get_pgfaults module loaded.\n");
    return 0;
}

static void endingfn(void)
{
    remove_proc_entry("get_pgfaults", NULL);
    printk("get_pgfaults module removed.\n");
}

module_init(startingfn);
module_exit(endingfn);


