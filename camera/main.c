#include <stdio.h>
#include "camera.h"
#include <stdlib.h>
//#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include "menu.h"


static const char short_options[] = "d:w:h:f:"; 

static const struct option long_options[] = { 
	{ "device", required_argument,		NULL, 'd' },
	{ "width", required_argument,		NULL, 'w' }, 
	{ "height", required_argument, NULL, 'h' }, 
	{ "format", required_argument, NULL, 'f' }, 
	{ "fps", required_argument, NULL, 'p' }, 
	{ 0, 0, 0, 0 } 
};

int parse_argv(int argc, char **argv, struct camera_args *args)
{

	for (;;) 
	{		
		int index;		
		int c; 		

		c = getopt_long(argc, argv, short_options, long_options, &index);
		if (-1 == c)			
			break; 		

		switch (c) 
		{		
			case 0: /* getopt_long() flag */
				break;
			case 'd':
				args->camera_node_id = atoi(optarg);
				break; 		
			case 'p':
				args->priv_fps = atoi(optarg);
				break;
			case 'w':
				args->priv_w = atoi(optarg);
				break;
			case 'h':
				args->priv_h = atoi(optarg);
				break;
			case 'f':
				strcpy(args->priv_format, optarg);
				break;
		//	default:			
				//usage(stderr, argc, argv);				
		}	
	}


	return 0;
}

int main(int argc, char **argv)
{
	int ret;
	int camera_node_id=0;
	struct camera_args args;

	show_menu();

	memset(&args, 0, sizeof(struct camera_args));
	args.priv_w=1280;
	args.priv_h = 720;
	args.priv_h = 30;
	strcpy(args.priv_format, "MJPG");


	ret = parse_argv(argc, argv, &args);
	if(ret < 0)
		return ret;

	ret = test_camera(&args);

	return ret;

}
