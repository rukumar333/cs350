----- Files ------
makefile
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
