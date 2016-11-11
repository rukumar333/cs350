obj-m := process_list.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean: clean_user
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

rmmod:
	-sudo rmmod process_list.ko
insmod: all
	sudo insmod process_list.ko
reload: clean rmmod all insmod

zombie: zombie.c
	gcc -g zombie.c -o zombie.exe

main: main.c
	gcc -g main.c

run: reload main
	sudo ./a.out

clean_user:
	-rm a.out zombie.exe
