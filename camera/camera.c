#include "camera.h"
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stropts.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

struct buffer {

	void * start;
	size_t length;

};

int camera_querycap(int fd)
{
	struct v4l2_capability cap;

	ioctl(fd, VIDIOC_QUERYCAP, &cap);
	printf("DriverName:%s\nCard Name:%s\nBus info:%s\nDriverVersion:%u.%u.%u\n\n",
			cap.driver,
			cap.card,
			cap.bus_info,
			(cap.version>>16)&0xFF,
			(cap.version>>8)&0xFF,
			cap.version&0xFF);

	printf("capabilities:%x,device_caps:%x\n", cap.capabilities, cap.device_caps);

	return (cap.device_caps&V4L2_CAP_VIDEO_CAPTURE);
}

int camera_enum_fmt(int fd)
{
	struct v4l2_fmtdesc fmtdesc;
	
	fmtdesc.index=0;
	fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	printf("Supportformat:\n");
	while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
        {
		printf("\t%d.pixelformat:%c%c%c%c,description%s\n", fmtdesc.index+1, 
				fmtdesc.pixelformat&0xff, (fmtdesc.pixelformat>>8)&0xff, (fmtdesc.pixelformat>>16)&0xff, (fmtdesc.pixelformat>>24)&0xff,
				fmtdesc.description);
		fmtdesc.index++;
	}

	return 0;
}

int camera_get_fmt(int fd)
{
	struct v4l2_format fmt;

        fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(fd,VIDIOC_G_FMT,&fmt);

        printf("Current data format information:\n\twidth:%d\n\theight:%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height);
#if 0
        __u32                   width;
        __u32                   height;
        __u32                   pixelformat;
        __u32                   field;          /* enum v4l2_field */
        __u32                   bytesperline;   /* for padding, zero if unused */
        __u32                   sizeimage;
        __u32                   colorspace;     /* enum v4l2_colorspace */
        __u32                   priv;           /* private data, depends on pixelformat */
        __u32                   flags;          /* format flags (V4L2_PIX_FMT_FLAG_*) */

#endif
	printf("\tpixelformat:%x,%c%c%c%c\tfield:%x\tbytesperlinet:%x\tsizeimage:%d\tcolorspace:%x\tpriv:%x\tflags:%x\n",
			fmt.fmt.pix.pixelformat,
			fmt.fmt.pix.pixelformat&0xff, (fmt.fmt.pix.pixelformat>>8)&0xff, (fmt.fmt.pix.pixelformat>>16)&0xff, (fmt.fmt.pix.pixelformat>>24)&0xff,
			fmt.fmt.pix.field,
			fmt.fmt.pix.bytesperline,
			fmt.fmt.pix.sizeimage,
			fmt.fmt.pix.colorspace,
			fmt.fmt.pix.priv,
			fmt.fmt.pix.flags);

	return 0;
}

int camera_set_fmt(int fd, int w, int h, char *format)
{
	struct v4l2_format fmt;

        fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(fd,VIDIOC_G_FMT,&fmt);

	fmt.fmt.pix.width = w;
	fmt.fmt.pix.height = h;
	fmt.fmt.pix.pixelformat = v4l2_fourcc(format[0], format[1], format[2], format[3]);

	printf("will set pixelformat:%x,%c%c%c%c.",
			fmt.fmt.pix.pixelformat,fmt.fmt.pix.pixelformat&0xff, (fmt.fmt.pix.pixelformat>>8)&0xff, (fmt.fmt.pix.pixelformat>>16)&0xff, (fmt.fmt.pix.pixelformat>>24)&0xff);

	if(ioctl(fd,VIDIOC_S_FMT,&fmt)<0)
	{
		printf("try set fmt error\n");
	}else
		printf("try set fmt success\n");

	return 0;
}

int camera_crop(int fd)
{
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;

	cropcap.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	cropcap.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;

	ioctl(fd, VIDIOC_CROPCAP, &cropcap);

	printf("crop:w:%d,h:%d,l:%d,t:%d\n",cropcap.bounds.width,cropcap.bounds.height,cropcap.bounds.left,cropcap.bounds.top);
	printf("crop:w:%d,h:%d,l:%d,t:%d\n",cropcap.defrect.width,cropcap.defrect.height,cropcap.defrect.left,cropcap.defrect.top);

	ioctl(fd, VIDIOC_G_CROP, &crop);
	printf("crop:w:%d,h:%d,l:%d,t:%d\n",crop.c.width,crop.c.height,crop.c.left,crop.c.top);

	//crop.c.width=500;
	//crop.c.height=400;

	ioctl(fd, VIDIOC_S_CROP, &crop);

	ioctl(fd, VIDIOC_G_CROP, &crop);
        printf("crop:w:%d,h:%d,l:%d,t:%d\n",crop.c.width,crop.c.height,crop.c.left,crop.c.top);

	return 0;
}

void process_image(void *buf, int len)
{
	FILE *fp;
	static int id=0;
	char name[10];

	sprintf(name, "test%d.jpg", id++);

	fp = fopen(name, "wb");
	if(fp)
	{
		fwrite(buf,len,1,fp);
		fclose(fp);
	}

	printf("cap:%lu,%d\n", (long)buf,len);
}

int camera_cap(int fd)
{
	struct v4l2_requestbuffers req;
	struct buffer * buffers = NULL;
	int n_buffers;
	struct v4l2_buffer buf;
	enum v4l2_buf_type type;
	int i;

//init
	req.count=4;
	req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory=V4L2_MEMORY_MMAP;

	ioctl(fd,VIDIOC_REQBUFS,&req);

	buffers = calloc(req.count, sizeof(*buffers));
	if(buffers==NULL)
		return -1;

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) 
	{
		memset(&buf, 0, sizeof(buf));		
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;		
		buf.memory = V4L2_MEMORY_MMAP;		
		buf.index = n_buffers; 		
		
		if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buf))			
			return -1;
		
		buffers[n_buffers].length = buf.length;		
		buffers[n_buffers].start = 
		mmap(NULL /* start anywhere */, buf.length,PROT_READ | PROT_WRITE /* required */,MAP_SHARED /* recommended */, fd, buf.m.offset); 		
		
		if (MAP_FAILED == buffers[n_buffers].start)
			return -1;
	}

	for (n_buffers = 0; n_buffers< req.count; ++n_buffers)
	{
		struct v4l2_buffer buf;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;
		ioctl (fd,VIDIOC_QBUF, &buf);
	}

//cap
	type =V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl (fd,VIDIOC_STREAMON, &type);

	memset(&buf, 0, sizeof(buf));	
	buf.type =V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory =V4L2_MEMORY_MMAP;
	
	for(i=0; i<5; i++)
	{
		ioctl (fd,VIDIOC_DQBUF, &buf);
		switch (errno) 
		{			
			case EAGAIN:				
				continue; 		
		}

		process_image(buffers[buf.index].start, buffers[buf.index].length);
		ioctl (fd, VIDIOC_QBUF,&buf);
	}


//uninit
	for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
	{
		if (-1 == munmap(buffers[n_buffers].start, buffers[n_buffers].length))
			perror("munmap");
	}

	free(buffers);
}

int test_camera(struct camera_args *args)
{
	int fd;
	char node_name[32];
	int ret;

	sprintf(node_name, "/dev/video%d", args->camera_node_id);
	printf("open with %s\n", node_name);

	fd = open(node_name, O_RDWR);
	if(fd <= 0)
	{
		perror("open:");
		return -1;
	}

	ret = camera_querycap(fd);

	if(ret > 0)
	{
		printf("The video node support capture\n");

	}else
		printf("The video node not support capture\n");

	camera_enum_fmt(fd);
	camera_set_fmt(fd,args->priv_w,args->priv_h,args->priv_format);
	camera_get_fmt(fd);
	//camera_crop(fd);
	camera_cap(fd);

	close(fd);

	return 0;
}
