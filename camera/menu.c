#include "menu.h"
#include <stdio.h>

static void format_printf(char *s)
{
	printf("%s", s);
}

void show_menu(void)
{
	format_printf("=======menu======\n");
	format_printf("-d [int],device(0,1,2...)\n");
	format_printf("-w [int],width(320,640...)\n");
	format_printf("-h [int],height(320,640...)\n");
	format_printf("-f [string],format(YUYV,MJPG...)\n");
	format_printf("-p [int],fps(0-30)\n");
	format_printf("-s [int],save image\n");
	format_printf("=================\n");
}
