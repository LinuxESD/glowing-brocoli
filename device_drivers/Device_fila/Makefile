obj-m += static_al_driver.o
obj-m += static_al_driver1.o 
obj-m += dynamic_al_driver.o
obj-m += auto_al_driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
