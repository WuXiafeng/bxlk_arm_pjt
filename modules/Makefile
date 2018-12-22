#Add by Wuxf 20181209
#compile target using ../build.sh
obj-m := gpio_isr_module.o
KDIR := /home/work/linux-xlnx-xilinx-v2018.2
gpio_isr_modulemodule-objs := module

all:
	make -C $(KDIR) M=$(PWD)/modules modules
clean:
	make -C $(KDIR) SUBDIRS=$(PWD) clean
	