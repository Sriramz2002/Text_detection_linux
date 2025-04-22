// cam_defs.h

#ifndef CAM_DEFS_H
#define CAM_DEFS_H
#include <linux/time64.h>  
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/videodev2.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/cdev.h>

#define DEVICE_NAME "camdevice"
#define DEFAULT_WIDTH  320
#define DEFAULT_HEIGHT 240
#define DEFAULT_PIX_FMT V4L2_PIX_FMT_YUYV
#define MAX_BUFFER_COUNT 6

enum cam_io_method {
    IO_METHOD_READ,
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR
};

struct cam_buffer {
    void   *start;
    size_t  length;
};

struct cam_device_ctx {
    struct v4l2_format fmt;
    enum cam_io_method io;
    struct cam_buffer *buffers;
    unsigned int n_buffers;
    int fd;
    int out_buf;
    int force_format;
    int frame_count;
    int device_opened;
    //struct timespec frame_time;
    struct mutex lock;
    char *device_name;
};

extern struct cam_device_ctx cam_ctx;

#endif
