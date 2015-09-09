#include "myvideo.h"


int main()
{
	int fd;
	struct v4l2_format fmt;
	struct v4l2_capability cap;

	CLEAR(cap);
	CLEAR(fmt);

	fd = open_video("/dev/video0");
	//检查各项属性
	if (check_cap(fd, &cap) < 0)
		err_sys("cap_video error");
	if (check_fmt(fd) < 0)
		err_sys("dis_surport_fmt error");
	check_std(fd);
	//配置

	close(fd);

	return 0;

}
