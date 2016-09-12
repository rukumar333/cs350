#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "JobsList.h"

int BUFFERSIZE = 100000;

JobsList jobsList;

void runShell();
void processInput(char * input);
void runCommand(char **input, unsigned char numArgs, char background);
void handler(int sig);

int main(int argc, char **argv){
    jobsList = initList();
    runShell();
    return 0;
}

void handler(int sig){
    pid_t pid;
    pid = wait(NULL);
    
    printf("Pid %d exited\n", pid);
}

void runShell(){
    char buffer[BUFFERSIZE];
    while(1){
	printf("cs350sh> ");
	if(fgets(buffer, BUFFERSIZE, stdin) != NULL){
	    processInput(buffer);
	}else{
	    printf("Error with input\n");
	}
    }
}

void processInput(char * input){
    unsigned char numArgs = 1;
    int i = 0;
    while(*(input + i) != '\n'){
	if(*(input + i) == ' '){
	    ++ numArgs;
	}
	++ i;
    }
    char * arguments[numArgs + 1];
    arguments[numArgs + 1] = '\0';
    i = 0;
    char argumentLength = 0;
    unsigned char countArgs = 0;
    while(*(input + i) != '\n'){
	if(*(input + i) != ' ' && *(input + i + 1) != '\n'){
	    ++ argumentLength;
	}else{
	    if(*(input + i + 1) == '\n'){
		++ argumentLength;
	    }
	    arguments[countArgs] = (char *)malloc(argumentLength * sizeof(char));
	    argumentLength = 0;
	    ++ countArgs;
	}
	++ i;
    }
    i = 0;
    countArgs = 0;
    argumentLength = 0;
    while(*(input + i) != '\n'){
	if(*(input + i) != ' ' && *(input + i + 1) != '\n'){
    	    *(arguments[countArgs] + argumentLength) = *(input + i);
    	    ++ argumentLength;
    	}else{
	    if(*(input + i + 1) == '\n'){
		*(arguments[countArgs] + argumentLength) = *(input + i);
		++ argumentLength;
	    }
	    /* *(arguments[countArgs] + argumentLength) = '\0'; */
	    argumentLength = 0;
	    ++ countArgs;
	}
    	++ i;
    }
    i = 0;
    if(*(arguments[numArgs - 1]) == '&'){
	printf("Background process\n");
	runCommand(arguments, numArgs, 1);
    }else{
	printf("Foreground process\n");
	runCommand(arguments, numArgs, 0);
    }
}

void runCommand(char **input, unsigned char numArgs, char background){
    
    signal(SIGCHLD, handler);
    pid_t pid = fork();
    if(pid == -1){
	fprintf(stderr, "fork failed\n");
	exit(1); 
    }
    if(pid == 0){
	if(execvp(*(input), input) == -1){
	    fprintf(stderr, "Command not found\n");
	}
    }else{
	printf("%d\n", pid);
	int status;
	/* pid_t result = waitpid(pid, &status, WNOHANG); */
	/* if (result == 0) { */
	/*     printf("Alive\n"); */
	/* } else if (result == -1) { */
	/*     printf("Error\n"); */
	/*     // Error  */
	/* } else { */
	/*     printf("Exited\n"); */
	/*     // Child exited */
	/* } */
    }
}
