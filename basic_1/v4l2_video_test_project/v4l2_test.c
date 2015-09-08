//cut a picture
//my include 

#include "./include/apue.h"
#include "./include/parent_child_sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
 
#include <getopt.h>           
 
#include <fcntl.h>            
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
 
#include <asm/types.h>        
#include <linux/videodev2.h>
 
#include "./include/myvideo.h"
#define CLEAR(x) memset (&(x), 0, sizeof (x))
 
struct buffer {
	        void *                  start;
			        size_t                  length;
};
 
static char *           dev_name        = "/dev/video0";//摄像头设备名
static int              fd              = -1;
struct buffer *         buffers         = NULL;
static unsigned int     n_buffers       = 0;

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_FORMAT V4L2_PIX_FMT_MJPEG
					//V4L2_PIX_FMT_MJPEG
                   //V4L2_PIX_FMT_JPEG
                   //V4L2_PIX_FMT_YUYV
                   //V4L2_PIX_FMT_YVU420
                   //V4L2_PIX_FMT_RGB32


FILE *file_fd;

pid_t pid;
int filedes[2];

char buffer[10];
#define CAPTURE_FILE "test.jpg"
static unsigned long file_length;

//////////////////////////////////////////////////////
//获取一帧数据
//从视频缓冲区的输出队列中取得一个已经保存有一帧视频数据的视频缓冲区
//////////////////////////////////////////////////////
static int read_frame (void)
{
     file_fd = fopen(CAPTURE_FILE, "w");//图片文件名
     struct v4l2_buffer buf;

     CLEAR (buf);
     buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
     buf.memory = V4L2_MEMORY_MMAP;
     if(ioctl (fd, VIDIOC_DQBUF, &buf) == -1)
     {
	     printf("VIDIOC_DQBUF failture\n"); //出列采集的帧缓冲
         exit(1);
     }

     assert (buf.index < n_buffers);
     //  printf ("buf.index dq is %d,\n",buf.index);
								   
     //将其写入文件中
     fwrite(buffers[buf.index].start, buffers[buf.index].length, 1, file_fd);
     //再将其入列
     if(ioctl (fd, VIDIOC_QBUF, &buf)<0)
	       printf("failture VIDIOC_QBUF\n");
	 fclose(file_fd);
	 TELL_CHILD(pid);
	 WAIT_CHILD();
     return 1;
}
 
int main (int argc,char ** argv)
{
	 char argv2_buf[4];

	 TELL_WAIT();
	 memset(argv2_buf, '\0', 4);
	 sprintf(argv2_buf, "%d%d", pfd2[1], pfd1[0]);
	 memset(buffer, '\0', 10);
     //打开设备
	 fd = open_video(dev_name);

      //获取驱动信息//获取摄像头参数//查询驱动功能并打印
      struct v4l2_capability cap;
	  cap_video(fd, &cap, TRUE);
						   
	//获取设备支持的视频格式
      struct v4l2_fmtdesc fmtdesc;     
      CLEAR (fmtdesc);
      fmtdesc.index = 0;
      fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      printf("\nSupport format:\n");
      while ((ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc)) == 0)
      {
	      printf("/t%d.\n{\npixelformat = '%c%c%c%c',\ndescription = '%s'\n }\n",
									                   fmtdesc.index+1,
	         			                               fmtdesc.pixelformat & 0xFF,
				                                       (fmtdesc.pixelformat >> 8) & 0xFF,
					                                   (fmtdesc.pixelformat >> 16) & 0xFF,
						                               (fmtdesc.pixelformat >> 24) & 0xFF,
						                               fmtdesc.description);  
	    fmtdesc.index++;
	  }  

	  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	  {
		  fprintf (stderr, "%s is no video capture device\n", dev_name);
	      exit (EXIT_FAILURE);
	  }
    //检查是否支持某种帧格式   
     struct v4l2_format fmt2;
     fmt2.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; 
	 fmt2.fmt.pix.pixelformat = VIDEO_FORMAT;

     if(ioctl(fd,VIDIOC_TRY_FMT,&fmt2)==-1)  
     if(errno==EINVAL)
           printf("not support format %d!\n",VIDEO_FORMAT);   
      //设置视频捕获格式
     struct v4l2_format fmt;
     CLEAR (fmt);
     fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	 fmt.fmt.pix.width       = 640;
	 fmt.fmt.pix.height      = 480;
	 fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
	 fmt.fmt.pix.pixelformat = VIDEO_FORMAT;

    //设置图像格式
    if(ioctl (fd, VIDIOC_S_FMT, &fmt) < 0)
    {
	    printf("failture VIDIOC_S_FMT\n");
        exit(1);
    }
   // 显示当前帧的相关信息
	dis_surport_fmt(fd);

	//视频分配捕获内存
	struct v4l2_requestbuffers req;
	CLEAR (req);
	req.count               = 4;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_MMAP;
									   
   //申请缓冲，count是申请的数量
	if(ioctl (fd, VIDIOC_REQBUFS, &req) < 0)
	{
		printf("failture VIDIOC_REQBUFS\n");
		exit(1);
	}
	if (req.count < 2)
	printf("Insufficient buffer memory\n");

	 //内存中建立对应空间
	 //获取缓冲帧的地址、长度
	 buffers = calloc (req.count, sizeof (*buffers));//在内存的动态存储区中分配n个长度为size的连续空间，函数返回一个指向分配起始地址的指针
	 if (!buffers)
	 {
	     fprintf (stderr, "Out of memory/n");
	     exit (EXIT_FAILURE);
	 }
										  
	 for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
	 {
		 struct v4l2_buffer buf;   //驱动中的一帧
         CLEAR (buf);
         buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         buf.memory      = V4L2_MEMORY_MMAP;
         buf.index       = n_buffers;// 要获取内核视频缓冲区的信息编号

         if (-1 == ioctl (fd, VIDIOC_QUERYBUF, &buf)) //映射用户空间
		 {
			 printf ("VIDIOC_QUERYBUF error\n");
			 exit(-1);
		}
         buffers[n_buffers].length = buf.length; 

      // 把内核空间缓冲区映射到用户空间缓冲区
        buffers[n_buffers].start = mmap (NULL ,    //通过mmap建立映射关系
									      buf.length,
										  PROT_READ | PROT_WRITE ,
										  MAP_SHARED ,
										  fd,
										  buf.m.offset);
																																   
       if (MAP_FAILED == buffers[n_buffers].start)
        {
             printf ("mmap failed\n");
           exit(1);
       }
   }

 //投放一个空的视频缓冲区到视频缓冲区输入队列中
 //把四个缓冲帧放入队列，并启动数据流
        unsigned int i;
     // 将缓冲帧放入队列
     enum v4l2_buf_type type;
    for (i = 0; i < n_buffers; ++i)
    {
          struct v4l2_buffer buf;
          CLEAR (buf);
          buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
          buf.memory      = V4L2_MEMORY_MMAP;
          buf.index       = i; //指定要投放到视频输入队列中的内核空间视频缓冲区的编号;

         if (-1 == ioctl (fd, VIDIOC_QBUF, &buf))//申请到的缓冲进入列队
             printf ("VIDIOC_QBUF failed\n");
    }
    //开始捕捉图像数据  
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == ioctl (fd, VIDIOC_STREAMON, &type))
    {
          printf ("VIDIOC_STREAMON failed\n");
        exit(1);
    }

	if (pipe(filedes) == -1)
		fprintf(stderr, "pipe failed\n");
	pid = fork();

	if (pid < 0) 
		fprintf(stderr, "fork failed\n");
	else if (pid == 0) 
		execl("./opencv_dis", "opencv_dis", "test.jpg", argv2_buf, (char *)0);
	else
		for (;;) //这一段涉及到异步IO
		{
		    fd_set fds;
		    struct timeval tv;
			int r;

			 FD_ZERO (&fds);//将指定的文件描述符集清空
			 FD_SET (fd, &fds);//在文件描述符集合中增加一个新的文件描述符
																					 
																																  
			 tv.tv_sec = 2;
			 tv.tv_usec = 0;
																					   
			 r = select (fd + 1, &fds, NULL, NULL, &tv);//判断是否可读（即摄像头是否准备好），tv是定时

			 if (-1 == r)
			 {
				  if (EINTR == errno)
				  continue;
				 printf ("select err\n");
			}
			if (0 == r)
			{
				fprintf (stderr, "select timeout\n");
				exit (EXIT_FAILURE);
			}
			if (read_frame ())//如果可读，执行read_frame ()函数，并跳出循环
				;
		}
 /////// Release the resource////////////////////////////////////////////////////  
	unsigned int ii;
	for (ii = 0; ii < n_buffers; ++ii)
	if (-1 == munmap (buffers[ii].start, buffers[ii].length))   
	free (buffers);
								    
	close (fd);
	printf("Camera test Done.\n");
	fclose (file_fd);
	//exit (EXIT_SUCCESS);
	return 0;
}
