#!/bin/bash
make host
jenkins ALL=(ALL) NOPASSWD: insmod ./char_drv_fops.ko
#sudo mknod -m 666 /dev/chardev c 258 0
echo "testing" > /dev/chardev
cat /dev/chardev
jenkins ALL=(ALL) NOPASSWD: rmmod char_drv_fops
