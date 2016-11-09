#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
MODULE_LICENSE("DUAL BSD/GPL");

static ssize_t read_process_list(struct file * file, char * buf, size_t count, loff_t * ppos){
  struct task_struct *p;
  /* for_each_process(p){ */
    
  /* } */
  return count;
}

static const struct file_operations process_list_fops = {
  .owner = THIS_MODULE,
  .read = read_process_list
};

static struct miscdevice process_list = {
  MISC_DYNAMIC_MINOR,
  "process_list",
  &process_list_fops
};

static int __init process_init(){
  struct task_struct *p;
  for_each_process(p){
    printk(KERN_ALERT "PID: %d", (int)task_pid_nr(p));
  }
  int ret;
  ret = misc_register(&process_list);
  if(ret)
    printk(KERN_ERR "Unable to register process_list misc device\n");
  return ret;
}

module_init(process_init);

static void __exit process_exit(){
  misc_deregister(&process_list);
  printk(KERN_ALERT "Module exiting - process list\n");
}
module_exit(process_exit);
