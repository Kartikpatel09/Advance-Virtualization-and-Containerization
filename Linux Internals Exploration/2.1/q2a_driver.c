#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <asm/io.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("kartik");
MODULE_DESCRIPTION("Second Module");
typedef struct {
    int pid;
    int* vaddr;
    unsigned long paddr;
} info;
typedef struct {
    int pid;
    unsigned long paddr;
    int value;
} info1;

#define mem_size 1024
#define CONVERT _IOWR('m', 'a', info)
#define UPDATE _IOWR('m', 'b', info1)


dev_t dev = 0;
struct cdev my_cdev;
struct class *dev_class;
info addrInfo;
info1 addrInfo1;
unsigned long paddr = 0;
char *kernel_buffer;

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

    if (pte == NULL) {
        return 0;
    }

    struct page *pg = pte_page(*pte);
    return (page_to_phys(pg) + (vaddr & (PAGE_SIZE - 1)));
}

static int my_open(struct inode *inode, struct file *file)
{
    kernel_buffer = kmalloc(mem_size, GFP_KERNEL);
    if (!kernel_buffer) {
        printk("Cannot allocate memory to kernel\n");
        return -ENOMEM;
    }
    printk("Device file opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    kfree(kernel_buffer);
    printk("Device File closed\n");
    return 0;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case CONVERT:
            if (copy_from_user(&addrInfo, (info *)arg, sizeof(addrInfo)) != 0) {
                printk(KERN_ERR "Error in reading from user\n");
                return -EFAULT;
            }
            struct task_struct *task;
            printk("Pid: %d Vaddr: %p",addrInfo.pid,addrInfo.vaddr);
            task = pid_task(find_get_pid(addrInfo.pid), PIDTYPE_PID);
            if (!task) {
                printk(KERN_ERR "Invalid PID: %d\n", addrInfo.pid);
                return -EINVAL;
            }
            addrInfo.paddr = va_to_pa(task, (unsigned long)addrInfo.vaddr);
            if (copy_to_user((info *)arg, &addrInfo, sizeof(addrInfo)) != 0){
                printk(KERN_ERR "Error in writing to user\n");
                return -EFAULT;
            }
            break;
        case UPDATE:
            if (copy_from_user(&addrInfo1, (info *)arg, sizeof(addrInfo1)) != 0) {
                printk(KERN_ERR "Error in reading from user\n");
                return -EFAULT;
            }
            unsigned char* vaddr;
            vaddr=(void*)phys_to_virt((phys_addr_t) addrInfo1.paddr);
            if (vaddr) {
                *(vaddr) = addrInfo1.value;
                } else {
                    printk(KERN_ERR "Invalid virtual address\n");
                    return -EINVAL;
                    }
            //  if (copy_to_user((info *)arg, &addrInfo, sizeof(addrInfo)) != 0) {
            //     printk(KERN_ERR "Error in writing to user\n");
            //     return -EFAULT;
            // }
            break;
        default:
            return -EINVAL; 
    }
    return 0;
}


struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .unlocked_ioctl = my_ioctl,
};

static int __init chr_driver_init(void)
{
    if (alloc_chrdev_region(&dev, 0, 1, "va2pa") < 0) {
        printk("Cannot allocate the major number\n");
        return -1;
    }

    printk("Major= %d and Minor= %d\n", MAJOR(dev), MINOR(dev));

    cdev_init(&my_cdev, &fops);
    if (cdev_add(&my_cdev, dev, 1) < 0) {
        printk("Error in adding in kernel module\n");
        goto r_class;
    }

    dev_class = class_create(THIS_MODULE, "my_class");
    if (dev_class == NULL) {
        printk("Error in creating struct class\n");
        goto r_class;
    }

    if (device_create(dev_class, NULL, dev, NULL, "va2pa") == NULL) {
        printk("Cannot create the device\n");
        goto r_devices;
    }

    printk("Device created successfully\n");
    return 0;

r_devices:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev, 1);
    return -1;
}

static void __exit chr_driver_exit(void)
{
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    printk("Device is unregistered successfully\n");
}

module_init(chr_driver_init);
module_exit(chr_driver_exit);
