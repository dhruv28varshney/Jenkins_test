#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>

#include <linux/errno.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/ioctl.h>

typedef struct {
    int num;
    char buf[256];
} mmmptest_ctx;

#define DEV_NAME "mmptest3" 
static mmmptest_ctx *sh_mem = NULL;
#define PAGESIZE        4096
#define SHARE_MEM_SIZE (PAGE_SIZE * 5)  
#define BUF_LEN 1024                 /* Max length of the message from the device */
static int Major = 262;              /* Major number assigned to our device driver */
static int is_open = 0;              /* Is device open? :Used to prevent multiple access to device */
static char mesg[BUF_LEN];           /* The msg the device will give when asked */
int num_bytes = BUF_LEN;

ssize_t my_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
        int bytes_read = 0;
        if (offset == NULL) return -EINVAL;
        if (*offset >= num_bytes) return 0;

        while((bytes_read < length) && (*offset < num_bytes)){
          put_user(mesg[*offset], &buffer[bytes_read]);
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
        if (*offset >= 1023) return -EINVAL;

        while((bytes_write < length) && (*offset < 1023)){
          get_user(mesg[*offset], &buffer[bytes_write]);
          *offset = *offset + 1;
          bytes_write++;
        }

        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
        printk(KERN_INFO "(%s)\n",mesg);
        return bytes_write;
}

static int my_open(struct inode *ip, struct file *filp)
{
        pr_info("== %s ==\n", __func__);

        if(is_open == 1){
          printk(KERN_INFO "Error: chrdev device already open\n");
          return -EBUSY;
        }
        is_open = 1;
        try_module_get(THIS_MODULE); /* increase the use count of a module*/

        return 0;
}

static int my_release(struct inode * ip, struct file *filp)
{
        pr_info("== %s ==\n", __func__);

        if(is_open == 0){
          printk(KERN_INFO "Error: chrdev device was not opened\n");
          return -EBUSY;
        }
        is_open = 0;
        module_put(THIS_MODULE); /*prevents module from being unloaded if module is being used*/

        return 0;
}

static void my_vma_open(struct vm_area_struct *vma)
{
        pr_info("== %s ==\n", __func__);
}

static void my_vma_close(struct vm_area_struct *vma)
{
        pr_info("== %s ==\n", __func__);
}

struct vm_operations_struct my_vm_ops = {
        .open   = my_vma_open,
        .close  = my_vma_close,
};

static int mmptest_mmap(struct file *filp, struct vm_area_struct *vma)
{
    int ret;
    struct page *page = NULL;
    unsigned long size = (unsigned long)(vma->vm_end - vma->vm_start);
    printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);

    if (size > SHARE_MEM_SIZE)
    {
        ret = -EINVAL;
        goto err;
    }

    page = virt_to_page((unsigned long)sh_mem + (vma->vm_pgoff << PAGE_SHIFT));
    ret = remap_pfn_range(vma, vma->vm_start, page_to_pfn(page), size, vma->vm_page_prot);
    if (ret)
    {
        goto err;
    }

    vma->vm_ops = &my_vm_ops;
    my_vma_open(vma);
    return 0;

err:
    return ret;
}

static struct file_operations mmptest_fops =
{
        .owner          = THIS_MODULE,
        .mmap           = mmptest_mmap,
        .open           = my_open,
        .read           = my_read,
        .write          = my_write,
        .release        = my_release,
};

static int mmptest_init(void)
{
    int ret;
    printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
    strncpy(mesg,"Hello world.",1023);

    ret = register_chrdev(Major,DEV_NAME,&mmptest_fops);
    if (ret) {
        printk("register char device error\n");
        return ret;
    }

    printk("register char dev ok\n");

    sh_mem = kmalloc(SHARE_MEM_SIZE, GFP_KERNEL);  
    if (!sh_mem) {
        printk("kmalloc error\n");
        goto err;
    }

    sh_mem->num = 28;
    sprintf(sh_mem->buf, "Hi Dhruv Varshney, Good to see you");

    printk("kmalloc ok\n");
    return 0;

err:
    unregister_chrdev(Major,DEV_NAME);
    return ret;
}

static void mmptest_exit(void)
{
    printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);

    kfree(sh_mem);

   unregister_chrdev(Major,DEV_NAME);
}

module_init(mmptest_init);
module_exit(mmptest_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Dhruv Varshney");








