#!/bin/bash
make host
sudo insmod ./char_drv_fops.ko
#sudo mknod -m 666 /dev/chardev c 258 0
echo "testing" > /dev/chardev
cat /dev/chardev
sudo rmmod char_drv_fops
jenkins ALL=(ALL) NOPASSWD: ALL

