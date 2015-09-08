/* 
 *查看设备属性
 */

#include "myvideo.h"
#include "../include/apue.h"
#define DEVNAME "/dev/video0"

int main()
{
	int fd;
	struct v4l2_capability cap;

	fd = open_video(DEVNAME);
	cap_video(fd, &cap, TRUE);
	close(fd);
	return 0;
}
