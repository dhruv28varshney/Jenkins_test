include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>           /* For the character driver support */
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/ioctl.h>

#define SUCCESS 0
#define DEVICE_NAME "chardev"   /* Dev name as it appears in /proc/devices   */
#define BUF_LEN 1024            /* Max length of the message from the device */

static int devnum = 0;
static int Major = 258;         /* Major number assigned to our device driver */
static int is_open = 0;         /* Is device open? :Used to prevent multiple access to device */
static char msg[BUF_LEN];       /* The msg the device will give when asked */
int num_bytes = BUF_LEN;

/* Macros used to implement the ioctl */
#define IOCTL_SET_MIXED _IO(258,0)
#define IOCTL_SET_UPPER _IO(258,1)
static int mode = 0;

ssize_t my_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
        int bytes_read = 0;
        if (offset == NULL) return -EINVAL;
        if (*offset >= num_bytes) return 0;

        while((bytes_read < length) && (*offset < num_bytes)){

          if (mode == 0){    /* for mode=0, print the msg as it is*/
            put_user(msg[*offset], &buffer[bytes_read]);
          }
          else{              /* for mode!=0, convert the lower case letters to uppercase */
             if((msg[*offset]>='a') && (msg[*offset]<='z')){
                put_user(msg[*offset] - 'a' + 'A', &buffer[bytes_read]);
             }
             else{           /* print other characters as it is*/
                put_user(msg[*offset], &buffer[bytes_read]);
             }
          }
          *offset = *offset + 1;
          bytes_read++;
        }

        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
        return bytes_read;
}

ssize_t my_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
        int bytes_write = 0;
        if (offset == NULL) return -EINVAL;
        if (*offset >= num_bytes) return -EINVAL;

        while((bytes_write < length) && (*offset < num_bytes)){
          get_user(msg[*offset], &buffer[bytes_write]);
          *offset = *offset + 1;
          bytes_write++;
        }

        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
        printk(KERN_INFO "(%s)\n",msg);
        return bytes_write;
}

int my_open(struct inode *pinode, struct file *pfile)
{
        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
        if(is_open == 1){
          printk(KERN_INFO "Error: chrdev device already open\n");
          return -EBUSY;
        }

        is_open = 1;
        try_module_get(THIS_MODULE); /* increase the use count of a module*/
        return 0;
}

int my_close(struct inode *pinode, struct file *pfile)
{
        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
        if(is_open == 0){
          printk(KERN_INFO "Error: chrdev device was not opened\n");
          return -EBUSY;
        }
        is_open = 0;
        module_put(THIS_MODULE); /*prevents module from being unloaded if module is being used*/
        return 0;
}

long my_ioctl(struct file *pfile, unsigned int ioctl_num, unsigned long ioctl_param)
{
        switch(ioctl_num){
        case IOCTL_SET_MIXED:
            mode = 0;
            break;
        case IOCTL_SET_UPPER:
            mode= 1;
            break;
        }

        printk(KERN_INFO "IOCTL set mode %d \n",mode);
        return 0;
}

/* To hold the file operations performed on the device */
struct file_operations my_fops = {
        .owner = THIS_MODULE,
        .open = my_open,
        .read = my_read,
        .write = my_write,
        .release = my_close,
        .unlocked_ioctl = my_ioctl,
};

/* This function is called when the module is loaded */
static int my_module_init(void)
{
        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);

        strncpy(msg,"Hello world.",1023);
        //num_bytes= strlen(msg);

        /* Register the character device driver with the kernel */
        devnum = register_chrdev(Major,DEVICE_NAME,&my_fops);
        printk(KERN_INFO "I was assigned major number %d. \n", Major);
        return 0;
}

/* This function is called when the module is unloaded */
static void my_module_exit(void)
{
        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
        /*Unregister the character device driver */
        unregister_chrdev(Major,DEVICE_NAME);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
