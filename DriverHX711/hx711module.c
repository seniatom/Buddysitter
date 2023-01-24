#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/timer.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mm.h>
#include <linux/gpio/consumer.h>
#include <linux/io.h>
//#include <mach/platform.h>

#include "utils.h"

#define DEVICE_NAME "hx7110"
#define CLASS_NAME "hx711Class"

MODULE_LICENSE("GPL");

/* Device variables */
static struct class* hx711Device_class = NULL;
static struct device* hx711Device_device = NULL;
static dev_t hx711Device_majorminor;
static struct cdev c_dev;  // Character device structure


static struct class *s_pDeviceClass;
static struct device *s_pDeviceObject;
struct GpioRegisters *s_pGpioRegisters;

static unsigned int dout_pin = 21;	//branco 
static unsigned int pd_sck_pin = 13;	//amarelo 
static unsigned int t1 = 150; //to be confirmed
static unsigned int t2 = 100; //to be confirmed
static unsigned int t3 = 200; //to be confirmed
static unsigned int t4 = 200; //to be confirmed

static int hx711_wait_for_ready(void)
{
	int i, val;

    /*When  output  data  is  not  ready  for  retrieval,  
     * digital  output  pin  DOUT  is  high.  Serial  clock  input PD_ SCK should be low.  
     */
    SetGPIOOutputValue(s_pGpioRegisters, pd_sck_pin, 0);

	for (i = 0; i < 100; i++) {
		val = GetGPIOValue(s_pGpioRegisters, dout_pin);
		if (!val)
            break;
		/* sleep at least 10 ms */
		msleep(10);
	}
	if (val)
		return -EIO;

	return 0;
}

static int hx711_cycle(void)
{
    ndelay(t1);

    SetGPIOOutputValue(s_pGpioRegisters, pd_sck_pin, 1);

	ndelay(t3);
    int value = GetGPIOValue(s_pGpioRegisters, dout_pin);

    SetGPIOOutputValue(s_pGpioRegisters, pd_sck_pin, 0);

    ndelay(t4);

	/* sample as late as possible */
	return value;
}

//loop
ssize_t hx711_device_read(struct file *pfile, char __user *p_buff,size_t len, loff_t *poffset){
    
	int i, ret;
	unsigned value = 0;
    printk(KERN_NOTICE "#####################");
    int val = hx711_wait_for_ready();

	if (val)
		return -EIO; //some issue with the sensor, what do we do?

	for (i = 0; i < 24; i++) {
		ret = hx711_cycle();
        value <<= 1;
		if (ret)
			value++;
	}
    value ^= 0x800000;

    printk(KERN_NOTICE "exit value %d", value);
    printk(KERN_NOTICE "######################");

	return 0;
}

int hx711_device_close(struct inode *p_inode, struct file * pfile){
	
	pr_alert("%s: calhx711\n",__FUNCTION__);
	pfile->private_data = NULL;
	return 0;
}


int hx711_device_open(struct inode* p_indode, struct file *p_file){

	pr_alert("%s: calhx711\n",__FUNCTION__);
	p_file->private_data = (struct GpioRegisters *) s_pGpioRegisters;
	return 0;
	
}


static struct file_operations hx711Device_fops = {
	.owner = THIS_MODULE,
	.read = hx711_device_read,
	.release = hx711_device_close,
	.open = hx711_device_open,
};

static int __init hx711Module_init(void) {
    int ret;
	struct device *dev_ret;

	pr_alert("%s: calhx711\n",__FUNCTION__);

	if ((ret = alloc_chrdev_region(&hx711Device_majorminor, 0, 1, DEVICE_NAME)) < 0) {
		return ret;
	}

	if (IS_ERR(hx711Device_class = class_create(THIS_MODULE, CLASS_NAME))) {
		unregister_chrdev_region(hx711Device_majorminor, 1);
		return PTR_ERR(hx711Device_class);
	}
	if (IS_ERR(dev_ret = device_create(hx711Device_class, NULL, hx711Device_majorminor, NULL, DEVICE_NAME))) {
		class_destroy(hx711Device_class);
		unregister_chrdev_region(hx711Device_majorminor, 1);
		return PTR_ERR(dev_ret);
	}

	cdev_init(&c_dev, &hx711Device_fops);
	c_dev.owner = THIS_MODULE;
	if ((ret = cdev_add(&c_dev, hx711Device_majorminor, 1)) < 0) {
		printk(KERN_NOTICE "Error %d adding device", ret);
		device_destroy(hx711Device_class, hx711Device_majorminor);
		class_destroy(hx711Device_class);
		unregister_chrdev_region(hx711Device_majorminor, 1);
		return ret;
	}

	s_pGpioRegisters = (struct GpioRegisters *)ioremap(GPIO_BASE, sizeof(struct GpioRegisters));
	//s_pGpioRegisters = (struct GpioRegisters *)ioremap_nocache(GPIO_BASE, sizeof(struct GpioRegisters));
	
	pr_alert("map to virtual adresse: 0x%x\n", (unsigned)s_pGpioRegisters);
	
	SetGPIOFunction(s_pGpioRegisters, pd_sck_pin, 0b001); //Output
	SetGPIOFunction(s_pGpioRegisters, dout_pin, 0); //Input
    SetGPIOOutputValue(s_pGpioRegisters, pd_sck_pin, 0);

	return 0;
}

static void __exit hx711Module_exit(void) {
    pr_alert("%s: calhx711\n",__FUNCTION__);
	
	SetGPIOFunction(s_pGpioRegisters, pd_sck_pin, 0); //Configure the pin as input
	iounmap(s_pGpioRegisters);
	cdev_del(&c_dev);
	device_destroy(hx711Device_class, hx711Device_majorminor);
	class_destroy(hx711Device_class);
	unregister_chrdev_region(hx711Device_majorminor, 1);

}

module_init(hx711Module_init);
module_exit(hx711Module_exit);
