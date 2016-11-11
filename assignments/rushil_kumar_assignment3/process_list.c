#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <asm/uaccess.h>
MODULE_LICENSE("GPL");

static ssize_t read_process_list(struct file * file, char * buf, size_t count, loff_t * ppos);
static char get_processes_info(void);
static void parse_state(struct task_struct *p);
static void __exit process_exit(void);
static int __init process_init(void);
static char resize_buffer(void);

static long BUFFER_LENGTH = 10000;
static char *buffer = NULL;

static char resize_buffer(void){
  char *new_mem;
  printk(KERN_DEBUG "Resizing buffer");
  BUFFER_LENGTH = BUFFER_LENGTH * 2;
  new_mem = kcalloc(sizeof(char), BUFFER_LENGTH, GFP_KERNEL);
  if(new_mem == NULL){
      return 0;
  }
  strcpy(new_mem, buffer);
  kfree(buffer);
  buffer = new_mem;
  return 1;
};

static char get_processes_info(void){
  int len = 0;
  struct task_struct *p;
  for_each_process(p){
    char pid_string[25];
    char ppid_string[25];
    char cpu_string[10];
    if(len + 150 >= BUFFER_LENGTH){
	if(resize_buffer() == 0){
	    return 0;
	}
    }
    snprintf(pid_string, 25, "%d", p->pid);
    strcat(buffer, "PID=");
    strcat(buffer, pid_string);
    strcat(buffer, " ");
    snprintf(ppid_string, 25, "%d", p->parent->pid);
    strcat(buffer, "PPID=");
    strcat(buffer, ppid_string);
    strcat(buffer, " ");
    snprintf(cpu_string, 10, "%d", task_cpu(p));
    strcat(buffer, "CPU=");
    strcat(buffer, cpu_string);
    strcat(buffer, " ");
    parse_state(p);
    len = strlen(buffer);
    if(buffer[len - 1] == ','){
      buffer[len - 1] = '\0';
    }
    strcat(buffer, "\n");
    len = strlen(buffer);
  }
  return 1;
};

static void parse_state(struct task_struct *p){
  long state = p->state;
  long exit_state = p->exit_state;
  strcat(buffer, "STATE=");
  if(state == TASK_RUNNING){
    strcat(buffer, "TASK_RUNNING,");
  }else{
    if(state & TASK_INTERRUPTIBLE)
      strcat(buffer, "TASK_INTERRUPTIBLE,");
    if(state & TASK_UNINTERRUPTIBLE)
      strcat(buffer, "TASK_UNINTERRUPTIBLE,");
    if(state & __TASK_STOPPED)
      strcat(buffer, "TASK_STOPPED,");
    if(state & __TASK_TRACED)
      strcat(buffer, "TASK_TRACED,");
    if(exit_state & EXIT_DEAD){
      if(exit_state & EXIT_ZOMBIE){
	strcat(buffer, "EXIT_TRACE,");
      }else{
	strcat(buffer, "EXIT_DEAD,");
      }
    }
    if(exit_state & EXIT_ZOMBIE)
      strcat(buffer, "EXIT_ZOMBIE,");
    if(state & TASK_DEAD)
      strcat(buffer, "TASK_DEAD,");
    if(state & TASK_WAKEKILL)
      strcat(buffer, "TASK_WAKEKILL,");
    if(state & TASK_WAKING)
      strcat(buffer, "TASK_WAKING,");
    if(state & TASK_PARKED)
      strcat(buffer, "TASK_PARKED,");
    if(state & TASK_NOLOAD)
      strcat(buffer, "TASK_NOLOAD,");
    if(state & TASK_NEW)
      strcat(buffer, "TASK_NEW,");
    if(state & TASK_STATE_MAX)
      strcat(buffer, "TASK_STATE_MAX,");
  }
};

static ssize_t read_process_list(struct file * file, char * buf, size_t count, loff_t * ppos){
  int len;  
  if(*ppos == 0){
      if(get_processes_info() == 0){
	  printk(KERN_ERR "Error with reading process information\n");
	  return -EINVAL;
      }
  }
  len = strlen(buffer);
  if(*ppos >= len){
    *ppos = 0;
    return 0;
  }else{
    if(copy_to_user(buf, (buffer + *ppos), count)){
      printk(KERN_ERR "Unable to copy buffer to user space\n");
      return -EINVAL;
    }
    *ppos = *ppos + count;
    return count;
  }
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
  int ret;
  printk(KERN_DEBUG "process_list mod initiated");
  buffer = kcalloc(sizeof(char), BUFFER_LENGTH, GFP_KERNEL);
  ret = misc_register(&process_list);
  if(ret)
    printk(KERN_ERR "Unable to register process_list misc device\n");
  return ret;
}

module_init(process_init);

static void __exit process_exit(void){
  misc_deregister(&process_list);
  kfree(buffer);
  printk(KERN_DEBUG "Module exiting - process list\n");
}
module_exit(process_exit);
