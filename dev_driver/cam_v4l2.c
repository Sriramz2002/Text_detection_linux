// cam_v4l2.c — Method B: Streaming-based V4L2 Capture

#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/videodev2.h>
#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/fdtable.h>
#include <media/v4l2-dev.h>
#include "cam_defs.h"

#define FRAME_WIDTH  320
#define FRAME_HEIGHT 240
#define FRAME_PIX_FMT V4L2_PIX_FMT_YUYV
#define FRAME_SIZE (FRAME_WIDTH * FRAME_HEIGHT * 2)
#define MAX_BUFFERS 4

static struct file *v4l2_filp = NULL;
static struct v4l2_buffer queued_buffers[MAX_BUFFERS];
static void *kernel_buffers[MAX_BUFFERS];
static int streaming = 0;

static int __v4l2_ioctl(unsigned int cmd, void *arg)
{
    mm_segment_t oldfs = get_fs();
    set_fs(KERNEL_DS);
    long ret = v4l2_filp->f_op->unlocked_ioctl(v4l2_filp, cmd, (unsigned long)arg);
    set_fs(oldfs);
    return ret;
}

int v4l2_open_camera(const char *devname)
{
    struct v4l2_format fmt = {0};
    struct v4l2_requestbuffers req = {0};
    int i;

    v4l2_filp = filp_open(devname, O_RDWR, 0);
    if (IS_ERR(v4l2_filp)) {
        pr_err("v4l2: failed to open %s\n", devname);
        return PTR_ERR(v4l2_filp);
    }

    // Set format
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = FRAME_WIDTH;
    fmt.fmt.pix.height = FRAME_HEIGHT;
    fmt.fmt.pix.pixelformat = FRAME_PIX_FMT;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (__v4l2_ioctl(VIDIOC_S_FMT, &fmt) < 0) {
        pr_err("v4l2: VIDIOC_S_FMT failed\n");
        return -EINVAL;
    }

    // Request buffers
    req.count = MAX_BUFFERS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (__v4l2_ioctl(VIDIOC_REQBUFS, &req) < 0) {
        pr_err("v4l2: VIDIOC_REQBUFS failed\n");
        return -ENOMEM;
    }

    // Map and queue buffers
    for (i = 0; i < req.count; i++) {
        struct v4l2_buffer buf = {
            .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
            .memory = V4L2_MEMORY_MMAP,
            .index = i
        };

        if (__v4l2_ioctl(VIDIOC_QUERYBUF, &buf) < 0) {
            pr_err("v4l2: QUERYBUF failed at %d\n", i);
            return -EFAULT;
        }

        kernel_buffers[i] = vm_mmap(v4l2_filp, 0, buf.length,
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED, buf.m.offset);
        if (IS_ERR(kernel_buffers[i])) {
            pr_err("v4l2: mmap failed for buffer %d\n", i);
            return PTR_ERR(kernel_buffers[i]);
        }

        // Queue it
        if (__v4l2_ioctl(VIDIOC_QBUF, &buf) < 0) {
            pr_err("v4l2: QBUF failed\n");
            return -EFAULT;
        }

        queued_buffers[i] = buf;
    }

    // Start streaming
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (__v4l2_ioctl(VIDIOC_STREAMON, &type) < 0) {
        pr_err("v4l2: STREAMON failed\n");
        return -EINVAL;
    }

    streaming = 1;
    pr_info("v4l2: streaming started\n");
    return 0;
}

int v4l2_capture_frame(char **out_data, size_t *out_len)
{
    struct v4l2_buffer buf = {
        .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
        .memory = V4L2_MEMORY_MMAP
    };

    if (!streaming)
        return -EINVAL;

    if (__v4l2_ioctl(VIDIOC_DQBUF, &buf) < 0) {
        pr_err("v4l2: DQBUF failed\n");
        return -EIO;
    }

    if (buf.index >= MAX_BUFFERS || !kernel_buffers[buf.index]) {
        pr_err("v4l2: invalid buffer index\n");
        return -EFAULT;
    }

    *out_data = kernel_buffers[buf.index];
    *out_len = buf.bytesused;

    // Queue back
    if (__v4l2_ioctl(VIDIOC_QBUF, &buf) < 0) {
        pr_warn("v4l2: failed to requeue buffer %d\n", buf.index);
    }

    return 0;
}

void v4l2_close_camera(void)
{
    int i;
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (!v4l2_filp || IS_ERR(v4l2_filp))
        return;

    if (streaming) {
        __v4l2_ioctl(VIDIOC_STREAMOFF, &type);
        streaming = 0;
    }

    for (i = 0; i < MAX_BUFFERS; i++) {
        if (kernel_buffers[i]) {
            vm_munmap((unsigned long)kernel_buffers[i], FRAME_SIZE);
            kernel_buffers[i] = NULL;
        }
    }

    filp_close(v4l2_filp, NULL);
    v4l2_filp = NULL;

    pr_info("v4l2: streaming stopped and camera closed\n");
}

EXPORT_SYMBOL(v4l2_open_camera);
EXPORT_SYMBOL(v4l2_capture_frame);
EXPORT_SYMBOL(v4l2_close_camera);
MODULE_LICENSE("GPL");
