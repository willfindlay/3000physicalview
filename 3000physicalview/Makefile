obj-m += 3000physicalview.o

.PHONY: all, clean, insert, remove

all: 3000memview2 3000physicalview.ko

3000physicalview.ko:
	make ARCH=x86_64 -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

3000memview2:
	gcc -o 3000memview2 3000memview2.c

insert:
	sudo insmod 3000physicalview.ko

remove:
	sudo rmmod 3000physicalview

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm 3000memview2
