#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct 
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/err.h>

struct mutex my_mutex;

unsigned long global_var = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
 
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

struct task_struct *thread1;
struct task_struct *thread2;


/*************** Driver functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);
 /******************************************************/


int thread_fn1(void *pv);
int thread_fn2(void *pv);

/*
** Thread function 1
*/
int thread_fn1(void *pv)
{
	while(!kthread_should_stop())
	{
		mutex_lock(&my_mutex);
		global_var++;
		pr_info("In EmbeTronicX Thread Function1 %lu\n", global_var);
		mutex_unlock(&my_mutex);
		msleep(1000);
	}

	return 0;
}

/*
** Thread function 2
*/
int thread_fn2(void *pv)
{
	while(!kthread_should_stop())
	{
		mutex_lock(&my_mutex);
		global_var++;
		pr_info("In EmbeTronicX Thread Function2 %lu\n", global_var);
		mutex_unlock(&my_mutex);
		msleep(1000);
	}

	return 0;
}

//File operation structure
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = etx_read,
        .write          = etx_write,
        .open           = etx_open,
        .release        = etx_release,
};

/*
** This function will be called when we open the Device file
*/ 
static int etx_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}

/*
** This function will be called when we close the Device file
*/
static int etx_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}

/*
** This function will be called when we read the Device file
*/ 
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
        pr_info("Read function\n");
 
        return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp, 
                const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Write Function\n");
        return len;
}

/*
** Module Init function
*/
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
            pr_info("Cannot create the Device \n");
            goto r_device;
        }

	mutex_init(&my_mutex);

	/* Creating Thread 1 */
	thread1 = kthread_run(thread_fn1, NULL, "Thread 1");

	if(thread1)
	{
		pr_err("Kthread1 Created Successfully...\n");
	}
	else
	{
		pr_err("Cannot create kthread1\n");
		goto r_device;
        }

	/* Creating Thread 2 */
	thread2 = kthread_run(thread_fn2, NULL, "thread2");
	if(thread2)
	{
		pr_err("Kthread2 Created Successfully...\n");
	}
	else
	{
		pr_err("Cannot create kthread1\n");
		goto r_device;
        }

	pr_info("Device Driver Insert...Done!!!\n");
        return 0;
	
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        cdev_del(&etx_cdev);
        return -1;
}

/*
** Module exit function
*/ 
static void __exit etx_driver_exit(void)
{
	kthread_stop(thread1);
	kthread_stop(thread2);
	device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
