//#include<linux/kernel.h>
//#include<linux/init.h>
#include<linux/module.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/cdev.h>
//#include <linux/kdev_t.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define mem_size    1024

dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_cdev;
uint8_t *kernel_buffer;


/*Function Prototypes*/
static int __init mod_init(void);
static void __exit mod_exit(void);
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);

static struct file_operations fops =
{
	.owner   = THIS_MODULE,
	.read    = my_read,
	.write   = my_write,
	.open    = my_open,
	.release = my_release
};


static int __init mod_init(void)
{
	if((alloc_chrdev_region(&dev,0, 1, "My_device_drv")) < 0)
	{
		pr_info("Cannot allocate major number for device 1\n");
		return -1;
	}

	pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

	cdev_init(&my_cdev, &fops);


	if((cdev_add(&my_cdev, dev, 1)) < 0)
	{
		pr_err("Cannot add the device to the system\n");
		goto r_class;
        }


	
	dev_class = class_create(THIS_MODULE, "MY_device_calss");
	if(IS_ERR(dev_class))
	{
		pr_err("Cannot create the struct class for device\n");
		goto r_class;
        }

	if(IS_ERR(device_create(dev_class, NULL, dev, NULL, "My_device")))
	{
		pr_err("Cannot create the Device\n");
		goto r_device;
        }


	if((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0)
	{
		pr_info("Cannot allocate memory in kernel\n");
		goto r_device;
        }

	strcpy(kernel_buffer, "Hello_World\n");
	
        pr_info("Kernel Module Inserted Successfully...\n");

	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	return -1;
}


static int my_open(struct inode *inode, struct file *file)
{
	pr_info("Driver Open Function Called...!!!\n");
        return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	pr_info("Driver Release Function Called...!!!\n");
        return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	pr_info("Driver Read Function Called...!!!\n");

	if( copy_to_user(buf, kernel_buffer, mem_size))
	{
		pr_err("Data read error!!\n");
	}
	pr_info("Data Read : Done!\n");
        return mem_size;
 
}

static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Driver write Function Called...!!!\n");
        if( copy_from_user(kernel_buffer, buf, len) )
        {
                pr_err("Data Write : Err!\n");
        }
        pr_info("Data Write : Done!\n");
        return len;
}


static void __exit mod_exit(void)
{
	kfree(kernel_buffer);
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("Kernel Module Removed Successfully...\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
