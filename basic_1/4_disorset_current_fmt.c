#include "../include/apue.h"
#include "myvideo.h"

#define DEVNAME "/dev/video0"

int main()
{
	int fd;
	
	fd = open_video(DEVNAME);

	dis_surport_fmt(fd);
	close(fd);
}
