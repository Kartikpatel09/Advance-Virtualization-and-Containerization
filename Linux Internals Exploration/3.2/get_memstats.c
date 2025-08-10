#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kartik");
MODULE_DESCRIPTION("Kernel module to create sysfs attributes");

struct kobject *mem_stats;

int pid = -1;
unsigned long virtmem = 0;
unsigned long physmem = 0;
char unit = 'B';  // B for Bytes & K for KB & M for MB
char prevUnit = 'K';

unsigned long convert(unsigned long addr, char u) {
    unsigned long c_addr = 0;
    if(u=='K')
        return addr;
    if(u=='B'){
        c_addr=addr*1024;
        return c_addr;
    }
    if(u=='M'){
        c_addr=addr/1024;
        return c_addr;
    }
    return c_addr;
}

ssize_t pid_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", pid);
} 

ssize_t pid_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    sscanf(buf, "%d", &pid);
    return count;
}

ssize_t virtmem_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    struct task_struct *pcb;
    pcb =pid_task(find_get_pid(pid),PIDTYPE_PID);
    virtmem = convert(pcb->mm->total_vm * 4, unit);
    return sprintf(buf, "%ld\n", virtmem);
}

ssize_t physmem_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    struct task_struct *pcb;
    pcb =pid_task(find_get_pid(pid),PIDTYPE_PID);
    physmem = atomic_long_read(&pcb->mm->rss_stat.count[MM_FILEPAGES]) 
              + atomic_long_read(&pcb->mm->rss_stat.count[MM_ANONPAGES]) 
              + atomic_long_read(&pcb->mm->rss_stat.count[MM_SWAPENTS]) 
              + atomic_long_read(&pcb->mm->rss_stat.count[MM_SHMEMPAGES]);

    physmem = convert(physmem * 4, unit);
    return sprintf(buf, "%ld\n", physmem);
}

ssize_t unit_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%c\n", unit);
}

ssize_t unit_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    prevUnit = unit;
    sscanf(buf, "%c", &unit);
    return count;
}

static struct kobj_attribute pid_attribute = __ATTR(pid, 0660, pid_show, pid_store);
static struct kobj_attribute virtmem_attribute = __ATTR(virtmem, 0660, virtmem_show, NULL);
static struct kobj_attribute physmem_attribute = __ATTR(physmem, 0660, physmem_show, NULL);
static struct kobj_attribute unit_attribute = __ATTR(unit, 0660, unit_show, unit_store);

int __init mem_stats_init(void) {
    int error = 0;
    printk("mem_stats: initialized\n");
    mem_stats = kobject_create_and_add("mem_stats", kernel_kobj);
    if (!mem_stats)
        return -ENOMEM;

    error = sysfs_create_file(mem_stats, &pid_attribute.attr);
    if (error) {
        pr_info("failed to create the pid file in /sys/kernel/mem_stats\n");
        goto error_out;
    }

    error = sysfs_create_file(mem_stats, &virtmem_attribute.attr);
    if (error) {
        pr_info("failed to create the virtmem file in /sys/kernel/mem_stats\n");
        goto error_out;
    }

    error = sysfs_create_file(mem_stats, &physmem_attribute.attr);
    if (error) {
        pr_info("failed to create the physmem file in /sys/kernel/mem_stats\n");
        goto error_out;
    }

    error = sysfs_create_file(mem_stats, &unit_attribute.attr);
    if (error) {
        pr_info("failed to create the unit file in /sys/kernel/mem_stats\n");
        goto error_out;
    }

    return 0;

error_out:
    kobject_put(mem_stats);
    return error;
}

static void __exit mem_stats_exit(void) {
    pr_info("mem_stats: Exit success\n");
    kobject_put(mem_stats);
}

module_init(mem_stats_init);
module_exit(mem_stats_exit);
