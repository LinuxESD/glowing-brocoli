#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/hw_irq.h>
#include <linux/err.h>

#define IRQ_NO 11

void tasklet1_fn(unsigned long);
void tasklet2_fn(unsigned long);

/* Init the Tasklet by dynamic Method */
struct tasklet_struct *tasklet1 = NULL;
struct tasklet_struct *tasklet2 = NULL;

/*Tasklet Function*/
void tasklet1_fn(unsigned long arg)
{
	int i;
	for(i=0; i<5; i++)
		pr_info("Executing Tasklet 1 Function : arg = %ld\n", arg);
}

void tasklet2_fn(unsigned long arg)
{
	int i;
	for(i=0; i<5; i++)
		    pr_info("Executing Tasklet 2 Function : arg = %ld\n", arg);
}


//Interrupt handler for IRQ 11.
irqreturn_t irq_handler(int irq, void *dev_id)
{
	pr_info("Shared IRQ: Interrupt Occurred");
        /*Scheduling Task to Tasklet*/
	tasklet_schedule(tasklet1);
	tasklet_hi_schedule(tasklet2);

	return IRQ_HANDLED;
}

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
 
/*************** Driver functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);
/*
** File operation sturcture
*/
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
	struct irq_desc *desc;
        pr_info("Read function\n");

	/* Triggering Interrupt */
	desc = irq_to_desc(11);
        if (!desc) 
        {
            return -EINVAL;
        }
        __this_cpu_write(vector_irq[59], desc);
        asm("int $0x3B");  // Corresponding to irq 11

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
            pr_info("Cannot create the Device 1\n");
            goto r_device;
        }

	if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "etx_device", (void *)(irq_handler))) {
            pr_info("my_device: cannot register IRQ ");
                    goto irq;
        }

	/* Init the tasklet bt Dynamic Method */
	tasklet1 = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
	/*if(tasklet1 == NULL) {
            pr_info("etx_device: cannot allocate Memory");
            goto irq;
        }
	*/
	tasklet2 = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
	/*if(tasklet2 == NULL) {
            pr_info("etx_device: cannot allocate Memory");
            goto irq;
        }
	*/
	tasklet_init(tasklet1, tasklet1_fn, 1);
	tasklet_init(tasklet2, tasklet2_fn, 2);
 
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 
irq:
        free_irq(IRQ_NO,(void *)(irq_handler));
 
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
        /*Kill the Tasklet */
	tasklet_kill(tasklet1);
	tasklet_kill(tasklet2);

	kfree(tasklet1);
	kfree(tasklet2);
	
	free_irq(IRQ_NO,(void *)(irq_handler));
	device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
