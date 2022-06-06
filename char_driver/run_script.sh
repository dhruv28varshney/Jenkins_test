#!/bin/bash
make host
insmod ./char_drv_fops.ko
mknod -m 666 /dev/chardev c 258 0
echo "testing" > /dev/chardev
cat /dev/chardev
rmmod char_drv_fops
