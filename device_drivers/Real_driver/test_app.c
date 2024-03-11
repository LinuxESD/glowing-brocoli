#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int8_t write_buf[512];
int8_t read_buf[512];
int read_out;
int write_out;
int seek_out;
int main()
{
	int fd;
	char option;
	printf("********************\n");
	printf("********Test_app********\n");
	fd=open("/dev/etx_device",O_RDWR);
	if(fd<0)
	{
		printf("Cannot open device file...\n");
		return 0;
	}
	//Performing file operations
	while(1)
	{
		printf("*****Enter option*****\n");
		printf("     1.Write     \n");
		printf("     2.Read     \n");
		printf("     3.Exit     \n");
		printf("********************\n");
		scanf("\n%c",&option);
		printf("Your option=%c\n",option);
		switch(option)
		{
		case '1':
			lseek(fd,0,0);
			printf("Enter string into driver\n");
			scanf("\n%[^\n]s",write_buf);
			printf("Data writing...\n");
			//write(fd, write_buf, strlen(write_buf));
			printf("%ld is writeout\n",write(fd,write_buf,strlen(write_buf)));
                        printf("Done!\n");
			break;
		case '2':
			printf("Data Reading ...");
			lseek(fd,0,0);
                        printf("%ld is readout\n",read(fd, read_buf,512));
                        printf("Done!\n\n");
                        printf("Data = %s\n\n", read_buf);
			//changes in the code
			printf("%ld\n",strlen(read_buf));
			//int i;
			printf("\n\nData2=%s\n",read_buf);
			break;
		case '3':
			close(fd);
                        exit(1);
			break;
		 default:
                        printf("Enter Valid option = %c\n",option);
                        break;
			
		}
	}
	close(fd);
}
