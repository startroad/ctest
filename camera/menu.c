#include "menu.h"
#include <stdio.h>

static void format_printf(char *s)
{
	printf("%s", s);
}

void show_menu(void)
{
	format_printf("=======menu======\n");
	format_printf("1.test camera");
	format_printf("=================\n");
		
}
