#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include <linux/fs.h>


dev_t dev = 0;

static int __init mod_init(void)
{
	if((alloc_chrdev_region(&dev,0, 1, "My_device")) < 0)
	{
		pr_info("Cannot allocate major number for device 1\n");
		return -1;
	}

	pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
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
