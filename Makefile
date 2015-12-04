all: init update mkkmod ldkmod configure mktest dotest

configure: cfgif cfrt

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
	
cfgif:
	./config/interfaces_arch.sh
	sleep 2

cfrt:
	cd userspace && \
	$(MAKE) fresh && \
	sudo ./464route local add 192.168.254.1 fd16:6db2:c925:0000:2544:67fa:9e35:2876 && \
	sudo ./464route remote add 192.168.254.2 fd16:6db2:c925:0000:2544:67fa:9e35:2877
	
mktest:
	cd ./util/core_udp_test/ && \
	$(MAKE) fresh
	
dotest:
	cd ./util/core_udp_test/ && \
	./client4 192.168.254.1 2000 "hi"