#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

KSYMTAB_DATA(cam_ctx, "", "");
KSYMTAB_DATA(cam_fops, "", "");
KSYMTAB_FUNC(cam_start_capture, "", "");
KSYMTAB_FUNC(cam_read_frame, "", "");
KSYMTAB_FUNC(cam_stop_capture, "", "");
KSYMTAB_FUNC(v4l2_open_camera, "", "");
KSYMTAB_FUNC(v4l2_capture_frame, "", "");
KSYMTAB_FUNC(v4l2_requeue_frame, "", "");
KSYMTAB_FUNC(v4l2_close_camera, "", "");

SYMBOL_CRC(cam_ctx, 0xd76a580b, "");
SYMBOL_CRC(cam_fops, 0xa5605e5d, "");
SYMBOL_CRC(cam_start_capture, 0x8abc883a, "");
SYMBOL_CRC(cam_read_frame, 0x6f38d358, "");
SYMBOL_CRC(cam_stop_capture, 0xe553c688, "");
SYMBOL_CRC(v4l2_open_camera, 0x20c3fff1, "");
SYMBOL_CRC(v4l2_capture_frame, 0xa5047053, "");
SYMBOL_CRC(v4l2_requeue_frame, 0x3aab95a0, "");
SYMBOL_CRC(v4l2_close_camera, 0x400c1b65, "");

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xccbc60be, "filp_open" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x378b3561, "class_destroy" },
	{ 0x37a0cba, "kfree" },
	{ 0x122c3a7e, "_printk" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x282d49c2, "cdev_add" },
	{ 0xd4ae550c, "device_create" },
	{ 0xdde25e1, "class_create" },
	{ 0x4dfa8d4b, "mutex_lock" },
	{ 0x9ed12e20, "kmalloc_large" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0xd3e4113, "kernel_read" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0xa8798d27, "filp_close" },
	{ 0xcf435ce7, "device_destroy" },
	{ 0xfde7dc9b, "cdev_init" },
	{ 0x6b8f7feb, "cdev_del" },
	{ 0x773354b7, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "0D17513AF3EF4FC654676B8");
