// cam_main.c

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include "cam_defs.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sriramkumar");
MODULE_DESCRIPTION("Kernel camera frame capture via V4L2");

static dev_t dev_num;
static struct class *cam_class;
static struct cdev cam_cdev;

extern const struct file_operations cam_fops; // Defined in cam_device.c

struct cam_device_ctx cam_ctx = {
    .io = IO_METHOD_MMAP,
    .fd = -1,
    .device_name = "/dev/video0",
    .force_format = 1,
    .frame_count = 30,
    .device_opened = 0
};
static int __init cam_module_init(void)
{
    int result;

    mutex_init(&cam_ctx.lock);

    // Allocate a major number dynamically
    result = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (result < 0) {
        pr_err("camdevice: failed to allocate chrdev\n");
        return result;
    }

    pr_info("camdevice: registered major = %d\n", MAJOR(dev_num));

    // Initialize and add character device
    cdev_init(&cam_cdev, &cam_fops);
    cam_cdev.owner = THIS_MODULE;
    result = cdev_add(&cam_cdev, dev_num, 1);
    if (result < 0) {
        unregister_chrdev_region(dev_num, 1);
        pr_err("camdevice: failed to add cdev\n");
        return result;
    }

    // Create device class
    cam_class = class_create(DEVICE_NAME);
    if (IS_ERR(cam_class)) {
        cdev_del(&cam_cdev);
        unregister_chrdev_region(dev_num, 1);
        pr_err("camdevice: failed to create class\n");
        return PTR_ERR(cam_class);
    }

    // Create device node /dev/camdevice
    device_create(cam_class, NULL, dev_num, NULL, DEVICE_NAME);

    pr_info("camdevice: module loaded\n");
    return 0;
}

static void __exit cam_module_exit(void)
{
    cdev_del(&cam_cdev);
    device_destroy(cam_class, dev_num);
    class_destroy(cam_class);
    unregister_chrdev_region(dev_num, 1);
    mutex_destroy(&cam_ctx.lock);

    pr_info("camdevice: module unloaded\n");
}
EXPORT_SYMBOL(cam_ctx);

module_init(cam_module_init);
module_exit(cam_module_exit);
