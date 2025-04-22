// cam_device.c

#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/printk.h>
#include "cam_defs.h"
extern int v4l2_open_camera(const char *devname);
extern int v4l2_capture_frame(char **out_data, size_t *out_len);
// Forward declarations of core capture logic (to be implemented in cam_core.c)
extern int cam_start_capture(void);
extern ssize_t cam_read_frame(char __user *buf, size_t len);
extern void cam_stop_capture(void);

static int cam_open(struct inode *inode, struct file *file)
{
    int ret = 0;

    mutex_lock(&cam_ctx.lock);

    if (cam_ctx.device_opened) {
        pr_warn("camdevice: already opened\n");
        ret = -EBUSY;
        goto out;
    }

    ret = cam_start_capture();
    if (ret) {
        pr_err("camdevice: failed to start capture\n");
        goto out;
    }

    cam_ctx.device_opened = 1;
    pr_info("camdevice: device opened\n");

out:
    mutex_unlock(&cam_ctx.lock);
    return ret;
}

static int cam_release(struct inode *inode, struct file *file)
{
    mutex_lock(&cam_ctx.lock);

    if (cam_ctx.device_opened) {
        cam_stop_capture();
        cam_ctx.device_opened = 0;
        pr_info("camdevice: device closed\n");
    }

    mutex_unlock(&cam_ctx.lock);
    return 0;
}

ssize_t cam_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    char *kbuf;
    size_t frame_len;
    int ret;

    mutex_lock(&cam_ctx.lock);

    if (!cam_ctx.device_opened) {
        ret = v4l2_open_camera(cam_ctx.device_name);
        if (ret < 0) {
            mutex_unlock(&cam_ctx.lock);
            return ret;
        }
        cam_ctx.device_opened = 1;
    }

    ret = v4l2_capture_frame(&kbuf, &frame_len);
    if (ret < 0) {
        pr_err("camdevice: v4l2_capture_frame failed: %d\n", ret);

        mutex_unlock(&cam_ctx.lock);
        return ret;
    }

    if (copy_to_user(buf, kbuf, frame_len)) {
        mutex_unlock(&cam_ctx.lock);
        return -EFAULT;
    }

    mutex_unlock(&cam_ctx.lock);
    return frame_len;
}


const struct file_operations cam_fops = {
    .owner = THIS_MODULE,
    .open = cam_open,
    .release = cam_release,
    .read = cam_read,
};
EXPORT_SYMBOL(cam_fops);

MODULE_LICENSE("GPL");
