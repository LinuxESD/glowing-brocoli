#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
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

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x32e21920, "module_layout" },
	{ 0x952664c5, "do_exit" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x45937b6f, "wake_up_process" },
	{ 0x2da00799, "kthread_create_on_node" },
	{ 0x748154da, "class_destroy" },
	{ 0x36a97168, "device_create" },
	{ 0xaf919f03, "__class_create" },
	{ 0x6a0a71c7, "cdev_add" },
	{ 0x320c3a3f, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x3eeb2322, "__wake_up" },
	{ 0x92540fbf, "finish_wait" },
	{ 0x1000e51, "schedule" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x800473f, "__cond_resched" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x92997ed8, "_printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "C6A3384954931AE0D2B088E");
