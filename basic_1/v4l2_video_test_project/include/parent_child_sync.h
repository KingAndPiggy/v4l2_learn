/*
 *使用管道实现parent and child process 同步
 *
 * parent-------->pfd1[1]----->pfd1[0]------->child     
 * parent<--------pfd2[0]<-----pfd2[1]<-------child     
 *
 */

#ifndef __PAR_CHA_SYNC
#define __PAR_CHA_SYNC

#include "./apue.h"

static int pfd1[2], pfd2[2];
//create two pipe 创建2个管道
void TELL_WAIT(void)
{
	if (pipe(pfd1) < 0 || pipe(pfd2) < 0)
		err_sys("pipe error");
	printf("fd1:%d %d \nfd2:%d %d\n", pfd1[0], pfd1[1], pfd2[0], pfd2[1]);
}
//tell parent
void TELL_PARENT(pid_t pid)
{
	if (write(pfd2[1], "c", 1) != 1)
		err_sys("write error to pfd2[1]");
}	
//wait parent
void WAIT_PARENT(void)
{
	char c;

	if (read(pfd1[0], &c, 1) != 1)
		err_sys("read error for pfd1[0]");

	if (c != 'p')
		err_sys("WAIT_PARENT: incorrect data");
}
//tell child
void TELL_CHILD(pid_t pid)
{
	if (write(pfd1[1], "p", 1) != 1)
		err_sys("write error to pfd1[1]");
}	
//wait child
void WAIT_CHILD(void)
{
	char c;

	if (read(pfd2[0], &c, 1) != 1)
		err_sys("read error for pfd1[0]");

	if (c != 'c')
		err_sys("WAIT_PARENT: incorrect data");
}

#endif /* parent_child_sync.h end*/

