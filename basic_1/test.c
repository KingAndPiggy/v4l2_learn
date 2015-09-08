#include "../include/apue.h"
#include "myvideo.h"


int main()
{
	int fd;

	fd = open_video("/dev/video0");
	check_dev_std(fd);

	//设置视频捕获格式
	struct v4l2_format fmt;

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width  = 640;
	fmt.fmt.pix.height = 480;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	close(fd);

	return 0;
}
