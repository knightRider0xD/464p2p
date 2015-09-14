all: init update mkkmod ldkmod configure mktest dotest

configure: cfgif

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
	sudo $(MAKE) install

rmkmod:
	cd kmod && \
	sync && \
	sudo $(MAKE) uninstall

clkmod:
	cd kmod && \
	sudo $(MAKE) clean
	

cfgif:
	./config/interfaces_arch.sh
	sleep 2
	
mktest:
	cd ./util/core_udp_test/ && \
	$(MAKE) fresh
	
dotest:
	cd ./util/core_udp_test/ && \
	./client4 192.168.254.1 2000 "hi"