/* 
 * Memory mapping: provides user programs with direct access to device memory
 *
 * syscall: mmap(caddr_t addr, size_t len, int ptro, int flags, int fd, off_t off)
 * file operation: int (*mmap)(struct file *f, struct vm_area_struct *vma)
 *
 * Driver needs to: build page tables for address range, and replace vma->vm_ops
 * Building page tables:
 *      - all at once: remap_page_range
 *      - one page at a time: nopage method. Finds correct page for address, and
 *    increments its reference count. 
 *
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/version.h>

#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/ioctl.h>

#define DEVICE_NAME "chardev_mmap"   /* Dev name as it appears in /proc/devices   */
#define BUF_LEN 1024                 /* Max length of the message from the device */

static int devnum = 0;
static int Major = 268;              /* Major number assigned to our device driver */
static int is_open = 0;              /* Is device open? :Used to prevent multiple access to device */
static char mesg[BUF_LEN];           /* The msg the device will give when asked */
int num_bytes = BUF_LEN;

const char *msg = "My mmap options implement, this is file: ";

struct mmap_info{
        void *data;  /* the data */
        int ref;     /* how many times it is mmapped */
};

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

/*Added to implement mmap*/
static void my_vma_open(struct vm_area_struct *vma)
{
        struct mmap_info *info = (struct mmap_info *)vma->vm_private_data;

        info->ref++;
        pr_info("== %s ==\n", __func__);
}

static void my_vma_close(struct vm_area_struct *vma)
{
        struct mmap_info *info = (struct mmap_info *)vma->vm_private_data;

        info->ref--;
        pr_info("== %s ==\n", __func__);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
static int my_vma_nopage(struct vm_fault *vmf)
#else
static int my_vma_nopage(struct vm_area_struct *vma, struct vm_fault *vmf)
#endif
{
        struct mmap_info *info;
        struct page *page = NULL;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
        struct vm_area_struct *vma = vmf->vma;
#endif

        pr_info("== %s ==\n", __func__);

        if(!vma || (unsigned long)vmf->address > vma->vm_end){
                pr_err("== %s: invalid address ==\n", __func__);
                return VM_FAULT_SIGBUS;
        }

        info = (struct mmap_info *)vma->vm_private_data;
        if(!info->data){
                pr_err("== %s: No data pool pointer ==\n", __func__);
                return VM_FAULT_SIGBUS;
        }

        /* Get the page */
        page = virt_to_page(info->data);
        get_page(page);
        vmf->page = page;

        return 0;
}

struct vm_operations_struct my_vm_ops = {
        .open   = my_vma_open,
        .close  = my_vma_close,
        .fault  = my_vma_nopage,
};

static int my_open(struct inode *ip, struct file *filp)
{
        struct mmap_info *info;

        pr_info("== %s ==\n", __func__);

        if(is_open == 1){
          printk(KERN_INFO "Error: chrdev device already open\n");
          return -EBUSY;
        }
        is_open = 1;
        try_module_get(THIS_MODULE); /* increase the use count of a module*/

        info = kzalloc(sizeof(struct mmap_info), GFP_KERNEL);
        info->data = (void *)get_zeroed_page(GFP_KERNEL);
        memcpy((char *)info->data, msg, strlen(msg));
        memcpy((char *)info->data + strlen(msg),
                                filp->f_path.dentry->d_name.name,
                                strlen(filp->f_path.dentry->d_name.name));
        filp->private_data = info;

        return 0;
}

static int my_release(struct inode * ip, struct file *filp)
{
        struct mmap_info *info = filp->private_data;

        pr_info("== %s ==\n", __func__);

        if(is_open == 0){
          printk(KERN_INFO "Error: chrdev device was not opened\n");
          return -EBUSY;
        }
        is_open = 0;
        module_put(THIS_MODULE); /*prevents module from being unloaded if module is being used*/

        free_page((unsigned long)info->data);
        kfree(info);
        filp->private_data = NULL;

        return 0;
}

static int my_mmap(struct file *filp, struct vm_area_struct *vma)
{
        pr_info("== %s ==\n", __func__);

        vma->vm_ops = &my_vm_ops;
        vma->vm_flags |= (VM_DONTEXPAND | VM_DONTDUMP);
        /* assigned the pointer of structure */
        vma->vm_private_data = filp->private_data;
        my_vma_open(vma);

        return  0;
}

static struct file_operations my_fops = {
        .owner          = THIS_MODULE,
        .open           = my_open,
        .read           = my_read,
        .write          = my_write,
        .release        = my_release,
        .mmap           = my_mmap,
};

static int __init my_mmap_example_init(void)
{

        pr_info("== %s ==\n", __func__);
        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
        strncpy(mesg,"Hello world.",1023);

        /* Register the character device driver with the kernel */
        devnum = register_chrdev(Major,DEVICE_NAME,&my_fops);
        printk(KERN_INFO "I was assigned major number %d. \n", Major);
        return 0;
}

static void __exit my_mmap_example_exit(void)
{
        pr_info("== %s ==\n", __func__);
        //misc_deregister(&my_miscdev);
        printk(KERN_ALERT "Inside the %s function \n", __FUNCTION__);
        /*Unregister the character device driver */
        unregister_chrdev(Major,DEVICE_NAME);
}

module_init(my_mmap_example_init);
module_exit(my_mmap_example_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dhruv Varshney");



