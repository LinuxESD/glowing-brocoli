#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int8_t write_buf[512];
int8_t read_buf[512];

int main()
{
        int fd,i;
        char option;
        printf("*********************************\n");
        printf("*******Welcome*******\n");

        fd = open("/dev/Pcd_device", O_RDWR);
	//printf("%d\n",fd);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }

        while(1) {
                printf("****Please Enter the Option******\n");
                printf("        1. Write               \n");
                printf("        2. Read                 \n");
                printf("        3. Exit                 \n");
                printf("*********************************\n");
                scanf("\n%c", &option);
                printf("Your Option = %c\n", option);
                
                switch(option) {
                        case '1':
				lseek(fd, 0,0);
                                printf("Enter the string to write into driver : ");
				scanf("\n%[^\n]s", write_buf);
			     
                                printf("Data Writing ...");
                                write(fd, write_buf, strlen(write_buf)+1);
                                printf("Done!\n");
                                break;
                        case '2':
				i=0;
				//printf("Data1 = %s", read_buf);
				lseek(fd, 0,0);
                                printf("Data Reading ...");
                                read(fd, read_buf, 512);
                                printf("Done!\n\n");
				printf("%ld\n", strlen(read_buf));
                                printf("\n\nData2 = %s\n", read_buf);
				/*	while(i <=512 )
				{
					printf("%c", read_buf[i]);
					i++;
					}*/
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
