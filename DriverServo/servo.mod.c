#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

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

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xab2e3178, "module_layout" },
	{ 0xce47cd8, "cdev_del" },
	{ 0xcd708882, "cdev_init" },
	{ 0xf9a482f9, "msleep" },
	{ 0x91ddacb4, "pwm_apply_state" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0x56470118, "__warn_printk" },
	{ 0xd393bb1a, "device_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xc5850110, "printk" },
	{ 0xd9af1a1d, "device_create" },
	{ 0x124ef924, "pwm_free" },
	{ 0x6b4b2933, "__ioremap" },
	{ 0xcc3bed4e, "cdev_add" },
	{ 0x9f49dcc4, "__stack_chk_fail" },
	{ 0xaf56600a, "arm64_use_ng_mappings" },
	{ 0x570e683b, "class_destroy" },
	{ 0xb742fd7, "simple_strtol" },
	{ 0xe7a89283, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "B28E10D9F2F9A73BCAF17B9");
