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
#include <linux/list.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kartik");
MODULE_DESCRIPTION("Second Module");

#define mem_size 1024

typedef struct
{
    pid_t pid;
    pid_t ppid;
} info;

dev_t dev = 0;
struct cdev my_cdev;
struct class *dev_class;
char *kernel_buffer;
info processInfo;
int pid;
#define UPDATE _IOWR('x', 'a', info)
#define EMERGENCY _IOWR('x', 'b', int *)

static int my_open(struct inode *inode, struct file *file)
{
    kernel_buffer = kmalloc(mem_size, GFP_KERNEL);
    if (!kernel_buffer)
    {
        printk("Cannot allocate memory to kernel\n");
        return -ENOMEM;
    }
    printk("Device file opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    kfree(kernel_buffer);
    printk("Device file closed\n");
    return 0;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd)
    {
    case UPDATE:
        if (copy_from_user(&processInfo, (info *)arg, sizeof(info)) != 0)
        {
            printk("Error in reading from user\n");
            return -EFAULT;
        }

        struct task_struct *soldier, *control_station, *prev_parent;

        soldier = pid_task(find_get_pid(processInfo.pid), PIDTYPE_PID);
        control_station = pid_task(find_get_pid(processInfo.ppid), PIDTYPE_PID);

        prev_parent = soldier->parent;
        if (!soldier || !control_station)
        {
            printk(KERN_ERR "Invalid PID or PPID\n");
            return -ESRCH;
        }

        struct list_head *list, *tmp;

        task_lock(prev_parent);
        list_for_each_safe(list, tmp, &prev_parent->children)
        {
            struct task_struct *child = list_entry(list, struct task_struct, sibling);
            if (child->pid == soldier->pid)
            {
                list_del(list);
                task_lock(soldier);
                task_lock(control_station);

                soldier->parent = control_station;
                soldier->real_parent = control_station;
                list_add_tail(&soldier->sibling, &control_station->children);

                task_unlock(control_station);
                task_unlock(soldier);
                break;
            }
        }
        task_unlock(prev_parent);

        break;

    case EMERGENCY:

        if (copy_from_user(&pid, (int *)arg, sizeof(int)) != 0)
        {
            printk("Error in reading from user\n");
            return -EFAULT;
        }
        struct task_struct *control_station_task = pid_task(find_get_pid(pid), PIDTYPE_PID);
        struct list_head *list1;
        printk("Inside emergency\n");
        list_for_each(list1, &control_station_task->children)
        {
            struct task_struct *child_task = list_entry(list1, struct task_struct, sibling);
            send_sig(SIGKILL, child_task, 1);
            wake_up_process(child_task);
            ssleep(1);
        }
        send_sig(SIGTERM, control_station_task, 1);
        wake_up_process(control_station_task);
        break;
    }
    return 0;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    // .open = my_open,
    // .release = my_release,
    .unlocked_ioctl = my_ioctl};

static int __init chr_driver_init(void)
{
    if (alloc_chrdev_region(&dev, 0, 1, "dr_bloom") < 0)
    {
        printk("Cannot allocate the major number\n");
        return -1;
    }

    // printk("Major= %d and Minor= %d\n", MAJOR(dev), MINOR(dev));

    // Create device class
    dev_class = class_create(THIS_MODULE, "dr_bloom_class");
    if (IS_ERR(dev_class))
    {
        printk("Cannot create the class\n");
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(dev_class);
    }

    cdev_init(&my_cdev, &fops);
    if (cdev_add(&my_cdev, dev, 1) < 0)
    {
        printk("Error in adding the cdev to kernel module\n");
        class_destroy(dev_class);
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    // Create device file
    if (device_create(dev_class, NULL, dev, NULL, "dr_bloom") == NULL)
    {
        printk("Cannot create the device file\n");
        class_destroy(dev_class);
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    // printk("Device created successfully\n");
    return 0;
}

static void __exit chr_driver_exit(void)
{
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    // printk("Device unregistered successfully\n");
}

module_init(chr_driver_init);
module_exit(chr_driver_exit);
