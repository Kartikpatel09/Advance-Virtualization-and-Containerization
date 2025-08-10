#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>  // for class_create

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kartik");
MODULE_DESCRIPTION("Second Module");

#define mem_size 1024
#define RD_DATA _IOR('a', 'b', int)  // Define a command to read data

dev_t dev = 0;
struct cdev my_cdev;
struct class *dev_class;
int val = 70;  // Value to send to user-space
char *kernel_buffer;

static int my_open(struct inode *inode, struct file *file) {
    // Allocate memory for the kernel buffer
    kernel_buffer = kmalloc(mem_size, GFP_KERNEL);
    if (!kernel_buffer) {
        printk("Cannot allocate memory to kernel\n");
        return -ENOMEM;
    }
    printk("Device file opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file) {
    // Free the allocated memory when the device is closed
    kfree(kernel_buffer);
    printk("Device File closed\n");
    return 0;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case RD_DATA:
            // Copy the value from kernel to user-space
            if (copy_to_user((int *)arg, &val, sizeof(val))) {
                return -EFAULT;  // Error while copying to user-space
            }
            break;
        default:
            return -EINVAL;  // Invalid command
    }

    return 0;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .unlocked_ioctl = my_ioctl,
};

static int __init chr_driver_init(void) {
    if (alloc_chrdev_region(&dev, 0, 1, "my_dev") < 0) {
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

    if (device_create(dev_class, NULL, dev, NULL, "my_device") == NULL) {
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

static void __exit chr_driver_exit(void) {
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    printk("Device is unregistered successfully\n");
}

module_init(chr_driver_init);
module_exit(chr_driver_exit);
