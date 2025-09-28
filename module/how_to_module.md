# how to implement kernel module

1) include kernel modules 

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
```

2) use entry and exit functions

```c
static int __init example_init(void) {
  printk(KERN_INFO "Hello, World!\n");
}

module_init(example_init);
```



