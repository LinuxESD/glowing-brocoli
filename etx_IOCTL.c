#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
//#include<asm/uaccess.h>

#define MAGIC_NUM 'L'

#define WR_VALUE _IOW(MAGIC_NUM, 1, int32_t*)
#define RD_VALUE _IOR(MAGIC_NUM, 2, int32_t*)

int32_t value = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_cdev;


/*Function Prototypes*/
static int        __init mod_init(void);
static void       __exit mod_exit(void);
static int        my_open(struct inode *inode, struct file *file);
static int        my_release(struct inode *inode, struct file *file);
static ssize_t    my_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t    my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static long       my_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations fops =
{
	.owner   = THIS_MODULE,
	.read    = my_read,
	.write   = my_write,
	.open    = my_open,
	.release = my_release,
	.unlocked_ioctl = my_ioctl
	
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
        return 0;
}

static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	pr_info("Driver Write Function Called...!!!\n");
        return len;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	int retval = 0;

	if( _IOC_TYPE(cmd) != MAGIC_NUM)
		return -ENOTTY;

	if( _IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok((void *)arg, _IOC_SIZE(cmd));
	else if( _IOC_TYPE(cmd) & _IOC_WRITE)
		err = !access_ok((void *)arg, _IOC_SIZE(cmd));
	if(err)
		return -EFAULT;
	
	switch(cmd)
	{
		
	case WR_VALUE:
		/*if( copy_from_user(&value ,(int32_t*) arg, sizeof(value)) )
                {
			pr_err("Data Write : Err!\n");
		}*/
	        retval = __get_user(value, (int32_t *)arg);
		pr_info("Value = %d\n", value);
		break;
		
	case RD_VALUE:
		/*if( copy_to_user((int32_t*) arg, &value, sizeof(value)) )
                {
			pr_err("Data Read : Err!\n");
		}*/
		retval = __put_user(value, (int32_t *)arg);
		break;
		
	default:
		pr_info("Default\n");
		return -ENOTTY;
		break;
	}

	return 0;
	
}

static void __exit mod_exit(void)
{
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("Kernel Module Removed Successfully...\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
