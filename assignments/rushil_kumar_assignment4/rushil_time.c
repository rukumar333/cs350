#include <linux/linkage.h>
#include <linux/export.h>
#include <linux/time.h>
#include <asm/uaccess.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/timekeeping.h>

asmlinkage int sys_my_xtime(struct timespec *current_time){
  int ret = 0;
  struct timespec my_kernel_timespec;
  ktime_get_ts(&my_kernel_timespec);
  printk(KERN_DEBUG "Time: %ld\n", my_kernel_timespec.tv_nsec);
  ret = copy_to_user(current_time, &my_kernel_timespec, sizeof(struct timespec));
  if(ret != 0){
    return -EFAULT;
  }else{
    return 0;
  }
}

EXPORT_SYMBOL(sys_my_xtime);
