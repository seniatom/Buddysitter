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
//#include <linux/gpio/consumer.h>
#include <linux/io.h>
//#include <mach/platform.h>
#include <linux/pwm.h>
#include <linux/kernel.h>
#define DEVICE_NAME "servo0"
#define CLASS_NAME "servoClass"

MODULE_LICENSE("GPL");

/* Device variables */
static struct class* servoDevice_class = NULL;
static struct device* servoDevice_device = NULL;
static dev_t servoDevice_majorminor;
static struct cdev c_dev;  // Character device structure

#define BCM2835_CLOCK_BASE              0x101000
#define BCM2835_GPIO_BASE               0x200000
#define BCM2835_GPIO_PWM                0x20C000
#define BCM2835_PERI_BASE               0xFE000000
#define BCM2835_PERI_SIZE               0x01800000
#define BCM2835_GPFSEL0                 0x0000
#define BCM2835_GPIO_FSEL_ALT5 0x02
#define BCM2835_GPIO_FSEL_MASK 0x07
#define BCM2835_PWM_CLOCK_DIVIDER_32 32

/* Defines for PWM, word offsets (ie 4 byte multiples) */
#define BCM2835_PWM_CONTROL 0
#define BCM2835_PWM_STATUS  1
#define BCM2835_PWM_DMAC    2
#define BCM2835_PWM0_RANGE  4
#define BCM2835_PWM0_DATA   5
#define BCM2835_PWM_FIF1    6
#define BCM2835_PWM1_RANGE  8
#define BCM2835_PWM1_DATA   9

#define BCM2835_PWM0_MS_MODE    0x0080  /*!< Run in Mark/Space mode */
#define BCM2835_PWM_CLEAR_FIFO  0x0040  /*!< Clear FIFO */
#define BCM2835_PWM0_USEFIFO    0x0020  /*!< Data from FIFO */
#define BCM2835_PWM0_REVPOLAR   0x0010  /*!< Reverse polarity */
#define BCM2835_PWM0_OFFSTATE   0x0008  /*!< Ouput Off state */
#define BCM2835_PWM0_REPEATFF   0x0004  /*!< Repeat last value if FIFO empty */
#define BCM2835_PWM0_SERIAL     0x0002  /*!< Run in serial mode */
#define BCM2835_PWM0_ENABLE     0x0001  /*!< Channel Enable */

/* Defines for PWM Clock, word offsets (ie 4 byte multiples) */
#define BCM2835_PWMCLK_CNTL     40
#define BCM2835_PWMCLK_DIV      41
#define BCM2835_PWM_PASSWRD     (0x5A << 24)  /*!< Password to enable setting PWM clock */

/* And the register bases within the peripherals block
 */
// volatile uint32_t *bcm2835_gpio        = BCM2835_PERI_BASE + BCM2835_GPIO_PWM/4;
// volatile uint32_t *bcm2835_pwm         = BCM2835_PERI_BASE + BCM2835_GPIO_PWM/4;
// volatile uint32_t *bcm2835_clk         = BCM2835_PERI_BASE + BCM2835_CLOCK_BASE/4;


static struct class *s_pDeviceClass;
static struct device *s_pDeviceObject;

struct pwm_device *pwm0 = NULL;

void bcm2835_peri_set_bits(volatile uint32_t* paddr, uint32_t value, uint32_t mask)
{
    uint32_t v = *paddr;
    v = (v & ~mask) | (value & mask);
	*paddr = v;
}

ssize_t servoDevice_device_write(struct file *pfile, const char __user *pbuff, size_t len, loff_t *off){
    volatile uint32_t *bcm2835_peripherals = (uint32_t *)ioremap(BCM2835_PERI_BASE, BCM2835_PERI_SIZE);
	volatile uint32_t *bcm2835_pwm         = bcm2835_peripherals + BCM2835_GPIO_PWM/4;
	int to_copy, not_copied, delta;
	char  value[5];

	if(len < 0)
		return 0;

	if(copy_from_user(&value, pbuff, len))
		return -EFAULT;

	/* Copy data to user */
	//not_copied = copy_from_user(&value, pbuff, sizeof(value));
	// memcpy(pbuff, &value, len);
	// kstrtol_from_user(pbuff, len, 10, &value);

	/* Set PWM on time */
	*(bcm2835_pwm + BCM2835_PWM0_DATA) = simple_strtol(value, NULL, 10);

	// delta = value;
	*off += len;

	return len;
}

int servoDevice_device_close(struct inode *p_inode, struct file * pfile){
	
	pr_alert("%s: calservo\n",__FUNCTION__);
	pfile->private_data = NULL;
	return 0;
}


int servoDevice_device_open(struct inode* p_indode, struct file *p_file){
	printk("dev_nr - open was called!\n");
	
	return 0;
	
}


static struct file_operations servoDevice_fops = {
	.owner = THIS_MODULE,
	.write = servoDevice_device_write,
	.release = servoDevice_device_close,
	.open = servoDevice_device_open,
};

static int __init servoModule_init(void) {
    int ret;
	struct device *dev_ret;

	pr_alert("%s: calservo\n",__FUNCTION__);

	if ((ret = alloc_chrdev_region(&servoDevice_majorminor, 0, 1, DEVICE_NAME)) < 0) {
		return ret;
	}

	if (IS_ERR(servoDevice_class = class_create(THIS_MODULE, CLASS_NAME))) {
		unregister_chrdev_region(servoDevice_majorminor, 1);
		return PTR_ERR(servoDevice_class);
	}
	if (IS_ERR(dev_ret = device_create(servoDevice_class, NULL, servoDevice_majorminor, NULL, DEVICE_NAME))) {
		class_destroy(servoDevice_class);
		unregister_chrdev_region(servoDevice_majorminor, 1);
		return PTR_ERR(dev_ret);
	}

	cdev_init(&c_dev, &servoDevice_fops);
	c_dev.owner = THIS_MODULE;
	if ((ret = cdev_add(&c_dev, servoDevice_majorminor, 1)) < 0) {
		printk(KERN_NOTICE "Error %d adding device", ret);
		device_destroy(servoDevice_class, servoDevice_majorminor);
		class_destroy(servoDevice_class);
		unregister_chrdev_region(servoDevice_majorminor, 1);
		return ret;
	}

	volatile uint32_t *bcm2835_peripherals = (uint32_t *)ioremap(BCM2835_PERI_BASE, BCM2835_PERI_SIZE);
	volatile uint32_t *bcm2835_gpio        = bcm2835_peripherals + BCM2835_GPIO_BASE/4;
	volatile uint32_t *bcm2835_pwm         = bcm2835_peripherals + BCM2835_GPIO_PWM/4;
	volatile uint32_t *bcm2835_clk         = bcm2835_peripherals + BCM2835_CLOCK_BASE/4;
	// printk(KERN_NOTICE "******** bcm2835_peripherals %x", bcm2835_peripherals);
	// printk(KERN_NOTICE "******** bcm2835_gpio %x", bcm2835_gpio);
	// printk(KERN_NOTICE "******** bcm2835_pwm %x", bcm2835_pwm);
	// printk(KERN_NOTICE "******** bcm2835_clk %x", bcm2835_clk);

    /* Set Pin as ALT5 (PWM)*/
    volatile uint32_t* paddr = bcm2835_gpio + BCM2835_GPFSEL0/4 + (18/10);
    uint8_t   shift = (18 % 10) * 3;
    uint32_t  mask = BCM2835_GPIO_FSEL_MASK << shift;
    uint32_t  value = BCM2835_GPIO_FSEL_ALT5 << shift;
	
    bcm2835_peri_set_bits(paddr, value, mask);

	/* Set clock divisor*/
	//According to the manual to avoid lockups and glitches we should not set this while busy
	//Maybe we should stop the PWM clock and wait for not busy before doing this ??
	*(bcm2835_clk + BCM2835_PWMCLK_CNTL) = BCM2835_PWM_PASSWRD | 0x01;
	msleep(110);
	while((*(bcm2835_clk + BCM2835_PWMCLK_CNTL) & 0x80) != 0)
	msleep(1);
	
	*(bcm2835_clk + BCM2835_PWMCLK_DIV) = (BCM2835_PWM_PASSWRD | (BCM2835_PWM_CLOCK_DIVIDER_32 << 12));
	*(bcm2835_clk + BCM2835_PWMCLK_CNTL) = (BCM2835_PWM_PASSWRD | (0x11));  /* Source=osc and enable */

	/* Set PWM channel 0 and PWM mode*/
	
	uint32_t control = *(bcm2835_pwm + BCM2835_PWM_CONTROL);
	control |= BCM2835_PWM0_MS_MODE;
	control |= BCM2835_PWM0_ENABLE;
	*(bcm2835_pwm + BCM2835_PWM_CONTROL) = control;

	/* Set PWM Range*/
	*(bcm2835_pwm + BCM2835_PWM0_RANGE) = 1024;

	/* Set PWM default value */
	*(bcm2835_pwm + BCM2835_PWM0_DATA) = 1024;

   //initializing pwm

    // pwm0 = pwm_request(0, "my-pwm");
	// if(IS_ERR(pwm0 == NULL)) {
	// 	printk("Could not get PWM0!\n"); 
	// 	return -1;
	// }

	// pwm_config(pwm0, 1000000000*0, 1000000000);
	// pwm_enable(pwm0);

	return 0;
}

static void __exit servoModule_exit(void) {
    pr_alert("%s: calservo\n",__FUNCTION__);

	pwm_disable(pwm0);
	pwm_free(pwm0);
	cdev_del(&c_dev);
	device_destroy(servoDevice_class, servoDevice_majorminor);
	class_destroy(servoDevice_class);
	unregister_chrdev_region(servoDevice_majorminor, 1);
	
}

module_init(servoModule_init);
module_exit(servoModule_exit);
