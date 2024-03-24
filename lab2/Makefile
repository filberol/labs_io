MODULE_NAME=blk_module
PATH_SRC=$(shell pwd)
KERNEL=$(shell uname -r)
DEV_NAME=var2

obj-m += $(MODULE_NAME).o

all: build install mount

build:
	/usr/bin/make -C /lib/modules/$(KERNEL)/build M=$(PATH_SRC) modules

clean:
	/usr/bin/make -C /lib/modules/$(KERNEL)/build M=$(PATH_SRC) clean

install:
	sudo insmod $(MODULE_NAME).ko
	@lsmod | grep "$(MODULE_NAME)"
	@-sudo chmod 777 /dev/$(DEV_NAME)

	@echo "Installed"

uninstall:
	@for device in 1 5 6 ; do \
		sudo umount /mnt/vram$${device}; \
		sudo rm -rf /mnt/vram$${device}; \
	done

	sudo rmmod $(MODULE_NAME).ko
	@sudo rm -f /dev/$(MODULE_NAME)
	@sudo rm -f /proc/$(MODULE_NAME)


	@echo "Uninstalled"

mount:
	@for device in 1 5 6 ; do \
	    sudo mkfs.vfat /dev/vramdisk$${device}; \
		sudo mkdir -p /mnt/vram$${device}; \
		sudo mount -t vfat /dev/vramdisk$${device} /mnt/vram$${device}; \
	done

purge: uninstall clean

recompile: purge all