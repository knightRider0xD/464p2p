all: init update kmod install configure mktest runtest

init: 
	sudo echo starting

update:
	git pull

kmod:
	cd kmod && $(MAKE) fresh

install:
	cd kmod && sync && sudo $(MAKE) install

configure:
	./config/interfaces_arch.sh
	sleep 2
	
mktest:
	cd ./util/core_udp_test/ && $(MAKE) fresh && ./client4 192.168.254.1 2000 "hi"
	
runtest:
	cd ./util/core_udp_test/ && $(MAKE) fresh && ./client4 192.168.254.1 2000 "hi"