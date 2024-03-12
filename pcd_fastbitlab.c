#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/err.h>
//#include<errno.h>
#include<linux/device.h>
#include<linux/cdev.h>
//#include <linux/kdev_t.h>
#include<linux/slab.h>
#include<linux/uaccess.h>


#define DEV_MEM_SIZE    512

dev_t dev = 0;
static struct class *dev_class;
static struct cdev pcd_cdev;
uint8_t *Dev_kernel_buffer;

static int __init pcd_init(void);
static void __exit pcd_exit(void);
static int pcd_open(struct inode *inode, struct file *file);
static int pcd_release(struct inode *inode, struct file *file);
static ssize_t pcd_read(struct file *filp, char __user *buf, size_t len, loff_t *f_ops);
static ssize_t pcd_write(struct file *filp, const char __user *buf, size_t len, loff_t *f_ops);
static loff_t pcd_lseek(struct file *filp, loff_t offset, int whence);


static struct file_operations fops =
{
	.owner   = THIS_MODULE,
	.read    = pcd_read,
	.write   = pcd_write,
	.open    = pcd_open,
	.release = pcd_release,
	.llseek  = pcd_lseek
};

static int __init pcd_init(void)
{
	if((alloc_chrdev_region(&dev,0, 1, "Pcd_device_drv")) < 0)
	{
		pr_info("Cannot allocate major number for device 1\n");
		return -1;
	}

	pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

	cdev_init(&pcd_cdev, &fops);


	if((cdev_add(&pcd_cdev, dev, 1)) < 0)
	{
		pr_err("Cannot add the device to the system\n");
		goto r_class;
        }


	
	dev_class = class_create(THIS_MODULE, "pcd_device_calss");
	if(IS_ERR(dev_class))
	{
		pr_err("Cannot create the struct class for device\n");
		goto r_class;
        }

	if(IS_ERR(device_create(dev_class, NULL, dev, NULL, "Pcd_device")))
	{
		pr_err("Cannot create the Device\n");
		goto r_device;
        }


	if((Dev_kernel_buffer = kmalloc(DEV_MEM_SIZE, GFP_KERNEL)) == 0)
	{
		pr_info("Cannot allocate memory in kernel\n");
		goto r_device;
        }

	pr_info("Kernel Module Inserted Successfully...\n");

	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(dev, 1);
	return -1;
	
}

static int pcd_open(struct inode *inode, struct file *file)
{
	pr_info("Driver Open Function Called...!!!\n");
        return 0;
}
static int pcd_release(struct inode *inode, struct file *file)
{
	pr_info("Driver Release Function Called...!!!\n");
        return 0;
}
static ssize_t pcd_read(struct file *filp, char __user *buf, size_t len, loff_t *f_ops)
{
	pr_info("Read requested for %zu bytes \n",len);
	pr_info("Current file position = %lld\n",*f_ops);
	
	if((*f_ops + len) > DEV_MEM_SIZE)
	{
		len = DEV_MEM_SIZE - *f_ops;
	}

	if(copy_to_user(buf, &Dev_kernel_buffer[*f_ops], len))
	{
		pr_err("Data read error!!\n");
		return -EFAULT;
	}

	*f_ops += len;
	
	pr_info("Number of bytes successfully read = %zu\n",len);
	pr_info("Updated file position = %lld\n",*f_ops);

	return len;
}


static ssize_t pcd_write(struct file *filp, const char __user *buf, size_t len, loff_t *f_ops)
{
	pr_info("Write requested for %zu bytes\n",len);
	pr_info("Current file position = %lld\n",*f_ops);

	if((*f_ops + len) > DEV_MEM_SIZE)
	{
		len = DEV_MEM_SIZE - *f_ops;
	}

	if(!len)
	{
		pr_err("No space left on the device \n");
		return -ENOMEM;
	}

	if(copy_from_user(&Dev_kernel_buffer[*f_ops], buf, len))
	{
		return -EFAULT;
	}

	*f_ops += len;

	pr_info("Number of bytes successfully written = %zu\n",len);
	pr_info("Updated file position = %lld\n",*f_ops);

	/*Return number of bytes which have been successfully written */
	return len;
	
}


static loff_t pcd_lseek(struct file *filp, loff_t offset, int whence)
{
	pr_info("lseek requested \n");
	pr_info("Current file position = %lld\n",filp->f_pos);

	loff_t temp;

	switch(whence)
	{
	case SEEK_SET:
		if(offset > DEV_MEM_SIZE || (offset < 0))
			return -EINVAL;
		filp->f_pos = offset;
		break;
		
	case SEEK_CUR:
		temp = filp->f_pos + offset;
		if(temp > DEV_MEM_SIZE || (temp < 0))
			return -EINVAL;
		filp->f_pos = temp;
		break;
		
	case SEEK_END:
		temp = DEV_MEM_SIZE + offset;
		if(temp > DEV_MEM_SIZE || (temp < 0))
			return -EINVAL;
		filp->f_pos = temp;
		break;
		
	default:
		return -EINVAL;
	}
	pr_info("Updated file position = %lld\n",filp->f_pos);
	return filp->f_pos;
}


static void __exit pcd_exit(void)
{
	kfree(Dev_kernel_buffer);
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("Kernel Module Removed Successfully...\n");
}

module_init(pcd_init);
module_exit(pcd_exit);

MODULE_LICENSE("GPL");
