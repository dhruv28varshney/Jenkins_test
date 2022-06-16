#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h> /* For the character driver support */

ssize_t my_read(struct file *file, char __user *buffer, size_t length, loff_t *offset)
{
        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
        return 0;
}

ssize_t my_write(struct file *file, const char __user *buffer, size_t length, loff_t *offset)
{
        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
        return length;
}

int my_open(struct inode *inode, struct file *file)
{
        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
        return 0;
}

int my_close(struct inode *inode, struct file *file)
{
        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
        return 0;
}

/* To hold the file operations performed on the device */
struct file_operations my_file_operations = {
        .owner = THIS_MODULE,
        .open = my_open,
        .read = my_read,
        .write = my_write,
        .release = my_close,
};

int my_simple_module_init(void)
{
        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);

        /* Register the character device driver with the kernel */
        register_chrdev(255 /* Major Number */,
                         "Sample Char Dev" /* Name of the Driver */,
                         &my_file_operations /* File Operations */);

        return 0;
}

void my_simple_module_exit(void)
{
        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);

        /*Unregister the character device driver */
        unregister_chrdev(255,"Sample Char Dev");
}

module_init(my_simple_module_init);
module_exit(my_simple_module_exit);

