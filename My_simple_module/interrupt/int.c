#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <asm/current.h>
#include <linux/sched.h>

#define minor_first 0
#define minor_count 256
#define majordom 0
#define name "chrdev"
#define BUFFER_SIZE 256
#define KB_IRQ 1
#define KB_NAME "kb_int"

static unsigned long kb_cnt_int = 0;

struct cdev dev_ch;
wait_queue_head_t queue;

static irqreturn_t kb_int(int irq, void *dev_id)
{
	kb_cnt_int++;
	wake_up_interruptible(&queue);
	
	return IRQ_HANDLED;
}

static int reg_irq(void)
{
	return request_irq(
				KB_IRQ,
				kb_int,
				IRQF_SHARED,
				KB_NAME,
				(void *)kb_int
				);
}

static ssize_t dev_read
(struct file *file, char *buff, size_t size, loff_t *off) {
	wait_event_interruptible(queue, (kb_cnt_int > 0));
	char *str;
	int length;

	if (*off != 0)
	    return 0;

	pr_info("Int %lu\n", kb_cnt_int);

	length = snprintf(NULL, 0, "%lu\n", kb_cnt_int);
	length++;

	str = kmalloc(length, GFP_KERNEL);
	if (!str)
		return 0;

	snprintf(str, length, "%lu\n", kb_cnt_int);
	copy_to_user(buff, str, length);
	*off += length;

	kfree(str);

	return length;
}

static const struct file_operations dev_fops = { 
   .owner = THIS_MODULE,  
   .read  = dev_read, 
}; 

static int __init chd_init(void) {
	   dev_t dev; /*для определения мажора и минора*/
       	   int res_of_reg, major;
	   res_of_reg = -1;
	   major = majordom;
	   if(major) { 
		dev = MKDEV(major,minor_first);
		res_of_reg = register_chrdev_region(dev,minor_count,name);
	   }
	   else {
		res_of_reg = alloc_chrdev_region(&dev, minor_first, minor_count, name);
		major = MAJOR(dev); /*Динамическое выделение */
	   }
	   if (res_of_reg < 0) {
		printk(KERN_ERR "ERROR:kernel doesn't register this device\n");
		goto err;
	   }	

	   cdev_init(&dev_ch ,&dev_fops);
	   dev_ch.owner = THIS_MODULE;
	   
	   res_of_reg = cdev_add(&dev_ch, dev, minor_count);
	   if (res_of_reg < 0) {
		printk(KERN_ERR "ERROR:kernel doesn't register this chdevice\n");
		goto err;
	   }
	   if (reg_irq() != 0)
		return -1;
	   init_waitqueue_head(&queue);
err:
	   printk(KERN_INFO "succesful init!\n");
	   return res_of_reg;
}

static void __exit my_device_exit(void)
{
    cdev_del(&dev_ch);
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(chd_init);
module_exit(my_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yarik");
MODULE_DESCRIPTION("Interrupt events");
	
