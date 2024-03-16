#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
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
__used __section(__versions) = {
	{ 0x736302c7, "module_layout" },
	{ 0xe4fccc3e, "device_destroy" },
	{ 0x1bf3f2b8, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x1961fca2, "class_destroy" },
	{ 0xff863ace, "sysfs_remove_file_ns" },
	{ 0xe7750f8, "kobject_put" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0x732a0532, "sysfs_create_file_ns" },
	{ 0x20b41469, "kobject_create_and_add" },
	{ 0x659ade75, "kernel_kobj" },
	{ 0xaa48f673, "device_create" },
	{ 0x314c3f16, "__class_create" },
	{ 0xcb66baf7, "cdev_add" },
	{ 0xf888cf1f, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x83a7687c, "vector_irq" },
	{ 0xa7fcda0d, "irq_to_desc" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "13F2A97DFCCB5CF21C52413");
