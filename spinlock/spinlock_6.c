#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct 
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/hw_irq.h>

struct irq_desc *desc;
//DEFINE_SPINLOCK(my_spinlock);

#define IRQ_NO 11
#define IRQ_NO1 12

spinlock_t my_spinlock;

unsigned long etx_global_variable = 0;

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
 /******************************************************/


//Interrupt handler for IRQ 11. 
static irqreturn_t irq_handler11(int irq,void *dev_id)
{
        printk(KERN_INFO "Shared IRQ 11: Interrupt Occurred");
	spin_lock_irq(&my_spinlock); 
        etx_global_variable++;
	
        printk(KERN_INFO "Executing ISR Function 11 : %lu\n", etx_global_variable);
        spin_unlock_irq(&my_spinlock); 
        
        return IRQ_HANDLED;
}

//Interrupt handler for IRQ 12. 
static irqreturn_t irq_handler12(int irq,void *dev_id)
{
        printk(KERN_INFO "Shared IRQ 12: Interrupt Occurred");
	spin_lock_irq(&my_spinlock); 
        etx_global_variable++;
        printk(KERN_INFO "Executing ISR Function 12: %lu\n", etx_global_variable);
        spin_unlock_irq(&my_spinlock);
        
        return IRQ_HANDLED;
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
	//struct irq_desc *desc;
        pr_info("Read function\n");
	desc = irq_to_desc(11);
        if (!desc) 
        {
            return -EINVAL;
        }
        __this_cpu_write(vector_irq[59], desc);
	//spin_lock_irq(&my_spinlock);
	asm("int $0x3B");  // Corresponding to irq 11
	//pr_info("before release of lock\n");
	//spin_unlock_irq(&my_spinlock);
	//pr_info("After release of lock\n");
	desc = irq_to_desc(12);
        if (!desc) 
        {
            return -EINVAL;
        }
        __this_cpu_write(vector_irq[60], desc);
	asm("int $0x3C");
	
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
 
	spin_lock_init(&my_spinlock);
 
        if (request_irq(IRQ_NO, irq_handler11, IRQF_SHARED, "etx_device11", (void *)(irq_handler11))) {
            printk(KERN_INFO "my_device: cannot register IRQ ");
                    goto irq;
        }

	if (request_irq(IRQ_NO1, irq_handler12, IRQF_SHARED, "etx_device12", (void *)(irq_handler12))) {
            printk(KERN_INFO "my_device: cannot register IRQ ");
                    goto irq;
        }
        
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 
 irq:
        free_irq(IRQ_NO,(void *)(irq_handler11));
	free_irq(IRQ_NO,(void *)(irq_handler12));
 
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
	//tasklet_kill(&tasklet);
	free_irq(IRQ_NO,(void *)(irq_handler11));
	free_irq(IRQ_NO1,(void *)(irq_handler12));
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
