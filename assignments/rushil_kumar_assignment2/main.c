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
void forkParentChild(char **input, unsigned char numArgs, unsigned char currentArg, char useOutputPipe, int * firstPipe, int * secondPipe, char **command, unsigned char numCommandArgs);
unsigned char setInputOutput(char **input, unsigned char numArgs, unsigned char currentArg, char status, char * useInputPipe);
char handleProgramCommands(char **input, unsigned char numArgs);
char checkPipeRedir(char * input);
void runCommand(char **input, unsigned char numArgs);
void handler(int sig);
void freeInput(char **input, int numArgs);
void foregroundProcess(char **input);
void directFrom(char * file);
void directTo(char * file, char append);
void pipeFrom(int * pipe);
void pipeTo(int * pipe);

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
    char status = handleProgramCommands(input, numArgs);
    if(status != -1)
	return status;
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
    if(pid == 0){
	return recurseProcessInput(input, numArgs, 0, NULL);
	exit(0);
    }else{
	if(background){
	    insert(&jobsList, pid, *input);
	}else{
	    int status;
	    if(waitpid(pid, &status, 0) == -1){
		fprintf(stderr, "waitpid failed\n");
		exit(1);	
	    }
	}
    }
    return 1;
}

char handleProgramCommands(char **input, unsigned char numArgs){
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
    return -1;
}

char recurseProcessInput(char **input, unsigned char numArgs, unsigned char currentArg, int * firstPipe){
    char useOutputPipe = 0;
    if(currentArg >= numArgs){
	freeInput(input, numArgs);
	exit(0);
    }
    char * command[numArgs + 1 - currentArg];
    unsigned char numCommandArgs = 0;    
    char status = checkPipeRedir(*(input + currentArg));
    //Move commands into array of c-style strings
    while(currentArg < numArgs && (status = checkPipeRedir(*(input + currentArg))) == -1){
	size_t length = strlen(*(input + currentArg));
	command[numCommandArgs] = (char *)malloc(length * sizeof(char) + 1);
	*(command[numCommandArgs] + length) = '\0';
	strcpy(command[numCommandArgs], *(input + currentArg));
	++ currentArg;
	++ numCommandArgs;
    }
    //Null terminate list of commands
    command[numCommandArgs] = NULL;
    int secondPipe[2];
    currentArg = setInputOutput(input, numArgs, currentArg, status, &useOutputPipe);
    if(useOutputPipe){
	if(pipe(secondPipe) == -1){
	    perror("pipe");
	    exit(1);
	}
    }
    forkParentChild(input, numArgs, currentArg, useOutputPipe, firstPipe, secondPipe, command, numCommandArgs);
    return 0;
}

void forkParentChild(char **input, unsigned char numArgs, unsigned char currentArg, char useOutputPipe, int * firstPipe, int * secondPipe, char **command, unsigned char numCommandArgs){
    pid_t pid;
    if((pid = fork()) == -1){
    	fprintf(stderr, "Fork failed\n");
    	exit(1);
    }
    if(pid == 0){
	if(firstPipe != NULL)
	    pipeFrom(firstPipe);
	if(useOutputPipe)
	    pipeTo(secondPipe);
	runCommand(command, numCommandArgs);
    }else{
	freeInput(command, numCommandArgs);
	close(*(secondPipe + 1));
	if(waitpid(pid, NULL, 0) == -1){
	    fprintf(stderr, "waitpid failed\n");
	    exit(1);
	}
	if(useOutputPipe)
	    recurseProcessInput(input, numArgs, currentArg, secondPipe);
	else
	    recurseProcessInput(input, numArgs, currentArg, NULL);
	freeInput(input, numArgs);
	exit(0);
    }
}

unsigned char setInputOutput(char **input, unsigned char numArgs, unsigned char currentArg, char status, char * useOutputPipe){
    switch(status){
    case 0:
	//Set input from file
	directFrom(*(input + currentArg + 1));
	currentArg = currentArg + 2;
	//Set output to pipe
	if(currentArg < numArgs){
	    char innerStatus = checkPipeRedir(*(input + currentArg));
	    return setInputOutput(input, numArgs, currentArg, innerStatus, useOutputPipe);
	}
	break;
    case 1:
	//Set output to file w/o append
	directTo(*(input + currentArg + 1), 0);
	currentArg = currentArg + 2;
	break;
    case 2:
	//Set output to file w/ append
	directTo(*(input + currentArg + 1), 1);
	currentArg = currentArg + 2;
	break;
    case 3:
	//Set output to pipe
	*useOutputPipe = 1;
	currentArg = currentArg + 1;
	break;
    }
    return currentArg;
}

char checkPipeRedir(char * input){
    char val = -1;
    if(strcmp(input, "<") == 0){
	val = 0;
    }
    if(strcmp(input, ">") == 0){
	val = 1;
    }
    if(strcmp(input, ">>") == 0){
	val = 2;
    }
    if(strcmp(input, "|") == 0){
	val = 3;
    }
    return val;
}

void runCommand(char **input, unsigned char numArgs){
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
	    if(waitpid(pid, &status, 0) == -1){
		fprintf(stderr, "waitpid failed\n");
		exit(1);
	    }
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

void freeInput(char **input, int numArgs){
    int i = 0;
    while(i < numArgs && *(input + i) != NULL){
    	free(*(input + i));
    	++ i;
    }
}

char parseInput(char * input){
    if(*(input) == '\n')
	return 1;
    unsigned char numArgs = 1;
    int i = 0;
    while(*(input + i) != '\0'){
	if(*(input + i) == ' ')
	    ++ numArgs;
	if(*(input + i) == '\n')
	    *(input + i) = '\0';
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
