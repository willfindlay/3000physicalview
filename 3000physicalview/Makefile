obj-m += 3000physicalview.o

.PHONY: all, clean

all:
	make ARCH=x86_64 -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insert:
	sudo insmod 3000physicalview.ko

remove:
	sudo rmmod 3000physicalview
