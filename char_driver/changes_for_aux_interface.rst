Changes made by me to code taken from Xilinx-CNS/sfc repo:
=========================================================
* Fixed the #include errors- update kernel version, copy sfc header file
* To assign a parent driver for the auxdev: refered a github code on how to init a char device using device_create and used the created struct device to assign as auxdev.dev->parent
* To execute the above, I passed the struct device* as input to auxbus_register function
* Another thing done was to remove efx struct from all function names (renamed to test in all functions) and also from usage
* In auxbus_unregister function, the auxdev was being called using the efx struct and to remove that I passed the rdev as input to the auxbus_unregister function. Also, I made the rdev as output type for auxbus_register function to execute this.

Observations on testing the code:
=================================
* No issues while generating the .ko file
* No issues on doing insmod
* Not sure about the rmmod execution- not getting any error, module/driver is removed from list of modules but getting some unexpected things in the syslog (can't understand much- might be expected/fine too)

Addressing review comments:
==========================
* The rmmod issue - got fixed by unregistering the auxdev before the test driver
* Modified the Makefile to compile the test_auxbus.c file instead of including it in cdev_drv_fops.c file
