#include<linux/init.h>       /* Needed for the macros */  
#include<linux/module.h>     /* Needed by all modules */
#include<linux/kernel.h>     /* Needed for KERN_INFO  */

MODULE_LICENSE("GPL");           /*The license type - this affects runtime behavior */
MODULE_AUTHOR("Fossee Team");    /* The author - visible when you use modinfo  */
MODULE_DESCRIPTION("First Linux Kernel Module");  /*The desription - visible when you use modinfo.*/

static int init_func(void)   /* initialization function  */
{
	printk(KERN_INFO "my_module : inside the init function of our module.\n");
	return(0);
}

static void exit_func(void)    /* cleanup function. */
{
	printk(KERN_INFO "my_module : inside the exit function of our module.\n");

}

module_init(init_func);
module_exit(exit_func);


