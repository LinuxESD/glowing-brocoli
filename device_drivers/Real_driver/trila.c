#include<stdio.h>
#include<stdlib.h>
int main()
{
	char a[10];
	printf("enter string\n");
	scanf("%[1-9^\n]s",a);
	printf("the string is\n");
	printf("%s",a);
}
