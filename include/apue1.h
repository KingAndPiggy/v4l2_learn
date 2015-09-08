#ifndef _APUE_H
#define _APUE_H
#define _POSIX_C_SOURCE 200809L
#if defined(SOLARIS) /* Solaris 10 */
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 700
#endif
#include <sys/types.h> /* some systems still require this */
#include <sys/stat.h>
#include <sys/termios.h> /* for winsize */
#if defined(MACOS) || !defined(TIOCGWINSZ)
#include <sys/ioctl.h>
#endif
#include <stdio.h> /* for convenience */
#include <stdlib.h> /* for convenience */
#include <stddef.h> /* for offsetof */
#include <string.h> /* for convenience */
#include <unistd.h> /* for convenience */
#include <signal.h> /* for SIG_ERR */
#define MAXLINE 4096  /* max line length */
/*
    * Default file access permissions for new files.
	*/
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
/*
    * Default permissions for new directories.
	*/
#define DIR_MODE (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
typedef void Sigfunc(int); /* for signal handlers */
#define min(a,b) ((a) < (b) ? (a):(b))
#define max(a,b) ((a) > (b) ? (a):(b))
/*
    * Prototypes for our own functions.
	*/
char *path_alloc(size_t *);
long open_max(void);
int set_cloexec(int);
void clr_fl(int, int);
void set_fl(int, int);
void pr_exit(int);
void pr_mask(const char *);
Sigfunc *signal_intr(int, Sigfunc *);
void daemonize(const char *);
void sleep_us(unsigned int);
ssize_t readn(int, void *, size_t);
ssize_t writen(int, const void *, size_t);
int fd_pipe(int *);
int recv_fd(int, ssize_t (*func)(int, const void *, size_t));
int send_fd(int, int);
int send_err(int, int, const char *);
int serv_listen(const char *);
int serv_accept(int, uid_t *);
int cli_conn(const char *);
int buf_args(char *, int (*func)(int, char **));
int tty_cbreak(int);
int tty_raw(int);
int tty_reset(int);
void tty_atexit(void);
struct termios *tty_termios(void);
int ptym_open(char *, int);
int ptys_open(char *);
#ifdef TIOCGWINSZ
pid_t pty_fork(int *, char *, int, const struct termios *, const struct winsize *);
#endif
int lock_reg(int, int, int, off_t, int, off_t);
#define read_lock(fd, offset, whence, len) lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))
pid_t lock_test(int, int, off_t, int, off_t);
#define is_read_lockable(fd, offset, whence, len) (lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define is_write_lockable(fd, offset, whence, len) (lock_test((fd), F_WRCLK, (offset), (whence), (len)) == 0)
void err_msg(const char *, ...);
void err_dump(const char *, ...);
void err_quit(const char *, ...);
void err_cont(int, const char *, ...);
void err_exit(int, const char *, ...);
void err_ret(const char *, ...);
void err_sys(const char *, ...) __attribute__((noreturn));
void log_msg(const char *, ...);
void log_open(const char *, int, int);
void log_quit(const char *, ...) __attribute__((noreturn));
void log_ret(const char *, ...);
void log_sys(const char *, ...)__attribute__((noreturn));
void log_exit(int, const char *, ...)__attribute__((noreturn));
void TELL_WAIT(void);
void TELL_PARENT(pid_t);
void TELL_CHILD(pid_t);
void WAIT_PARENT(void);
void WAIT_CHILD(void);
#include <errno.h> /* for definition of errno */
#include <stdarg.h> /* ISO C variable arguments */
static void err_doit(int, int, const char *, va_list);
/*
    * Nonfatal error related to a system call.
	 * Print a message and return.
	 */
void
err_ret(const char *fmt, ...)
{
	  va_list ap;
	    va_start(ap, fmt);
		  err_doit(1, errno, fmt, ap);
		    va_end(ap);
}
/*
    * Fatal error related to a system call.
	 * Print a message and terminate.
	 */
void 
err_sys(const char *fmt, ...)
{
	  va_list ap;
	    va_start(ap, fmt);
		  err_doit(1, errno, fmt, ap);
		    va_end(ap);
			  exit(1);
}
/*
    * Nonfatal error unrelated to a system call.
	 * Error code passed as explict parameter.
	  * Print a message and return.
	  */
void
err_cont(int error, const char *fmt, ...)
{
	  va_list ap;
	    va_start(ap, fmt);
		  err_doit(1, errno, fmt, ap);
		    va_end(ap);
}
/*
    * Fatal error unrelated to a system call.
	 * Error code passed as explict parameter.
	  * Print a message and terminate.
	  */
void 
err_exit(int error, const char *fmt, ...)
{
	  va_list ap;
	    va_start(ap, fmt);
		  err_doit(1, errno, fmt, ap);
		    va_end(ap);
			  exit(1);
}
/*
    * Fatal error related to a system call.
	 * Print a message, dump core, and terminate.
	 */
void
err_dump(const char *fmt, ...)
{
	  va_list ap;
	    va_start(ap, fmt);
		  err_doit(1, errno, fmt, ap);
		    va_end(ap);
			  abort(); /* core dump and terminate */
			    exit(1); /* should't get here */
}
/*
    * Nonfatal error unrelated to a system call.
	 * Print a message and return.
	 */
void 
err_msg(const char *fmt, ...)
{
	  va_list ap;
	    va_start(ap, fmt);
		  err_doit(0, 0, fmt, ap);
		    va_end(ap);
}
/*
    * Fatal error unrelated to a system call.
	 * Print a message and terminate.
	 */
void
err_quit(const char *fmt, ...)
{
	  va_list ap;
	    va_start(ap, fmt);
		  err_doit(0, 0, fmt, ap);
		    va_end(ap);
			  exit(1);
}
/*
    * Print a message and return to caller.
	 * Caller specifies "errnoflag"
	 */
static void 
err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	  char buf[MAXLINE];
	    vsnprintf(buf, MAXLINE-1, fmt, ap);
		  if(errnoflag){
			      snprintf(buf+strlen(buf), MAXLINE-strlen(buf) - 1, ": %s", strerror(error));
				    }
		    strcat(buf, "\n");
			  fflush(stdout); /* in case stdout and stderr are the same */
			    fputs(buf, stderr);
				  fflush(NULL); /* flushes all stdio output streams */
}
#include <syslog.h>
static void log_doit(int, int, int, const char *, va_list ap);
int log_to_stderr = 0;
/*
    * Initialize syslog(), if running as daemon.
	*/
void 
log_open(const char *ident, int option, int facility)
{
	  if(log_to_stderr == 0){
		      openlog(ident, option, facility);
			    }
}
/*
    * Nonfatal error related to a system call.
	 * Print a message with the system's errno value and return.
	 */
void 
log_ret(const char *fmt, ...)
{
	  va_list ap;
	    va_start(ap, fmt);
		  log_doit(1, errno, LOG_ERR, fmt, ap);
		    va_end(ap);
}
/*
    * Fatal error related to a system call.
	 * Print a message and terminate.
	 */
void 
log_sys(const char *fmt, ...)
{
	  va_list ap;
	    va_start(ap, fmt);
		  log_doit(1, errno, LOG_ERR, fmt, ap);
		    va_end(ap);
			  exit(2);
}
/*
    * Nonfatal error unrelated to a system call.
	 * Print a message and return.
	 */
void
log_msg(const char *fmt, ...)
{
	  va_list ap;
	    va_start(ap, fmt);
		  log_doit(0, 0, LOG_ERR, fmt, ap);
		    va_end(ap);
}
/*
    * Fatal error related to a system call.
	 * Print a message and terminate.
	 */
void 
log_quit(const char *fmt, ...)
{
	  va_list ap;
	    va_start(ap, fmt);
		  log_doit(0, 0, LOG_ERR, fmt, ap);
		    va_end(ap);
			  exit(2);
}
/*
    * Fatal error related to a system call.
	 * Error number passed as an explicit parameter.
	  * Print a message and terminate.
	  */
void
log_exit(int error, const char *fmt, ...)
{
	  va_list ap;
	    va_start(ap, fmt);
		  log_doit(1, error, LOG_ERR, fmt, ap);
		    va_end(ap);
			  exit(2);
}
/*
    * Print a message and return to caller.
	 * Caller specifies "errnoflag" and "priority".
	 */
static void 
log_doit(int errnoflag, int error, int priority, const char *fmt, va_list ap)
{
	  char buf[MAXLINE];
	    vsnprintf(buf, MAXLINE-1, fmt, ap);
		  if(errnoflag){
			      snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1, ": %s", strerror(error));
				    }
		    strcat(buf, "\n");
			  if(log_to_stderr){
				      fflush(stdout);
					      fputs(buf, stderr);
						      fflush(stderr);
							    }else{
									    syslog(priority, "%s", buf);
										  }
}
#include <sys/wait.h>
void 
pr_exit(int status)
{
	  if(WIFEXITED(status))
		      printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
	    else if(WIFSIGNALED(status))
			    printf("abnormal termination, signal number = %d%s\n", WTERMSIG(status), 
#ifdef WCOREDUMP
						    WCOREDUMP(status)?"(core file generated)":"");
#else
		    "");
#endif
			 else if(WIFSTOPPED(status))
				    printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}
 
void
pr_mask(const char *str)
{
	  sigset_t sigset;
	    int errno_save;
		  errno_save = errno;
		    if(sigprocmask(0, NULL, &sigset) < 0){
				    err_ret("sigprocmask error");
					  }else {
						      printf("%s", str);
							       
							      if(sigismember(&sigset, SIGINT))
									        printf(" SIGINT");
								      if(sigismember(&sigset, SIGQUIT))
										        printf(" SIGQUIT");
									      if(sigismember(&sigset, SIGUSR1))
											        printf(" SIGUSR1");
										      if(sigismember(&sigset, SIGALRM))
												        printf(" SIGALRM");
											      printf("\n");
												    }
			  errno = errno_save;
}
#endif
