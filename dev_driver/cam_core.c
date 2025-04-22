// cam_core.c - Updated to use V4L2 backend

#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include "cam_defs.h"

// External V4L2 interface from cam_v4l2.c
extern int v4l2_open_camera(const char *devname);
extern int v4l2_capture_frame(char **frame_data, size_t *frame_len);
extern int v4l2_requeue_frame(void);
extern void v4l2_close_camera(void);

int cam_start_capture(void)
{
    int ret;

    if (cam_ctx.device_opened)
        return 0;

    ret = v4l2_open_camera(cam_ctx.device_name);
    if (ret) {
        pr_err("cam_core: failed to initialize camera device\n");
        return ret;
    }

    pr_info("cam_core: capture started\n");
    return 0;
}

ssize_t cam_read_frame(char __user *buf, size_t len)
{
    char *frame_data;
    size_t frame_len;
    int ret;

    ret = v4l2_capture_frame(&frame_data, &frame_len);
    if (ret < 0) {
        pr_err("cam_core: failed to capture frame\n");
        return ret;
    }

    if (copy_to_user(buf, frame_data, min(len, frame_len))) {
        pr_err("cam_core: failed to copy frame to user\n");
        v4l2_requeue_frame();
        return -EFAULT;
    }

    v4l2_requeue_frame();
    return min(len, frame_len);
}

void cam_stop_capture(void)
{
    v4l2_close_camera();
    pr_info("cam_core: capture stopped\n");
}
EXPORT_SYMBOL(cam_start_capture);
EXPORT_SYMBOL(cam_read_frame);
EXPORT_SYMBOL(cam_stop_capture);

MODULE_LICENSE("GPL");
