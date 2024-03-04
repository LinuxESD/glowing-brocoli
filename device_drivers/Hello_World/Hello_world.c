#include<linux/module.h> //contains symbols and function definitions
#include<linux/init.h> //used to specify your initialization and cleanup functions
static int hello_init(void)   //__init can be added before hello_init to make sure that this is called at initialization only
{
	printk(KERN_ALERT "Hello, world\n");
	//printk(KERN_ALERT "%d\n",a);
	return 0;
}

static void hello_exit(void) //__exit an be added before hello_exit to make sure that this is called at exit only
{
	printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");

//MODULE_LICENSE is must without that the following error comes when running make command

/*ERROR: modpost: missing MODULE_LICENSE() in /home/vlab/git_comp/device_drivers/Hello_World/Hello_world.o
*/
