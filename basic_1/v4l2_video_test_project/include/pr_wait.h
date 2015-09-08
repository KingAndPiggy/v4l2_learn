#ifndef  __PR_WAIT_H__
#define  __PR_WAIT_H__
#include "./apue.h"
#include <sys/wait.h>

void pr_wait(int status)
{
	if (WIFEXITED(status))
		printf("normal termination, exit status = %d\n",
				WEXITSTATUS(status));
	else if (WIFSIGNALED(status)) 
		printf("abnormal termination, siganl number = %d%s\n", 
				WTERMSIG(status),
#ifdef WCOREDUMP
				WCOREDUMP(status) ? " (core file generated)" : "");
#else 
				"");
#endif
	else if (WIFSTOPPED(status))
		printf("child stopped, signal number = %d\n",
				WSTOPSIG(status));	
}

#endif /*end pr_wait.h*/

