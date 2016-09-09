#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv){
    pid_t pid = fork();
    if(pid < 0){
	printf("Fork failed: %d\n", pid);
	return -1;
    }
    if(pid == 0){
	printf("Child process\n");
	printf("Child PID: %d\n", getpid());
	printf("Child PPID: %d\n", getppid());
	int status = execvp("ls", argv);
	if(status < 0){
	    printf("execvp() failed\n");
	}
    }else{
	int status = 0;
	wait(&status);
	printf("Parent: PID %d\n", getpid());
	printf("Status from child: %d\n", status);	
    }
    return 0;
}
