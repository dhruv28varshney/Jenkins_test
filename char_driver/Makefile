obj-m := simple_module.o
obj-m := sample_char_driver.o
obj-m := char_drv_fops.o
#obj-m := char_driv_fops.o
#obj-m += char_drv_ioctl.o
HOST_KERN_DIR = /lib/modules/$(shell uname -r)/build/

clean:
        make -C $(HOST_KERN_DIR) M=$(PWD) clean
help:
        make -C $(HOST_KERN_DIR) M=$(PWD) help
host:
        make -C $(HOST_KERN_DIR) M=$(PWD) modules
