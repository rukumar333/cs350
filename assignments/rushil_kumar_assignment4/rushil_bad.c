#include <linux/linkage.h>
#include <linux/export.h>
#include <asm/uaccess.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/stddef.h>

int divideByZero(void);
int deferenceNullPtr(void);
void useCLibraryFunc(void);

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

int divideByZero(void){
  int x = 0;
  int y = 10;
  return y / x;
}

int deferenceNullPtr(void){
  int *foo = NULL;
  int x = *foo;
  return x;
}

void useCLibraryFunc(void){
  /* printf("Hello world C Library printk\n"); */
};

EXPORT_SYMBOL(sys_bad_call);
