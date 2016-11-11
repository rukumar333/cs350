#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
  pid_t child_pid;
  child_pid = fork();
  if(child_pid > 0){
    sleep(60);
  }else{
    exit(0);
  }
  return 0;
}
