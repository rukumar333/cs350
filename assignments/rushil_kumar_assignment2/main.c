#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "JobsList.h"

int BUFFERSIZE = 100000;

JobsList jobsList;

void runShell();
char parseInput(char * input);
char processInput(char **input, unsigned char numArgs);
char recurseProcessInput(char **input, unsigned char numArgs, unsigned char currentArg, int pipe[2]);
char checkPipeRedir(char * input);
void runCommand(char **input, unsigned char numArgs);
void handler(int sig);
void freeInput(char **input, int numArgs);
void foregroundProcess(char **input);
void directFrom(char * file);
void directTo(char * file, char append);
void pipeFrom(int * pipe);
void pipeTo(int * pipe);
void normalPipe();

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
	    status = parseInput(buffer);
	}else{
	    printf("Error with input\n");
	    exit(1);
	}
    }
}

char processInput(char **input, unsigned char numArgs){
    if(strcmp(*input, "listjobs") == 0){
	listJobs(&jobsList);
	freeInput(input, numArgs);
	return 1;
    }
    if(strcmp(*input, "fg") == 0){
	foregroundProcess(input);
	freeInput(input, numArgs);
	return 1;
    }
    if(strcmp(*input, "quit") == 0){
	deleteJobsList(&jobsList);
	freeInput(input, numArgs);
	return 0;
    }
    char background = **(input + numArgs - 1) == '&';
    if(background){
	free(*(input + numArgs - 1));
	*(input + numArgs - 1) = NULL;
	-- numArgs;
    }
    pid_t pid;
    if((pid = fork()) == -1){
	fprintf(stderr, "Fork failed\n");
    	exit(1);
    }
    /* signal(SIGCHLD, handler); */
    if(pid == 0){
	//Child Process
	return recurseProcessInput(input, numArgs, 0, NULL);
	exit(0);
    }else{
	//Parent Process
	if(background){
	    insert(&jobsList, pid, *input);
	}else{
	    int status;
	    /* printf("Waiting\n"); */
	    waitpid(pid, &status, 0);
	    /* printf("Finished\n"); */
	}
    }
    return 1;
}

char recurseProcessInput(char **input, unsigned char numArgs, unsigned char currentArg, int * firstPipe){
    char useInputPipe = 0;
    char useOutputPipe = 0;
    /* printf("currentArg: %d\n", currentArg); */
    /* printf("currentArg: %s\n", *(input + currentArg)); */
    /* printf("numArgs: %d\n", numArgs); */
    if(currentArg >= numArgs){
	freeInput(input, numArgs);
	exit(0);
    }
    if(firstPipe != NULL){	
	//Set input from pipe
	useInputPipe = 1;
	/* pipeFrom(firstPipe); */
    }
    char * command[numArgs + 1 - currentArg];
    unsigned char numCommandArgs = 0;    
    int i = 0;
    char status = checkPipeRedir(*(input + currentArg + i));
    while(currentArg + i < numArgs && (status = checkPipeRedir(*(input + currentArg + i))) == -1){
    /* while(i < numArgs */
	size_t length = strlen(*(input + currentArg + i));
	command[i] = (char *)malloc(length * sizeof(char) + 1);
	*(command[i] + length) = '\0';
	strcpy(command[i], *(input + currentArg + i));
	++ i;
	++ numCommandArgs;
    }
    command[i] = NULL;
    int secondPipe[2];
    /* char usedPipe = 0; */
    switch(status){
    case 0:
	//Set input from file
	directFrom(*(input + currentArg + i + 1));
	//Set output to pipe
	if(currentArg + i + 2 < numArgs){
	    char innerStatus = checkPipeRedir(*(input + currentArg + i + 2));
	    switch(innerStatus){
	    case 1:
		//Set output to file w/o append
		directTo(*(input + currentArg + i + 3), 0);
		i = i + 2;
		break;
	    case 2:
		//Set output to file w/ append
		directTo(*(input + currentArg + i + 3), 1);
		i = i + 2;
		break;
	    case 3:
		//Set output to pipe
		if(pipe(secondPipe) == -1){
		    perror("pipe");
		    exit(1);
		}
		/* pipeTo(secondPipe); */
		useOutputPipe = 1;
		/* usedPipe = 1; */
		i = i + 1;
		break;		
	    }
	}
	i = i + 2;
	break;
    case 1:
	//Set output to file w/o append
	directTo(*(input + currentArg + i + 1), 0);
	i = i + 2;
	break;
    case 2:
	//Set output to file w/ append
	directTo(*(input + currentArg + i + 1), 1);
	i = i + 2;
	break;
    case 3:
	//Set output to pipe
	if(pipe(secondPipe) == -1){
	    perror("pipe");
	    exit(1);
	}
	useOutputPipe = 1;
	/* pipeTo(secondPipe); */	
	/* usedPipe = 1; */
	i = i + 1;
	break;
    }
    pid_t pid;
    if((pid = fork()) == -1){
    	fprintf(stderr, "Fork failed\n");
    	exit(1);
    }
    if(pid == 0){
	/*
	  Child Process
	 */
	/* recurseProcessInput(input, numArgs, currentArg + i, usedPipe ? secondPipe : NULL); */
	if(useInputPipe){
	    pipeFrom(firstPipe);
	}
	if(useOutputPipe){
	    pipeTo(secondPipe);
	}
	/* printf("PID: %d -> Command: %s\n", getpid(), *command); */
	runCommand(command, numCommandArgs);
    }else{
	/*
	  Parent Process
	 */
	freeInput(command, numCommandArgs);
	close(*(secondPipe + 1));
	waitpid(pid, NULL, 0);
	if(useOutputPipe){
	    recurseProcessInput(input, numArgs, currentArg + i, secondPipe);
	}else{
	    recurseProcessInput(input, numArgs, currentArg + i, NULL);	    
	}
	/* printf("Exiting recursive\n"); */
	freeInput(input, numArgs);
	exit(0);
	/* return recurseProcessInput(input, numArgs, currentArg + i, useOutputPipe != 1 ? secondPipe : NULL); */
    }
    
    return 0;
}

char checkPipeRedir(char * input){
    char val = -1;
    if(strcmp(input, "<") == 0){
	val = 0;
	/* return 0; */
    }
    if(strcmp(input, ">") == 0){
	val = 1;
	/* return 1; */
    }
    if(strcmp(input, ">>") == 0){
	val = 2;
	/* return 2; */
    }
    if(strcmp(input, "|") == 0){
	val = 3;
	/* return 3; */
    }
    return val;
    /* return -1; */
}

void runCommand(char **input, unsigned char numArgs){
    /* if(strcmp(*input, "listjobs") == 0){ */
    /* 	listJobs(&jobsList); */
    /* 	freeInput(input, numArgs); */
    /* 	exit(0); */
    /* 	/\* return 1; *\/ */
    /* } */
    /* if(strcmp(*input, "fg") == 0){ */
    /* 	foregroundProcess(input); */
    /* 	freeInput(input, numArgs); */
    /* 	exit(0); */
    /* 	/\* return 1; *\/ */
    /* } */
    if(execvp(*(input), input) == -1){
	fprintf(stderr, "Command not found\n");
	exit(1);
    }
}

void foregroundProcess(char **input){
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
}

void directFrom(char * file){
    int fd = open(file, O_RDONLY);
    dup2(fd, 0);
    close(fd);
}

void directTo(char * file, char append){
    int fd;
    if(append){
	fd = open(file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    }else{
	fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    }
    dup2(fd, 1);
    close(fd);
}

void pipeTo(int * pipe){
    close(1);
    dup2(*(pipe + 1), 1);
    close(*(pipe));
}

void pipeFrom(int * pipe){
    close(0);
    dup2(*(pipe), 0);
    close(*(pipe + 1));
}

void normalPipe(){
    /* close(1); */
    /* close(0); */
    /* dup2(stdin, 0); */
    /* dup2(stdout, 1); */
}

void freeInput(char **input, int numArgs){
    int i = 0;
    while(i < numArgs && *(input + i) != NULL){
    	free(*(input + i));
    	++ i;
    }
}

char parseInput(char * input){
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
    return processInput(arguments, numArgs);
}
