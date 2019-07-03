#include <stdio.h>
#include "func.h"
#include <stdbool.h>


int main(int argc, char **argv)
{
	q3();	

	return 0;
}


bool is_leap_year(int year)
{
	if(year%400==0 && year%4==0)
		return true;
	else
		return false;
}

int sum_mon(int mon, bool is_leap_year)
{
	int i;
	int sum=0;

	for(i=1;i<mon;i++)
	{
		switch(i)
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				sum += 31;
				break;
			case 2:
				if(is_leap_year)
					sum += 29;
				else
					sum += 28;
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				sum += 30;
		}
	}

	return sum;
}

void q2()
{
//	题目：输入某年某月某日，判断这一天是这一年的第几天？
	int year = 2019;
	int mon = 12;
	int day =  30;
	int pass_day;

	pass_day = day + sum_mon(mon, is_leap_year(year));

	
	printf("pass day=%d,remain day:%d\n", pass_day, 365 - pass_day + (is_leap_year(year)?1:0));
}

int f(int i)
{
	if(i==1||i==2)
		return 1;
	return f(i-1)+f(i-2);
}

void q3()
{
	//有一对兔子，从出生后第3个月起每个月都生一对兔子，小兔子长到第三个月后每个月又生一对兔子，假如兔子都不死，问每个月的兔子总数为多少？
	//
	//
	//
	int i;
	for(i=1;i<20;i++)
	{
		printf("%d:%d\n",i,f(i));
	}
}
