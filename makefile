builddir = bin
testdir = test
srcdir = kmod

all: kmod

kmod: modpre $(builddir)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

modpre: $(builddir)
	VPATH = $(srcdir)
	obj-m += kmod.o
	kmod-objs := module.o 64_inbound.o 46_outbound.o 464_tables.o

test-all: testpre $(builddir)
	

testpre:
	CC = g++
	VPATH = $(srcdir) : $(testdir)
	
$(builddir):
	@echo "Preparing Build Directory ..."
	@mkdir -p $@

clean:
	@echo "Removing objects ..."
	@rm -rf $(builddir)/*.o
	@echo "Removing kernel objects ..."
	@rm -rf $(builddir)/*.ko
	@echo "Removing test objects ..."
	@rm -rf $(builddir)/*.test
	@echo "Done."