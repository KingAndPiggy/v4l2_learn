/*
 *
 */
#ifndef __MYVIDEO_H
#define __MYVIDEO_H

#include "../include/apue.h"
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <stropts.h>

#define FALSE 0
#define TRUE  1

/*
 *使用OPEN打开video0 设备/dev/video0
 */
int open_video(char *devname)
{
	int fd;
	//fd = open(devname, O_RDWR, 0);
	fd = open(devname, O_RDWR | O_NONBLOCK, 0);
	if (fd == -1) 
		err_sys("open %s video device failed\n", devname);
	return fd;
}
/*
 *获取设备VIDEO属性
 */
void cap_video(int fd, struct v4l2_capability *cap, char i)
{
	ioctl(fd, VIDIOC_QUERYCAP, cap);
	if (i) {
		printf("Diver name: %s\n", cap->driver);  //驱动名
		printf("card  name: %s\n", cap->card);    //设备名
		printf("Bus   info: %s\n", cap->bus_info);//设备在系统中的位置
		printf("Diver version:%u.%u.%u\n", (cap->version>>16)&0XFF, \
										   (cap->version>>8)&0xFF,  \
										   cap->version&0xFF);
		if (cap->capabilities & V4L2_CAP_VIDEO_CAPTURE)
			printf("is or not surport image:%s\n", "yes");
		else 
			printf("is or not surport image:%s\n", "no");
	}	
}

/*
 *显示所支持的格式
 */
void dis_surport_fmt(int fd)
{
	struct v4l2_fmtdesc fmtdesc; 
	struct v4l2_format fmt; 
	
	fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; 

	fmtdesc.index=0; 
	fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; 
	
	printf("Surport format:");
	while(ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1)
	{
		printf("\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);
		fmtdesc.index++;
	}
	ioctl(fd, VIDIOC_G_FMT, &fmt);
	printf("Current data format information:\n\twidth:%d\n\theight:%d\n",
			fmt.fmt.pix.width,fmt.fmt.pix.height);
}

/*
 *检查当前视频设备支持的标准
 */
void check_dev_std(int fd)
{
	v4l2_std_id std;
	int ret;

	do {
		ret = ioctl(fd, VIDIOC_QUERYSTD, &std);	
	}while (ret == -1 && errno == EAGAIN);

	switch (std) {
		case V4L2_STD_NTSC:
			printf("Current video device surport: NTSC fmt\n");
			break;
		case V4L2_STD_PAL:
			printf("Current video device surport: PAL fmt\n");
			break;
		default:
			printf("Other format\n");
			break;
	}
}
#endif /*end myvideo.h*/

