#include "../include/myvideo.h"
#include "../include/std.h"
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

/* V4L2 memory mapped image buffer */
struct v4l2_img_buf {
	size_t length;              ///< The size of the buffer
	struct timeval timestamp;   ///< The time value of the image
	void *buf;                  ///< Pointer to the memory mapped buffer
};
/* V4L2 device */
struct v4l2_device {
	char *name;                       ///< The name of the device
    int fd;                           ///< The file pointer to the device
    uint16_t w;                       ///< The width of the image
	uint16_t h;                       ///< The height of the image
	uint8_t buffers_cnt;              ///< The number of image buffers
	volatile uint8_t buffers_deq_idx; ///< The current dequeued index
	struct v4l2_img_buf *buffers;     ///< The memory mapped image buffers
};

struct v4l2_device *v4l2_init(char *devname, uint16_t width, uint16_t height, uint8_t buffers_cnt);

int main()
{
	struct v4l2_device *dev;

	dev = v4l2_init("/dev/video0", 640, 480, 4);

	printf("success\n");
	close(dev->fd);
	free(dev->name);
	free(dev);
	return 0;

}

struct v4l2_device *v4l2_init(char *devname, uint16_t width, uint16_t height, uint8_t buffers_cnt)
{
	int i;
	int fd;
	struct v4l2_format fmt;
	struct v4l2_capability cap;
	struct v4l2_requestbuffers req;

	CLEAR(cap);
	CLEAR(fmt);
	CLEAR(req);

	fd = open_video("/dev/video0");
	//检查各项属性
	if (check_cap(fd, &cap) < 0)
		err_sys("cap_video error");
	if (check_fmt(fd) < 0)
		err_sys("dis_surport_fmt error");
	check_std(fd);
	//set the format settings
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = width;
	fmt.fmt.pix.height = height;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;     //需要更改的地方
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;        //需要更改的地方

	if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
		printf("[V4L2] Could not set data format setting of %s\n", devname);
		close(fd);
		return NULL;
	}

	// Request MMAP buffers
	req.count = buffers_cnt;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
		printf("[v4l2] %s Does not support memory mapping\n", devname);
		close(fd);
		return NULL;
	}

	//获取每个缓存的信息，并mmap到用户空间
	//// Allocate memory for the memory mapped buffers
	struct v4l2_img_buf *buffers = calloc(req.count, sizeof(struct v4l2_img_buf));
	if (buffers == NULL) {
		printf("[v4l2] Not enough memory for %s to initialize %d MMAP buffers\n", devname, req.count);
		close(fd);
		return NULL;
	}
	
	for (i = 0; i < req.count; ++i) {
		struct v4l2_buffer buf;

		CLEAR(buf);
		// Request the buffer information
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
			printf("[v4l2] Querying buffer %d from %s failed\n", i, devname);
			free(buffers);
		    close(fd);
		    return NULL;
		}
		
		//  Map the buffer
		buffers[i].length = buf.length;
		buffers[i].buf = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
		if (buffers[i].buf == MAP_FAILED) {
			printf("[v4l2] Mapping buffer %d with length %d from %s failed\n", i, buf.length, devname);
		    free(buffers);
			close(fd);
			return NULL;
		}
	}
	// Create the device only when everything succeeded
	struct v4l2_device *dev = (struct v4l2_device *)malloc(sizeof(struct v4l2_device));
	CLEAR(*dev);
	dev->fd  = fd;
	dev->w   = width;
	dev->h   = height;
	dev->name = strdup(devname);// 需要释放
	dev->buffers_cnt = req.count;
	dev->buffers = buffers;

	return dev;
}
