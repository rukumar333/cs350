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
static void get_processes_info();
static void parse_state(struct task_struct *p);
static void __exit process_exit(void);
static int __init process_init(void);

static long BUFFER_LENGTH = 10000;
static char *buffer = NULL;

static void get_processes_info(){
    int len = 0;
    for_each_process(p){
	if(len + 150 >= BUFFER_LENGTH){
	    BUFFER_LENGTH = BUFFER_LENGTH * 2;
	    krealloc(buffer, BUFFER_LENGTH, GFP_KERNEL);
	}
	char pid_string[25];
	char ppid_string[25];
	char cpu_string[10];
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
	if(buffer[len - 1] == ','){
	    buffer[len - 1] = '\0';
	}
	strcat(buffer, "\n");
	len = strlen(buffer);
    }
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
	    strcat(buffer, "EXIT_ZOMBIE");
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
    struct task_struct *p;
    char *str = kmalloc(sizeof(char) * (count), GFP_KERNEL);
    unsigned long len = 0;
    if(!str){
	printk(KERN_ALERT "ERROR with allocating memory");
	return 0;
    }
    *str = '\0';
    printk(KERN_ALERT "Count: %lu\n", count);
    if(*ppos != 0)
	return 0;
    for_each_process(p){
	printk(KERN_ALERT "Length: %lu\n", len);
	if(len + 100 < count){
	    char pid_string[25];
	    char ppid_string[25];
	    char cpu_string[10];
	    long state = p->state;
	    long exit_state = p->exit_state;
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
	    len = strlen(str);
	    /* strcat(str, "STATE="); */
	    /* if(state == TASK_RUNNING){ */
	    /* 	strcat(str, "TASK_RUNNING,"); */
	    /* }else{	 */
	    /* 	if(state & TASK_INTERRUPTIBLE) */
	    /* 	  strcat(str, "TASK_INTERRUPTIBLE,"); */
	    /* 	if(state & TASK_UNINTERRUPTIBLE) */
	    /* 	  strcat(str, "TASK_UNINTERRUPTIBLE,"); */
	    /* 	if(state & __TASK_STOPPED) */
	    /* 	  strcat(str, "TASK_STOPPED,"); */
	    /* 	if(state & __TASK_TRACED) */
	    /* 	  strcat(str, "TASK_TRACED,"); */
	    /* 	if(exit_state & EXIT_DEAD){ */
	    /* 	  if(exit_state & EXIT_ZOMBIE){ */
	    /* 	    strcat(str, "EXIT_TRACE,"); */
	    /* 	  }else{ */
	    /* 	    strcat(str, "EXIT_DEAD,"); */
	    /* 	  } */
	    /* 	} */
	    /* 	if(exit_state & EXIT_ZOMBIE) */
	    /* 	  strcat(str, "EXIT_ZOMBIE"); */
	    /* 	if(state & TASK_DEAD) */
	    /* 	  strcat(str, "TASK_DEAD,"); */
	    /* 	if(state & TASK_WAKEKILL) */
	    /* 	  strcat(str, "TASK_WAKEKILL,"); */
	    /* 	if(state & TASK_WAKING) */
	    /* 	  strcat(str, "TASK_WAKING,"); */
	    /* 	if(state & TASK_PARKED) */
	    /* 	  strcat(str, "TASK_PARKED,"); */
	    /* 	if(state & TASK_NOLOAD) */
	    /* 	  strcat(str, "TASK_NOLOAD,"); */
	    /* 	if(state & TASK_NEW) */
	    /* 	  strcat(str, "TASK_NEW,"); */
	    /* 	if(state & TASK_STATE_MAX) */
	    /* 	  strcat(str, "TASK_STATE_MAX,"); */
	    /* } */
	    /* if(str[len - 1] == ','){ */
	    /* 	str[len - 1] = '\0'; */
	    /* } */
	    /* strcat(str, "\n"); */
	}
    }
    len = strlen(str);
    if(copy_to_user(buf, str, len)){
	kfree(str);
	return -EINVAL;
    }
    kfree(str);
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
    int ret;
    printk(KERN_DEBUG "process_list mod initiated");
    buffer = kmalloc(sizeof(char) * BUFFER_LENGTH);
    memset(buffer, '\0', sizeof(char) * BUFFER_LENGTH);
    ret = misc_register(&process_list);
    if(ret)
	printk(KERN_ERR "Unable to register process_list misc device\n");
    return ret;
}

module_init(process_init);

static void __exit process_exit(void){
    misc_deregister(&process_list);
    kfree(buffer);
    printk(KERN_ALERT "Module exiting - process list\n");
}
module_exit(process_exit);
