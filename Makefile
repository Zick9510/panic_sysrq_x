obj-m := panic_sysrq.o

KVER ?= $(shell uname -r)
KDIR ?= /usr/lib/modules/$(KVER)/build

all:
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean

.PHONY: all clean
