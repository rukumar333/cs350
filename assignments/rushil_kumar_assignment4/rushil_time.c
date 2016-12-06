#include <linux/linkage.h>
#include <linux/export.h>
#include <linux/time.h>
#include <asm/uaccess.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/timekeeping.h>

asmlinkage int sys_my_xtime(struct timespec *current_time){
  if(!access_ok(VERIFY_WRITE,current_time,sizeof(struct timespec))){
    printk(KERN_ERR "Invalid memory\n");
    return -EFAULT;
  }
  current_time->tv_sec = 0;
  current_time->tv_nsec = 0;
  ktime_get_ts(current_time);
  printk(KERN_DEBUG "Time: %ld\n", current_time->tv_nsec);
  return 0;
}

EXPORT_SYMBOL(sys_my_xtime);
