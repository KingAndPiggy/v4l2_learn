/*
 *
 */
#ifndef __MYVIDEO_H
#define __MYVIDEO_H

#include "./apue.h"
#include <string.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <stropts.h>

#define FALSE 0
#define TRUE  1

#define CLEAR(x) memset(&(x), 0, sizeof (x))

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
 *@[cap] struct v4l2_capability cap
 *@[命令]  VIDIOC_QUERYCAP
 */
int check_cap(int fd, struct v4l2_capability *cap)
{
	if (ioctl(fd, VIDIOC_QUERYCAP, cap) < 0) {
		printf("[v4l2]:can't read  capability of v4l2 video\n");
		return -1;
	}
	
	printf("-----------------video property--------------------------\n");
	printf("Diver name: %s\n", cap->driver);  //驱动名
	printf("card  name: %s\n", cap->card);    //设备名
	printf("Bus   info: %s\n", cap->bus_info);//设备在系统中的位置
	printf("Diver version:%u.%u.%u\n", (cap->version>>16)&0XFF, \
									   (cap->version>>8)&0xFF,  \
									   cap->version&0xFF);
	if (cap->capabilities & V4L2_CAP_VIDEO_CAPTURE)
		printf("is or not surport capture: %s\n", "YES");
	else 
		printf("is or not surport capture: %s\n", "NO");
	if (cap->capabilities & V4L2_CAP_STREAMING)
		printf("is or not surport streaming: %s\n", "YES");
	else 
		printf("is or not surport streaming: %s\n", "NO");
	return 0;
}

/*
 *显示所支持的格式
 *@ struct v4l2_fmtdesc fmtdesc 命令 VIDIOC_ENUM_FMT  获取当前CAM所支持的格式
 *@ struct v4l2_format  fmt     命令 VIDIOC_G_FMT     获取当前设置VIDEO的捕获格式
 */
int check_fmt(int fd)
{
	struct v4l2_fmtdesc fmtdesc; 
	struct v4l2_format fmt; 

	CLEAR(fmtdesc);
	CLEAR(fmt);
	
	fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; 

	fmtdesc.index=0; 
	fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; 
		
	printf("-----------------video surport fmt-----------------------\n");
	printf("Surport format:");
	while(ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1) {
		printf("\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);
		fmtdesc.index++; 
	}
	if (ioctl(fd, VIDIOC_G_FMT, &fmt) < 0) {
		printf("[v4l2] Could not set data format settings of v4l2 video\n");
		return -1;
	}
	printf("Current data format information:\n\twidth:%d\n\theight:%d\n",
			fmt.fmt.pix.width,fmt.fmt.pix.height);

	return 0;
}

/*
 *检查当前视频设备支持的标准
 *@ v4l2_std_id std 命令  VIDIOC_QUERYSTD
 */
void check_std(int fd)
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
			printf("Current video device surport: other fmt\n");
			break;
	}
}

/*
 *Read video cropping and scaling information VIDIOC_CROPCAP
 *@ struct v4l2_cropcap crop   命令 VIDIOC_CROPCAP
 */
int check_cropcap(int fd)
{
	struct v4l2_cropcap cropcap;

	CLEAR(cropcap);
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_CROPCAP, &cropcap) < 0)
		return -1;

	printf("----------------------MAX CROP---------------------------\n");
	printf("MAX  width: %d\n", cropcap.bounds.width);
	printf("MAX height: %d\n", cropcap.bounds.height);
	printf("MAX   left: %d\n", cropcap.bounds.left);
	printf("MAX    top: %d\n", cropcap.bounds.top);
	printf("----------------------DEF CROP---------------------------\n");
	printf("MAX  width: %d\n", cropcap.defrect.width);
	printf("MAX height: %d\n", cropcap.defrect.height);
	printf("MAX   left: %d\n", cropcap.defrect.left);
	printf("MAX    top: %d\n", cropcap.defrect.top);


	return 0;
}
#endif /*end myvideo.h*/

