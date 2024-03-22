#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/moduleparam.h>

int value, arr_value[4];
char * name;
int cb_value = 0;

module_param(value, int, S_IRUSR|S_IWUSR);
module_param(name, charp, S_IRUSR|S_IWUSR);
module_param_array(arr_value, int, NULL, S_IRUSR|S_IWUSR);

static int __init mod_init(void)
{
	int i;
	pr_info("Value = %d \n", value);
	pr_info("cb_value = %d\n", cb_value);
	pr_info("Name = %s\n", name);

	for(i = 0; i<(sizeof(arr_value)/sizeof(int)); i++)
	{
	  pr_info("Arr_value[%d] = %d\n", i, arr_value[i]);
	}

	printk(KERN_ALERT "Kernel Module Inserted Successfully...\n");

	return 0;
}

static void __exit mod_exit(void)
{
	pr_info("Kernel Module Removed Successfully...\n");
}

int notify_param(const char *val, const struct kernel_param *kp)
{
	int res = param_set_int(val, kp);
	if(res==0)
	{
		pr_info("Call back function called...\n");
                pr_info("New value of cb_value = %d\n", cb_value);
                return 0;
        }
        return -1;
}


const struct kernel_param_ops my_param_ops =
{
	.set = &notify_param,
	.get = &param_get_int
};

module_param_cb(cb_value, &my_param_ops, &cb_value, S_IRUGO|S_IWUSR );


module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
