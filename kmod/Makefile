obj-m += 464_p2p.o
464_p2p-objs := module.o 64_inbound.o 46_outbound.o 464_tables.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

all-debug:
	make CFLAGS="-D VERBOSE_464P2P" -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
install:
	insmod ./464_p2p.ko v6Addr="fd16:6db2:c925:0000:2544:67fa:9e35:2876" v4Addr="192.168.254.1"

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean