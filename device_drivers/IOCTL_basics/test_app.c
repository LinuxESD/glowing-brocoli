#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>

//define the IOCTL command macros
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int main()
{
	int fd;
	int32_t value,number;

	printf("\nOpening driver\n");
	fd=open("/dev/etx_device",O_RDWR);
	if(fd<0)
	{
		printf("Cannot open device file\n");
		return 0;
	}	
	//writing the number to driver
	
	printf("Enter the value to send\n");
	scanf("%d",&number);
	printf("Writinf value to driver\n");
	ioctl(fd,WR_VALUE,(int32_t*) &number);

	//reading the number from driver
	
	printf("Reading value from driver\n");
	ioctl(fd,RD_VALUE,(int32_t*) &value);
	printf("The value is %d\n",value);

	//close the driver
	
	printf("closing driver\n");
	close(fd);
}

