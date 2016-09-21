#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "JobsList.h"

int BUFFERSIZE = 100000;

JobsList jobsList;

void runShell();
char processInput(char * input);
char runCommand(char **input, unsigned char numArgs, char background);
void handler(int sig);
void freeInput(char **input, int numArgs);

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
    char status = 1;
    while(status){
	printf("cs350sh> ");
	if(fgets(buffer, BUFFERSIZE, stdin) != NULL){
	    status = processInput(buffer);
	}else{
	    printf("Error with input\n");
	}
    }
}

char processInput(char * input){
    if(*(input) == '\n'){
	return 1;
    }
    unsigned char numArgs = 1;
    int i = 0;
    while(*(input + i) != '\0'){
	if(*(input + i) == ' '){
	    ++ numArgs;
	}
	if(*(input + i) == '\n'){
	    *(input + i) = '\0';
	}
	++ i;
    }
    char * arguments[numArgs + 1];
    arguments[numArgs] = '\0';
    const char s[2] = " ";
    char * token;
    i = 0;
    token = strtok(input, s);
    while(token != NULL){
	size_t length = strlen(token);
	arguments[i] = (char *)malloc(length * sizeof(char) + 1);
	*(arguments[i] + length) = '\0';
	strcpy(arguments[i], token);
	token = strtok(NULL, s);
	++ i;
    }
    if(*(arguments[numArgs - 1]) == '&'){
	(arguments[numArgs - 1]) = NULL;
	return runCommand(arguments, numArgs, 1);
    }else{
	return runCommand(arguments, numArgs, 0);
    }
}

char runCommand(char **input, unsigned char numArgs, char background){
    if(strcmp(*input, "listjobs") == 0){
	listJobs(&jobsList);
	freeInput(input, numArgs);
	return 1;
    }
    if(strcmp(*input, "fg") == 0){
	char * ptr;
	int procID = strtol(*(input + 1), &ptr, 10);
	if(procID == 0){
	    printf("Enter a valid process ID\n");
	}else{
	    pid_t pid = deletePID(&jobsList, (pid_t)procID);
	    if(pid == -1){
		printf("PID not found\n");
	    }else{
		int status;
		waitpid(pid, &status, 0);
	    }
	}
	freeInput(input, numArgs);
	return 1;
    }
    if(strcmp(*input, "quit") == 0){
	deleteJobsList(&jobsList);
	freeInput(input, numArgs);
	return 0;
    }
    /* if(background){ */
    /* 	signal(SIGCHLD, handler); */
    /* } */
    pid_t pid = fork();
    if(pid == -1){
	fprintf(stderr, "Fork failed\n");
	exit(1);
    }
    if(pid == 0){
	/*
	  Child process
	*/
	if(execvp(*(input), input) == -1){
	    fprintf(stderr, "Command not found\n");
	    exit(0);
	}
    }else{
	if(background){
	    insert(&jobsList, pid, *input);
	}else{
	    int status;
	    waitpid(pid, &status, 0);
	}
	freeInput(input, numArgs);
    }
    return 1;
}

void freeInput(char **input, int numArgs){
    int i = 0;
    while(i < numArgs){
    	free(*(input + i));
    	++ i;
    }
}
