#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/err.h>

#define mem_size 1024

dev_t dev =0;

static struct class *dev_class;
static struct cdev etx_cdev;
uint8_t *kernel_buffer;

//my edit
//Function prototypes
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
static int etx_open(struct inode *inode, struct file *filp);
static int etx_release(struct inode *inode, struct file *filp);
static ssize_t  etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static loff_t etx_lseek(struct file *filp,loff_t offset, int whence);
static struct file_operations fops =
{
	.owner=THIS_MODULE,
	.open=etx_open,
	.read=etx_read,
	.write=etx_write,
	.release=etx_release,
	.llseek=etx_lseek
};

static loff_t etx_lseek(struct file *filp, loff_t offset, int whence)
{
	loff_t temp;
	pr_info("lseek requested \n");
	pr_info("Current value of the file position = %lld\n",filp->f_pos);
	switch(whence)
	{
	case SEEK_SET:
		if((offset > mem_size)||(offset <0))
		{
			return -EINVAL;
		}
		filp->f_pos=offset;
		break;
	case SEEK_CUR:
		temp=filp->f_pos+offset;
		if((temp>mem_size)||(temp<0))
		{
			return -EINVAL;
		}
		filp->f_pos=temp;
		break;
	case SEEK_END:
		temp=mem_size+offset;
		if((temp>mem_size)||(temp<0))
		{
			return -EINVAL;
		}
		filp->f_pos=temp;
		break;
	default:
		return -EINVAL;
		
	}
	pr_info("New value of the file pos is=%lld\n",filp->f_pos);
	return filp->f_pos;
}
static int etx_open(struct inode *inode, struct file *filp)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}

static int etx_release(struct inode *inode, struct file *filp)
{
	
        pr_info("Device File Closed...!!!\n");
        return 0;
}


static ssize_t etx_read(struct file *filp,char __user *buf,size_t len, loff_t *off)
{
	pr_info("read requed for %zu bytes \n",len);
	//changes
	pr_info("Current file position = %lld\n",*off);
	//adjust the count
	if((*off + len) > mem_size)
		len=mem_size - *off;
	//copy to user
	if(copy_to_user(buf,&kernel_buffer[*off],len))
	{
		pr_err("Data Read : Err\n");
		return -EFAULT;
	}
	//updating the current file pos
	*off += len;
	//filp->f_pos=*off;
	pr_info("no of bytes sus read = %zu\n",len); //zu is used for size_t type in c
	pr_info("updated file pos =%lld\n",*off);
	//changes
	//return 0;
	return len;
}
		

static ssize_t etx_write(struct file *filp,const char __user *buf,size_t len, loff_t *off)
{
	pr_info("write requed for %zu bytes \n",len);
	pr_info("Current file position = %lld\n",*off);
	//adjust the count
	if((*off + len)>mem_size)
	{
		len=mem_size -*off;
	}
	if(!len)
	{
		pr_err("No space left on the device\n");
		return -ENOMEM;
	}
	//copy from user
	if(copy_from_user(&kernel_buffer[*off],buf,len))
	{
		pr_err("Data write : Err\n");
		return -EFAULT;
	}
	//update the file position offset
	*off+=len;
	//filp->f_pos=*off;
	pr_info("no of bytes sus written = %zu\n",len); //zu is used for size_t type in c
	pr_info("updated file pos =%lld\n",*off);
	return 0;
	//return len;
}

static int __init etx_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
                pr_info("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&etx_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&etx_cdev,dev,1)) < 0){
            pr_info("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if(IS_ERR(dev_class = class_create(THIS_MODULE,"etx_class"))){
            pr_info("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"etx_device"))){
            pr_info("Cannot create the Device 1\n");
            goto r_device;
        }
        
        /*Creating Physical memory*/
        if((kernel_buffer = kmalloc(mem_size , GFP_KERNEL)) == 0){
            pr_info("Cannot allocate memory in kernel\n");
            goto r_device;
        }
        
        //strcpy(kernel_buffer, "Hello_World");
        
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
	//changes
	cdev_del(&etx_cdev);
        unregister_chrdev_region(dev,1);
        return -1;
}

/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
	kfree(kernel_buffer);
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("Simple Linux device driver (Real Linux Device Driver)");
MODULE_VERSION("1.4");
