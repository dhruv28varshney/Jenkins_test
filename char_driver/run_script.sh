#!/bin/bash
make clean
make host
sudo insmod ./char_drv_fops.ko
#sudo mknod -m 666 /dev/chardev c 258 0
echo "testing 1 2 3" > /dev/chardev
cat /dev/chardev
sudo rmmod char_drv_fops
