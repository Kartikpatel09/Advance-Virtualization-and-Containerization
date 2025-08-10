KDIR = /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)


obj-m += lkm1.o 
# obj-m += lkm2.o
# obj-m +=lkm3.o 
# obj-m +=lkm4.o 
#  obj-m +=lkm5.o 


all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
