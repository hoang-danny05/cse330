#include <linux/init.h>    
#include <linux/module.h>    
#include <linux/kernel.h>    

MODULE_LICENSE("GPL");    
MODULE_AUTHOR("Danny Hoang");    

static int intParameter = 2025;
module_param(intParameter, int, 0);

static char* charParameter = "Fall";
module_param(charParameter, charp, 0);

static int __init example_init(void) {    
    printk(KERN_INFO "Hello, I am Danny Hoang, a student of CSE330 %s %d.", charParameter, intParameter);    
    return 0;    
}    

static void __exit example_exit(void) {    
    printk(KERN_INFO "Goodbye, World!\n");    
}    

module_init(example_init);    
module_exit(example_exit);

