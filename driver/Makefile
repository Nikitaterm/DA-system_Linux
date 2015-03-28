ifeq ($(KERNELRELEASE),)
	PWD =$(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o *.ko .tmp_versions *.mod.c modules.order  Module.symvers	.DA_protocol*
else
	obj-m :=DA_protocol.o
endif
