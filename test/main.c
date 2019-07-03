#include <stdio.h>

void modify_value(int array[3])
{
	//printf("size:%d\n",sizeof(array));
	array[0]=997;
	array[1]=998;
	array[2]=999;
}

int main(int argc, char **argv)
{
	int array[3] = {7,8,9};

	printf("before:[0]=%d,[1]=%d,[2]=%d\n",array[0],array[1],array[2]);
	modify_value(array);
	printf("after:[0]=%d,[1]=%d,[2]=%d\n",array[0],array[1],array[2]);

	return 0;
}
