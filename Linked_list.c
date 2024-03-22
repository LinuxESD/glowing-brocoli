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
#include<asm/hw_irq.h>
#include <linux/workqueue.h>            // Required for workqueues
#include <linux/err.h>

#define IRQ_NO 11
 
volatile int etx_value = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

struct workqueue_struct *own_workqueue;

void workqueue_fn(struct work_struct *work);

static DECLARE_WORK(work, workqueue_fn);

/*Linked List Node*/
struct my_list
{
	struct list_head list;
	int data;
};

/*Declare and init the head node of the linked list*/
LIST_HEAD(Head_Node);

/*
** Function Prototypes
*/ 
/*************** Driver functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);


/*Workqueue Function*/

void workqueue_fn(struct work_struct *work)
{
	struct my_list *temp_node = NULL;

	pr_info("Executing Workqueue Function\n");

	/*Creating Node*/
	temp_node = kmalloc(sizeof(struct my_list), GFP_KERNEL);

	/*Assgin the data that is received*/
	temp_node->data = etx_value;

	/*Init the list within the struct*/
	INIT_LIST_HEAD(&temp_node->list);

	/*Add Node to Linked List*/
	list_add_tail(&temp_node->list, &Head_Node);

}

//Interrupt handler for IRQ 11.

irqreturn_t irq_handler(int irq, void *dev_id)
{
	pr_info("Shared IRQ: Interrupt Occurred\n");
        /*Allocating work to queue*/
	queue_work(own_workqueue, &work);

	return IRQ_HANDLED;
}

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
	struct my_list *temp;
	int count = 0;
	pr_info("Read function\n");

	/*Traversing Linked List and Print its Members*/
	list_for_each_entry(temp, &Head_Node, list)
	{
		pr_info("Node %d data = %d\n", count++, temp->data);
	}

	pr_info("Total Nodes = %d\n", count);
        return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp, 
                const char __user *buf, size_t len, loff_t *off)
{
	struct irq_desc *desc;
	
        pr_info("Write Function\n");
        /*Copying data from user space*/

	sscanf(buf, "%d", &etx_value);
	
	/* Triggering Interrupt */
	desc = irq_to_desc(11);
        if (!desc) 
        {
            return -EINVAL;
        }
        __this_cpu_write(vector_irq[59], desc);
        asm("int $0x3B");  // Corresponding to irq 11

	return len;
}
 
/*
** Module Init function
*/  
static int __init etx_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
                pr_err("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&etx_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&etx_cdev,dev,1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if(IS_ERR(dev_class = class_create(THIS_MODULE,"etx_class"))){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"etx_device"))){
            pr_err("Cannot create the Device \n");
            goto r_device;
        }
	
	if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "etx_device", (void *)(irq_handler))) {
            pr_err("my_device: cannot register IRQ \n");
            goto irq;
        }
	
	/*Creating workqueue */
        own_workqueue = create_workqueue("own_wq");
        
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
** Module Exit function
*/

static void __exit etx_driver_exit(void)
{
 
        /* Go through the list and free the memory. */
	struct my_list *cursor, *temp;
        list_for_each_entry_safe(cursor, temp, &Head_Node, list)
	{
		list_del(&cursor->list);
		kfree(cursor);
	}
	/* Delete workqueue */
        destroy_workqueue(own_workqueue);
        free_irq(IRQ_NO,(void *)(irq_handler));
	device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
	
