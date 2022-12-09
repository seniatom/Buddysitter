#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/timer.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mm.h>

#include <linux/io.h>
//#include <mach/platform.h>

#include "utils.h"



#define DEVICE_NAME "servo0"
#define CLASS_NAME "servoClass"




MODULE_LICENSE("GPL");

/* Device variables */
static struct class* servoDevice_class = NULL;
static struct device* servoDevice_device = NULL;
static dev_t servoDevice_majorminor;
static struct cdev c_dev;  // Character device structure


static struct class *s_pDeviceClass;
static struct device *s_pDeviceObject;
struct GpioRegisters *s_pGpioRegisters;

static const int servoGpioPin = GPIO_18; //<-!!!!!!!!!!!!! change pin?

int servo_open(struct inode* p_indode, struct file *p_file){
	
}

int servo_close(struct inode *p_inode, struct file * pfile){
	
}

ssize_t servo_read(struct file *pfile, char __user *p_buff,size_t len, loff_t *poffset){
	uint8_t gpio_state = 0;
  
  //reading GPIO value
	gpio_state = gpio_get_value(GPIO_21);
  
  //write to user
	len = 1;
	if( copy_to_user(p_buf, &gpio_state, len) > 0) {
		pr_err("ERROR: Not all the bytes have been copied to user\n");
	}
  
	pr_info("Read function : GPIO_21 = %d \n", gpio_state);
  
	return 0;
}

ssize_t servo_write(struct file *pfile, const char __user *pbuff, size_t len, loff_t *off) { 
	uint8_t rec_buf[10] = {0};
  
	if( copy_from_user( rec_buf, buf, len ) > 0) {
		pr_err("ERROR: Not all the bytes have been copied from user\n");
	}
  
	pr_info("Write Function : GPIO_18 Set = %c\n", rec_buf[0]);
  
	if (rec_buf[0]=='1') {
    //set the GPIO value to HIGH
		gpio_set_value(servoGpioPin, 1);
	} else if (rec_buf[0]=='0') {
    //set the GPIO value to LOW
		gpio_set_value(servoGpioPin, 0);
	} else {
		pr_err("Unknown command : Please provide either 1 or 0 \n");
	}
  
  	return len;
}



static struct file_operations servo_fops = {
	.owner = THIS_MODULE,
	.write = servo_write,
	.read = servo_read,	
	.open = servo_open,
    .release = servo_close,
};

static int __init servoModule_init(void) {
	int ret;
	struct device *dev_ret;

	pr_alert("%s: called\n",__FUNCTION__);

	if ((ret = alloc_chrdev_region(&ledDevice_majorminor, 0, 1, DEVICE_NAME)) < 0) {
		return ret;
	}

	if (IS_ERR(ledDevice_class = class_create(THIS_MODULE, CLASS_NAME))) {
		unregister_chrdev_region(ledDevice_majorminor, 1);
		return PTR_ERR(ledDevice_class);
	}
	if (IS_ERR(dev_ret = device_create(ledDevice_class, NULL, ledDevice_majorminor, NULL, DEVICE_NAME))) {
		class_destroy(ledDevice_class);
		unregister_chrdev_region(ledDevice_majorminor, 1);
		return PTR_ERR(dev_ret);
	}

	cdev_init(&c_dev, &ledDevice_fops);
	c_dev.owner = THIS_MODULE;
	if ((ret = cdev_add(&c_dev, ledDevice_majorminor, 1)) < 0) {
		printk(KERN_NOTICE "Error %d adding device", ret);
		device_destroy(ledDevice_class, ledDevice_majorminor);
		class_destroy(ledDevice_class);
		unregister_chrdev_region(ledDevice_majorminor, 1);
		return ret;
	}


	s_pGpioRegisters = (struct GpioRegisters *)ioremap(GPIO_BASE, sizeof(struct GpioRegisters));
	//s_pGpioRegisters = (struct GpioRegisters *)ioremap_nocache(GPIO_BASE, sizeof(struct GpioRegisters));
	
	pr_alert("map to virtual adresse: 0x%x\n", (unsigned)s_pGpioRegisters);
	
	SetGPIOFunction(s_pGpioRegisters, LedGpioPin, 0b001); //Output

	return 0;
}

static void __exit servoModule_exit(void) {
	
	pr_alert("%s: called\n",__FUNCTION__);
	
	SetGPIOFunction(s_pGpioRegisters, servoGpioPin, 0); //Configure the pin as input
	iounmap(s_pGpioRegisters);
	cdev_del(&c_dev);
	device_destroy(servoDevice_class, servoDevice_majorminor);
	class_destroy(servoDevice_class);
	unregister_chrdev_region(servoDevice_majorminor, 1);
}

module_init(servoModule_init);
module_exit(servoModule_exit);
