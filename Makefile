all: init update kmod install configure test

init: 
	sudo echo starting

update:
	git pull

kmod:
	cd kmod/
	make fresh
	cd ..

install:
	cd kmod/
	sync
	sudo make install
	cd ..

configure:
	./config/interfaces_arch.sh
	sleep 2
	
test:
	cd ./util/core_udp_test/
	make fresh
	./client4 192.168.254.1 2000 "hi"
	cd ../..