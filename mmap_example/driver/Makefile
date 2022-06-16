obj-m	:= char_driv_mmap.o
KDIR	:= /lib/modules/$(shell uname -r)/build

modules:
	$(MAKE) -C $(KDIR) SUBDIRS=$(shell pwd) modules

clean:
	rm -f modules.order Module.symvers *.o *.ko *.mod.c
