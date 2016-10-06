How to Compile
--------------

Use the command "make" to compile the program into "cs350sh".
Use the command "make clean" to remove all object files and cs350.

How to Run
----------

Use "./cs350sh" after compiling to run the shell. You can exit the
shell by giving it the command "quit". 
The shell handles background processes but the "&" has to have a space
before it at the end.
Good -> "emacs &"
Bad -> "emacs&"
List all of the background processes by using the command "listjobs"
If a child process finishes, it will remain a zombie process until it is
brought into the foreground or "listjobs" is used.
Can handle pipes and redirects.
Ex.
ls | grep main.c > output.txt
ls | grep main.c >> output.txt
grep int < main.c | grep freeInput | grep \{ > output.txt

Implementation
--------------

A very basic hashtable is used to store background processes and colissions are
handled using a linkedlist.

Recursion is used to handle forking difference processes for each command. Pipes are used to handle
pipes whereas file descriptors for files are used for redirection from files.
