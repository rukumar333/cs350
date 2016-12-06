#include <linux/linkage.h>
#include <linux/export.h>
#include <asm/uaccess.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/stddef.h>
#include <stdio.h>

int divideByZero();
int deferenceNullPtr();
void useCLibraryFunc();

asmlinkage int sys_bad_call(char code){
  switch(code){
  case 0:
    divideByZero();
    break;
  case 1:
    deferenceNullPtr();
    break;
  case 2:
    useCLibraryFunc();
    break;
  }
  if(code != 3){
    return 0;
  }
}

int divideByZero(){
  int x = 0;
  int y = 10;
  return y / x;
}

int deferenceNullPtr(){
  int *foo = NULL;
  int x = *foo;
  return x;
}

void useCLibraryFunc(){
  printf("Hello world C Library printk\n");
};

EXPORT_SYMBOL(sys_bad_call);
