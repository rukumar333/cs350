#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
MODULE_LICENSE("DUAL BSD/GPL");

static ssize_t read_process_list(struct file * file, char * buf, size_t count, loff_t * ppos){
  struct task_struct *p;
  char str[count];
  int len;
  for_each_process(p){
    char pid_string[25];
    char ppid_string[25];
    char cpu_string[10];
    len = strlen(str);
    if(len + 60 >= count)
      return -EINVAL;
    snprintf(pid_string, 25, "%d", p->pid);
    strcat(str, "PID=");
    strcat(str, pid_string);
    strcat(str, " ");
    snprintf(ppid_string, 25, "%d", p->parent->pid);
    strcat(str, "PPID=");
    strcat(str, ppid_string);
    strcat(str, " ");
    snprintf(cpu_string, 10, "%d", task_cpu(p));
    strcat(str, "CPU=");
    strcat(str, cpu_string);
    strcat(str, " ");    
  }
  len = strlen(str);
  if(copy_to_user(buf, str, len))
    return -EINVAL;
  *ppos = len;  
  return len;
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

static int __init process_init(void){
  struct task_struct *p;
  int ret;
  /* for_each_process(p){ */
  /*   printk(KERN_DEBUG "PID: %d\n", (int)task_pid_nr(p)); */
  /* } */
  ret = misc_register(&process_list);
  if(ret)
    printk(KERN_ERR "Unable to register process_list misc device\n");
  return ret;
}

module_init(process_init);

static void __exit process_exit(void){
  misc_deregister(&process_list);
  printk(KERN_ALERT "Module exiting - process list\n");
}
module_exit(process_exit);
