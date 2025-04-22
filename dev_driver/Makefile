obj-m += camdriver.o

camdriver-objs := cam_main.o cam_device.o cam_core.o cam_v4l2.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
