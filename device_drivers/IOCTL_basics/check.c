#include<stdio.h>
struct stu
{
	int a;
	char *b;
};
int main()
{
	struct stu stuff={10,"Srikar"};
	printf("%ld",sizeof(stuff));

}
