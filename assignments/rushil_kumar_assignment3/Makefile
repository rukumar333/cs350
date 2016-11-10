obj-m := hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

rmmod:
	-sudo rmmod hello.ko
insmod:
	sudo insmod hello.ko
reload: clean rmmod all insmod

main: main.c
	gcc -g main.c
zombie: zombie.c
	gcc -g zombie.c -o zombie.exe

run: reload main
	sudo ./a.out
