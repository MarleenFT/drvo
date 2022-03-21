KDIR = /lib/modules/$(shell uname -r)/build/
PWD = $(shell pwd)
MAKE = /usr/bin/make

%.ko : %.c
		$(MAKE) obj-m=$(*).o -C $(KDIR) M=$(PWD) modules

clean:
		$(MAKE) -C $(KDIR) M=$(PWD) clean
