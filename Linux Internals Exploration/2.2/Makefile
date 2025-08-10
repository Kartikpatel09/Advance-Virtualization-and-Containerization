TARGETS = control_station soldier
SOURCES = $(TARGETS:=.c)

all: $(TARGETS) chardev.ko

$(TARGETS): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
	make -C $(KDIR) M=$(PWD) clean

KDIR = /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m += chardev.o 

chardev.ko:
	make -C $(KDIR) M=$(PWD) modules
