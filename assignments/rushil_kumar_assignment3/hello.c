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
  /* str[0] = '\0'; */
  int len = 0;
  if(len + 60 < count){
    for_each_process(p){
      char pid_string[25];
      char ppid_string[25];
      char cpu_string[10];
      long state = p->state;
      long exit_state = p->exit_state;
      len = strlen(str);
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
      strcat(str, "STATE=");
      if(state == TASK_RUNNING){
	strcat(str, "TASK_RUNNING,");
      }else{	
	if(state & TASK_INTERRUPTIBLE)
	  strcat(str, "TASK_INTERRUPTIBLE,");
	if(state & TASK_UNINTERRUPTIBLE)
	  strcat(str, "TASK_UNINTERRUPTIBLE,");
	if(state & __TASK_STOPPED)
	  strcat(str, "TASK_STOPPED,");
	if(state & __TASK_TRACED)
	  strcat(str, "TASK_TRACED,");
	if(exit_state & EXIT_DEAD){
	  if(exit_state & EXIT_ZOMBIE){
	    strcat(str, "EXIT_TRACE,");
	  }else{
	    strcat(str, "EXIT_DEAD,");
	  }
	}
	if(exit_state & EXIT_ZOMBIE)
	  strcat(str, "EXIT_ZOMBIE");
	if(state & TASK_DEAD)
	  strcat(str, "TASK_DEAD,");
	if(state & TASK_WAKEKILL)
	  strcat(str, "TASK_WAKEKILL,");
	if(state & TASK_WAKING)
	  strcat(str, "TASK_WAKING,");
	if(state & TASK_PARKED)
	  strcat(str, "TASK_PARKED,");
	if(state & TASK_NOLOAD)
	  strcat(str, "TASK_NOLOAD,");
	if(state & TASK_NEW)
	  strcat(str, "TASK_NEW,");
	if(state & TASK_STATE_MAX)
	  strcat(str, "TASK_STATE_MAX,");
      }
      /* if(str[len - 1] == ','){ */
      /* 	str[len - 1] = '\0'; */
      /* } */
      /* strcat(str, "\n"); */
    }
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
  /* printk(KERN_ALERT "Module exiting - process list\n"); */
}
module_exit(process_exit);
