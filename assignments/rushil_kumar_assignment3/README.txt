How to Compile
--------------
------ Kernel Level --------
Use the command "make" to compile kernel module into "process_list.ko"
Use the command "make clean" to clean and remove module files.
Use the command "sudo insmod process_list.ko" or "make insmod" to insert the module
Use the command "sudo rmmod process_list.ko" or "make rmmod" to remove the module
Use the command "make reload" to clean, remove, compile, and then insert module

------ User Level ----------
Use the command "make main" to compile main.c into a.out (program to read device)
Use the command "make main" to compile zombie.c into zombie.exe (testing zombie process)

How to Run
----------

After compiling module, inserting, and then compiling main.c,
run "sudo ./a.out" to read character device "process_list".
It will print out all of the processes with their PID, PPID,
CPU, and STATE.

TEST ZOMBIE PROCESS
Compile zombie.c and run zombie.exe which will create a zombie
process for 60 seconds. Use a.out to read from the character
device to see if a process with a zombie state is found.

Implementation
--------------

The character device does not need to be read all at once.
When the character device is being read for the first time,
it loads up the information about current running processes
into a buffer which is reallocated based on size needed.
On every subsequent read, the device uses the loff_t offset
to copy data into the user buffer.
