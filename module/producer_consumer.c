#include <linux/init.h>    
#include <linux/module.h>    
#include <linux/kernel.h>   

#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/semaphore.h>
#include <linux/slab.h>

#define EXIT_ZOMBIE 0x00000020

MODULE_LICENSE("GPL");    
MODULE_AUTHOR("Danny Hoang");    


// non negative
static uint prod = 1;
module_param(prod, uint, 0);

static uint cons = 1;
module_param(cons, uint, 0);

// may only be positive
static uint size = 1;
module_param(size, uint, 0);

// UID of test user. 
static uint uid = 1000;
module_param(uid, uint, 0);

// number of empty slots
// producer waits for this 
struct semaphore empty; 
// number of full slots
// consumer waits for this
struct semaphore full;
// mutex lock, only one producer or consumer may run at a time.
struct semaphore lock;

struct task_struct *buffer;
struct task_struct *consumer_threads;
struct task_struct *producer_threads;




static int __init pc_init(void) {    

  //invalid inputs
  if((size < 1) || (prod < 0) || (cons < 0)) {
    printk(KERN_INFO "Invalid input, please note that buffer size must be positive and producer & consumer must be non negative.\n");
    return -1;
  }

  //debug
  printk(KERN_INFO "Starting program with %u producers with %u consumers\n", prod, cons);    
  printk(KERN_INFO "Buffer size = buf[%u], UID = %u\n", size, uid);    

  // initialize shared buffer
  // note: kmalloc allocates to kernel-space memory 
  const struct task_struct *structptr;
  buffer = kmalloc(size * sizeof(structptr), GFP_KERNEL);
  printk(KERN_INFO "Buffer initialized");    

  sema_init(&empty, size);
  sema_init(&full, 0);

  
  // producer_threads = kmalloc(prod * sizeof(*task_struct));
  // for (int i = 0; i < prod; i++) {
  //   producer_threads[i] = kthread_run(producer_thread, NULL, "thread-%d", 1);
  // }
  return 0;    
}    

static void __exit pc_exit(void) {    
  printk(KERN_INFO "Goodbye, World!\n");    
  kfree(buffer);
  kfree(producer_threads);
}    

static int producer_thread(void* arg) {
  size_t process_counter = 0;
  struct task_struct* task;

  for_each_process(task) {
    if(task->cred->uid.val != uid)
      continue;

    if (task->exit_state & EXIT_ZOMBIE) {
      //zombie process
      //val = sem_getvalue(sem, &temp);
      
      int idx = down_interruptible(&empty);
      printk(KERN_INFO "[%s] has produced zombie process with pid %u and parent id %u\n", 
             //TODO get name here
             "UNKNOWN GUY",
             task->pid
             ); 
      up(&full);
    }
    ++process_counter;
  }
}


// cant run anything here
//producer = kthread_run(producer_thread, NULL, "thread-%d", 1);


module_init(pc_init);    
module_exit(pc_exit);
