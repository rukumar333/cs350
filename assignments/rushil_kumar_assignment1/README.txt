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

Implementation
--------------

A very basic hashtable is used to store background processes and colissions are
handled using a linkedlist.
