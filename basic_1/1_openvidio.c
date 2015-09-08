/*
 *使用OPEN打开video0 设备/dev/video0
 *
 */

#include "../include/apue.h"
#define DEVNAME "/dev/video0"
int open_video(char *devname)
{
	int fd;
	fd = open(devname, O_RDWR);
	if (fd == -1) return -1;
	return fd;
}
int main()
{
	int fd;

	fd = open_video(DEVNAME);
	if (fd == -1)
		err_sys("open video0 error");
	
	printf("open video0 success, fd: %d\n", fd);


	close(fd);

}
