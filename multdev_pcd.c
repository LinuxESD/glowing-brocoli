#include<linux/module.h>
#include<linux/fs.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/device.h>
//#include<linux/slab.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>

#define NO_OF_DEVICES      4

#define MEM_SIZE_PCDEV1    1024
#define MEM_SIZE_PCDEV2    512
#define MEM_SIZE_PCDEV3    1024
#define MEM_SIZE_PCDEV4    512

/* pseudo device's memory */
char device_buffer_pcdev1[MEM_SIZE_PCDEV1];
char device_buffer_pcdev2[MEM_SIZE_PCDEV2];
char device_buffer_pcdev3[MEM_SIZE_PCDEV3];
char device_buffer_pcdev4[MEM_SIZE_PCDEV4];

/*Device private data structure */

struct pcdev_private_data
{
	char *buffer;
	unsigned size;
	const char *serial_number;
	int perm;
	struct cdev cdev;
};


struct pcdrv_private_data
{
	int total_devices;
	dev_t device_number;
	struct class *class_pcd;
	struct device *device_pcd;
	struct pcdev_private_data pcdev_data[NO_OF_DEVICES];
};

struct pcdrv_private_data pcdrv_data =
{
	.total_devices = NO_OF_DEVICES,
	.pcdev_data =
	{
		[0] = {
			.buffer        = device_buffer_pcdev1,
			.size          = MEM_SIZE_PCDEV1,
			.serial_number = "PCDEV1XYZ123",
			.perm          = O_RDONLY
		},

		[1] = {
			.buffer        = device_buffer_pcdev2,
			.size          = MEM_SIZE_PCDEV2,
			.serial_number = "PCDEV2XYZ123",
			.perm          = O_WRONLY
		},

		[2] = {
			.buffer        = device_buffer_pcdev3,
			.size          = MEM_SIZE_PCDEV3,
			.serial_number = "PCDEV3XYZ123",
			.perm          = O_RDWR
		},

		[3] = {
			.buffer        = device_buffer_pcdev4,
			.size          = MEM_SIZE_PCDEV4,
			.serial_number = "PCDEV4XYZ123",
			.perm          = O_RDWR
		}
	}
};

static int __init pcd_driver_init(void)
{
	int ret;
	int i,temp;

	/* Dynamically allocate device number*/
	ret = alloc_chrdev_region(&pcdrv_data.device_number, 0, NO_OF_DEVICES, "pcdevs");
	
	if(ret<0)
	{
		pr_err("Alloc chrdev failed!!\n");
		goto out;
	}

	/*create device class under /sys/class/ */

	pcdrv_data.class_pcd = class_create(THIS_MODULE, "pcd_class");
	if(IS_ERR(pcdrv_data.class_pcd))
	{
		pr_err("Class creation failed!!\n");
		ret = PTR_ERR(pcdrv_data.class_pcd);
		goto unreg_chrdev;
	}

	for(i=0; i<NO_OF_DEVICES; i++)
	{
		pr_info("Device number <major>:<minor> = %d:%d\n",MAJOR(pcdrv_data.device_number+i),MINOR(pcdrv_data.device_number+i));

	/*Initialize the cdev structure with fops*/
		cdev_init(&pcdrv_data.pcdev_data[i].cdev, &pcd_fops);

		/* Register a device (cdev structure) with VFS */
		pcdrv_data.pcdev_data[i].cdev.owner = THIS_MODULE;
		ret = cdev_add(&pcdrv_data.pcdev_data[i].cdev, pcdrv_data.device_number+i, 1);

		if(ret < 0)
		{
			pr_err("Cdev add failed!!\n");
			goto cdev_del;
		}

		pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, NULL, pcdrv_data.device_number+i, NULL, "pcdev_%d",i+1);
		if(IS_ERR(pcdrv_data.device_pcd))
		{
			pr_err("Device create failed!!\n");
			ret = PTR_ERR(pcdrv_data.device_pcd);
			goto class_del;
		}
	}

	pr_info("Module init was successful!!!\n");

	return 0;

class_del:
	temp = i;
	for(; temp>=0; temp--)
		device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number+temp);

	class_destroy(pcdrv_data.class_pcd);

cdev_del:
	temp = i;
	for(; temp>=0; temp--)
		cdev_del(&pcdrv_data.pcdev_data[temp].cdev);

unreg_chrdev:
	unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);

out:
	pr_info("Module insertion failed\n");
        return ret;
}
		
