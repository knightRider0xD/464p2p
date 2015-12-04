all: h1

h1: init update mkkmod ldkmod mkrt config1 mktest dotest1

h2: init update mkkmod ldkmod mkrt config2 mktest dotest2

config1: cfgif1 cfgrt1

config2: cfgif2 cfgrt2

clean: rmkmod clkmod

init: 
	sudo echo starting

update:
	git pull

	
mkkmod:
	cd kmod && \
	$(MAKE) fresh

ldkmod:
	cd kmod && \
	sync && \
	sudo $(MAKE) install && \
	sync

rmkmod:
	cd kmod && \
	sync && \
	sudo $(MAKE) uninstall

clkmod:
	cd kmod && \
	sudo $(MAKE) clean
	
runstatus:
	dmesg

runreview:
	journalctl -b -1
	
cfgif1:
	sudo ip addr add 192.168.254.1/24 broadcast 192.168.254.255 dev enp0s3
	sudo ip -6 addr add fd16:6db2:c925:0000:2544:67fa:9e35:2876/64 dev enp0s3
	sleep 2

cfgif2:
	sudo ip addr add 192.168.254.2/24 broadcast 192.168.254.255 dev enp0s3
	sudo ip -6 addr add fd16:6db2:c925:0000:2544:67fa:9e35:2877/64 dev enp0s3
	sleep 2

mkrt:
	cd userspace && \
	$(MAKE) fresh
	
cfgrt1:
	cd userspace && \
	sudo ./464route local add 192.168.254.1 fd16:6db2:c925:0000:2544:67fa:9e35:2876 && \
	sudo ./464route remote add 192.168.254.2 fd16:6db2:c925:0000:2544:67fa:9e35:2877
	
cfgrt2:
	cd userspace && \
	sudo ./464route local add 192.168.254.2 fd16:6db2:c925:0000:2544:67fa:9e35:2877 && \
	sudo ./464route remote add 192.168.254.1 fd16:6db2:c925:0000:2544:67fa:9e35:2876
	
mktest:
	cd ./util/core_udp_test/ && \
	$(MAKE) fresh
	
dotest1:
	cd ./util/core_udp_test/ && \
	./server4 2000
	
dotest2:
	cd ./util/core_udp_test/ && \
	./client4 192.168.254.1 2000 "hi"