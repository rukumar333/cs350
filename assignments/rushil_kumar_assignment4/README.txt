----- Explaining compilation of kernel and configuration -----
(Ran in arch linux)

1) Downloaded latest linux kernel
2) Went into linux directory and changed arch/x86/entry/syscalls/syscall_64.tbl to include
entries for syscalls.
Ex.
329	common	my_xtime		sys_my_xtime
330	common	bad_call		sys_bad_call
3) Include .c files into kernel/
4) Add .o files into kernel/Makefile
5) Configure kernel using make menuconfig. Do not include unnecessary drivers such as those
for Mac machines.
6) Compile kernel using "make" which creates bzImage.
7) Install kernel modules using "make modules_install"
8) Copy arch/x86_64/boot/bzImage to /boot/vmlinuz-linux48 (major, minor version)
9) Copied arch linux init RAM disk preset and then run "mkinitcpio -p linux48" to make init RAM disk
10) Updated systemd boot entry to run /boot/vmlinuz-linux48 with init ram disk initramfs-linux48.img
11) Reboot machine with new kernel.

----- Files ------
makefile
kernel-makefile.txt -> Modified kernel/Makefile
rushil_time.c -> time syscall
rushil_bad.c -> bad syscall code
user_level.c -> user level code for time syscall
user_level_bad.c -> user level code for bad syscall code
journal.txt -> journal of bad syscall code errors
*.png -> images of bad syscall code errors
syscall_64.tbl -> changed kernel syscall table

----- Compiling ------
User space: run "make" to compile user_level and user_level_bad

----- Usage --------
Insert rushil_time.c (time syscall) into kernel
Insert rushil_bad.c (bad syscall code) into kernel

./user_level to use time syscall
./user_level_bad to run bad syscall code

In user_level_bad.c define CODE according to which bad syscall code to run
Look in user_level_bad.c for codes.
