#include <linux/time.h>
#include <linux/unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define _sys_bad_call_ 330
#define CODE 3

void testBadCall(char code);

int main(){
  testBadCall(CODE);
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
