#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/uaccess.h> //copy_from_user , copy_to user
#include<linux/slab.h> //k_malloc
#include<linux/ioctl.h>
#include<linux/err.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int32_t value=0;
dev_t dev=0;
static struct class *dev_class;
static struct cdev etx_cdev;


// In this driver only IOCTL is being implemented, so no other file opeartions are taken by me.

static int __init etx_init(void);
static void __exit  etx_exit(void);
static long etx_ioctl(struct file *filp,unsigned int c_int,unsigned long c_long);
static struct file_operations f_ops =
{
	.owner=THIS_MODULE,
	.unlocked_ioctl=etx_ioctl,
};

static int __init etx_init(void)
{
	//Allocating major number
	if(alloc_chrdev_region(&dev,0,1,"etx_device")<0)
	{
		printk("Cannot allocate major number");
		return -1;
	}
	// alloced major number succesfully
	//create class and then create device for udev // automatic detection
	cdev_init(&etx_cdev,&f_ops);
	if((cdev_add(&etx_cdev,dev,1))<0)
	{
		pr_err("Cannot add the device to the system\n");
		goto r_class;
	}
	if(IS_ERR(dev_class=class_create(THIS_MODULE,"etx_class")))
	{
		pr_err("Cannot add new devices to the system\n");
		goto r_class;
	}
	if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"etx_device")))
	{
		pr_err("Cannot create the device\n");
		goto r_device;
	}
	pr_info("Device driver insert...done!!!\n");
	return 0;
r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev,1);
	return -1;
}

//Module exit function
static void __exit etx_exit(void)
{
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&etx_cdev);
	unregister_chrdev_region(dev,1);
	pr_info("Device Driver remove..Done\n");
}

static long etx_ioctl(struct file *filp,unsigned int c_int,unsigned long c_long )
{
	switch(c_int)
	{
	case WR_VALUE:
		if(copy_from_user(&value,(int32_t*)c_long,sizeof(value)))
		{
			pr_err("Data Write : Err\n");
		}
		pr_info("Value = %d\n",value);
		break;   
	case RD_VALUE:
		if(copy_to_user((int32_t*)c_long,&value,sizeof(value)))
		{
			pr_err("Data Read : Err\n");			
		}
		pr_info("Data sent\n");
		break;
	}
	return 0;	
}
module_init(etx_init);
module_exit(etx_exit);
MODULE_LICENSE("GPL");
