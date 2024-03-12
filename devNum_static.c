#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include <linux/fs.h>


dev_t dev = MKDEV(255,0);

static int __init mod_init(void)
{
	register_chrdev_region(dev, 1, "My_device");
	pr_info("My_device: Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));
	pr_info("Kernel Module Inserted Successfully...\n");

	return 0;
}


static void __exit mod_exit(void)
{
	unregister_chrdev_region(dev, 1);
	pr_info("Kernel Module Removed Successfully...\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
