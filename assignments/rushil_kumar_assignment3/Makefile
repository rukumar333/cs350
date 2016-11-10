obj-m := hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

rmmod:
	sudo rmmod hello.ko
insmod:
	sudo insmod hello.ko
reload: clean rmmod all insmod

run: reload main
	sudo ./a.out
main: main.c
	gcc -g main.c
