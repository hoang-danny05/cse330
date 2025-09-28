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
static struct semaphore empty; 
// number of full slots
// consumer waits for this
static struct semaphore full;
// mutex lock, only one producer or consumer may run at a time.
static struct semaphore lock;

// semaphore value trackers
static int offset = 0;
static int empty_val = 0;
static int full_val = 0;


/* 
 * ROUND ROBIN ATTEMPT
 *
 * let buffer have one item within it 
 * [full| null | null | null]
 * empty == 3
 * full == 1
 * offset == 0
 *
 * producer waits for empty, succeeds, acquires lock 
 *    (accesses size-empty + offset) = 4-3+0
 * [full| full | null | null]
 * empty == 2
 * full == 2
 * offset == 0
 *
 * consumer consumes buf[0]
 *    (acceses offset % size) = 0 % 4
 * [null| full | null | null]
 * empty == 3
 * full == 1
 * offset == 1
 *
 * producer fills buf[2]
 *    (size - empty + offset) = 4 - 3 + 1
 *    MOD size!!
 * [null| full | full | null]
 * empty == 2
 * full == 2
 * offset == 1
 *
 *
 * formulas 
 *  producer : (size - empty + offset) % size 
 *    same as (full + offset) % size
 *  consumer : offset % size
 * */

static struct task_struct **buffer;
static struct task_struct **consumer_threads;
static struct task_struct **producer_threads;

int _ = 0; //unused int 

typedef struct thread_args {
  int idx;
} thread_args;

static int producer_thread(void* arg) {
  struct task_struct* task;
  thread_args *args = (thread_args *) arg;

  for_each_process(task) {
    if(task->cred->uid.val != uid)
      continue;

    if (task->exit_state & EXIT_ZOMBIE) {
      
      //acuire a spot to produce
      _ = down_interruptible(&empty);

      //aquire lock
      _ = down_interruptible(&lock);


      printk(KERN_INFO "[%s] has produced zombie process with pid %d and parent pid %d\n", 
             producer_threads[args->idx]->comm,
             task->pid,
             task->parent->pid
             ); 

      int idx = (size - empty_val + offset) % size;
      printk(KERN_INFO "> [p] debug: idx=%d", 
             idx
             ); 

      // "produce"
      buffer[idx] = task;

      // update vars while you have lock 
      empty_val -= 1;
      full_val += 1;

      // signal lock
      up(&lock);

      // alert that full is allowed
      up(&full);
    }

    if (kthread_should_stop()) {
      break;
    }
  }
  kfree(arg);
  return 0;
}


static int consumer_thread(void* arg) {
  thread_args *args = (thread_args *) arg;

  while (!kthread_should_stop()) {

    // aquire a spot to consume 
    _ = down_interruptible(&full);

    //aquire lock
    _ = down_interruptible(&lock);

    
    int idx = (offset) % size;
    printk(KERN_INFO "> [c] debug: idx=%d", 
            idx
            ); 

    // "consume"
    struct task_struct *task = buffer[idx];
    buffer[idx] = 0;


    printk(KERN_INFO "[%s] has consumed zombie process with pid %d and parent pid %d\n", 
            consumer_threads[args->idx]->comm,
            task->pid,
            task->parent->pid
            ); 

    // update vars while you have lock 
    empty_val += 1;
    full_val -= 1;
    offset += 1;

    // signal lock
    up(&lock);

    // alert that full is allowed
    up(&empty);
  }

  kfree(arg);
  return 0;
}


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

  // this is just for sizeof, but i know sizeof(ptr) is 8
  //const struct task_struct examp_struct;

  buffer = kmalloc(size * sizeof(struct task_struct*), GFP_KERNEL);
  for (int i = 0; i < size; i++) {
    buffer[i] = (struct task_struct*)0;
  }

  printk(KERN_INFO "Buffer initialized");    

  sema_init(&empty, size);
  sema_init(&full, 0);
  sema_init(&lock, 1);

  empty_val = size;
  full_val = 0;

  
  producer_threads = kmalloc(prod * sizeof(struct task_struct*), GFP_KERNEL);
  for (int i = 0; i < prod; i++) {
    thread_args *args = (thread_args *)kmalloc(sizeof(thread_args), GFP_KERNEL);
    args->idx = i;
    producer_threads[i] = kthread_run(producer_thread, args, "Producer-%d", i + 1);
  }

  consumer_threads = kmalloc(cons * sizeof(struct task_struct*), GFP_KERNEL);
  for (int i = 0; i < cons; i++) {
    thread_args *args = (thread_args *)kmalloc(sizeof(thread_args), GFP_KERNEL);
    args->idx = i;
    consumer_threads[i] = kthread_run(consumer_thread, args, "Consumer-%d", i + 1);
  }


  return 0;    
}    




static void __exit pc_exit(void) {    
  printk(KERN_INFO "Goodbye, World!\n");    
  kfree(buffer);
  kfree(producer_threads);

  for (int i = 0; i < prod; i++) { 
    kthread_stop(producer_threads[i]);
  }

  //stop consumer threads
  for (int i = 0; i < cons; i++) { 
    kthread_stop(consumer_threads[i]);
  }

  kfree(producer_threads);
  kfree(consumer_threads);
}    




module_init(pc_init);    
module_exit(pc_exit);
