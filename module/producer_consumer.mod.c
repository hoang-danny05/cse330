#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x0001e22b, "down_interruptible" },
	{ 0xe8213e80, "_printk" },
	{ 0xe97c47f7, "up" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xd710adbf, "__kmalloc_noprof" },
	{ 0xbd03ed67, "random_kmalloc_seed" },
	{ 0xc2fefbb5, "kmalloc_caches" },
	{ 0x38395bf3, "__kmalloc_cache_noprof" },
	{ 0xb53b9c09, "kthread_create_on_node" },
	{ 0x32ba21a0, "wake_up_process" },
	{ 0x1b3db703, "param_ops_uint" },
	{ 0xd272d446, "__fentry__" },
	{ 0x43f4e0dd, "init_task" },
	{ 0x5e505530, "kthread_should_stop" },
	{ 0x70eca2ca, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x0001e22b,
	0xe8213e80,
	0xe97c47f7,
	0xcb8b6ec6,
	0xd272d446,
	0xd710adbf,
	0xbd03ed67,
	0xc2fefbb5,
	0x38395bf3,
	0xb53b9c09,
	0x32ba21a0,
	0x1b3db703,
	0xd272d446,
	0x43f4e0dd,
	0x5e505530,
	0x70eca2ca,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"down_interruptible\0"
	"_printk\0"
	"up\0"
	"kfree\0"
	"__x86_return_thunk\0"
	"__kmalloc_noprof\0"
	"random_kmalloc_seed\0"
	"kmalloc_caches\0"
	"__kmalloc_cache_noprof\0"
	"kthread_create_on_node\0"
	"wake_up_process\0"
	"param_ops_uint\0"
	"__fentry__\0"
	"init_task\0"
	"kthread_should_stop\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "D4F31910E472B6A997EEC73");
