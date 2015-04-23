obj-m += kmod.o
kmod-objs := module.o 64_inbound.o 46_outbound.o 464_tables.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean