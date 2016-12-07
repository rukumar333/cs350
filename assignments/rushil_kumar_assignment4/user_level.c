#include <linux/time.h>
#include <linux/unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define _sys_my_xtime_ 329
#define _sys_bad_call_ 330

char testSyscall();
void testBadCall(char code);

int main(){
  /* if(testSyscall() == -1) */
  /*   return -1; */
  testBadCall(0);
  return 0;
}

char testSyscall(){
  struct timespec my_timespec;
  int ret;
  ret = syscall(_sys_my_xtime_, my_timespec);
  if(ret < 0){
    /* perror("my_xtime()"); */
    printf("ERROR: Syscall return value: %d\n",ret);
    return -1;
  }
  printf("Time in nanoseconds: %ld\n", my_timespec.tv_nsec);
  printf("Time in seconds: %ld\n", my_timespec.tv_sec);  
  return 0;
}

void testBadCall(char code){
  switch(code){
  case 0:
    printf("Dividing by zero!\n");
    break;
  case 1:
    printf("Deferencing null pointer\n");
    break;
  case 3:
    printf("No return value\n");
    break;
  }
  int ret = syscall(_sys_bad_call_, code);
  if(ret < 0){
    printf("ERROR: Syscall return value: %d\n",ret);
  }else{    
    printf("Success: Syscall return value: %d\n", ret);
  }
}
