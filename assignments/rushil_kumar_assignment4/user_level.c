#include <linux/time.h>
#include <linux/unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define _sys_my_xtime_ 329

char testSyscall();

int main(){
  if(testSyscall() == -1)
    return -1;
  return 0;
}

char testSyscall(){
  struct timespec my_timespec;
  int ret;
  ret = syscall(_sys_my_xtime_, &my_timespec);
  if(ret < 0){
    printf("Syscall return value :%d; errno %d\n",ret, errno);
    return -1;
  }
  printf("Time in nanoseconds: %ld\n", my_timespec.tv_nsec);
  printf("Time in seconds: %ld\n", my_timespec.tv_sec);  
  return 0;
}
