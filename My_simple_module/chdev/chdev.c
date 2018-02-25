#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/kernel.h>	/* printk(), min() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/proc_fs.h>
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/fcntl.h>
#include <linux/poll.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/seq_file.h>

#define minor_first 0
#define minor_count 256
#define majordom 0
#define name "my_first_chrdev"
#define BUFFER_SIZE 256

/*
	*Circle buffer: 
	*head,
	*tail,
	*length, 
	*buffer
*/

struct buffer {
	int head;
	int tail;
	int length;
	char * buf;
};


struct my_fdevice {
  /*In these queues will be the processes 
   of reading and writing, which fell asleep
   due to some events*/
  wait_queue_head_t inq, outq;
  struct semaphore sem;
  struct cdev cdev; /*The kernel knows that this is a character device*/
  struct buffer * cbuf; /*Circle buffer*/
  int open;
};

static struct my_fdevice mfd; /*My device*/

static int dev_open(struct inode * inode, struct file * filp) { 
	struct my_fdevice *dvc;
	/*returns a structure that contains the cdev structure*/
	dvc = container_of(inode->i_cdev, struct my_fdevice, cdev);
	mfd.open++;
	printk(KERN_ALERT "OPEN:%d\n",mfd.open);
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY)
		printk ("The file is opened only for writing\n");
	if ( (filp->f_flags & O_ACCMODE) == O_RDONLY)
		printk ("The file is opened only for reading\n");
	printk(KERN_INFO "The process is \"%s\" (pid %i)\n",
                current->comm, current->pid);
	return 0;
}

static int dev_release(struct inode * inode, struct file * filp) {
	mfd.open--;
	return 0;
}

static int data_size = -2;
static int for_read_size = -2;

ssize_t dev_write(struct file *filp, const char __user *user_buf, size_t count,
	 loff_t *f_pos) {
	for_read_size = count; /*How many should be read*/
	data_size = count;     /*How many should be write*/
	struct buffer * pBuf;
	struct my_fdevice * device = &mfd;
	char * elem;	    
	elem = kmalloc(sizeof(char), GFP_KERNEL);
	pBuf = device->cbuf;
	/*The writer tries to take the semaphore. 
	  In case of failure, the scheduler places 
	  it in the queue of pending processes*/
	if (down_interruptible(&device->sem))
		{
			printk(KERN_INFO "WRITING: failed to take semaphore\n");
			return -ERESTARTSYS;
		}
	else printk(KERN_INFO "WRITING: the semaphore from me\n");
	while (data_size > 0) {
		if(pBuf->length < BUFFER_SIZE)
		{
			if (copy_from_user(elem, user_buf, 1)) {
				up (&device->sem);
				return -EFAULT;
			}
			user_buf++;
			pBuf->buf[pBuf->tail] = *elem;
			pBuf->length++;
			if(pBuf->tail == (BUFFER_SIZE - 1)) {
				pBuf->tail = 0;
			}
			else 
				pBuf->tail++;
			data_size--;
		}
		/*The buffer is clogged.
		  To fall asleep, to give control to the reader, 
		  even if the reader is not, then he will sleep until 
		  someone opens the reading and free 
		  up the space in the buffer*/
		else {
		/*Когда кто-то пытается расшевелить все процессы из inq 
		  (в нашем случае чтец), то первый процесс из этой очереди
		  который получит управление, должен проверить условие, если
		  оно не выполнено, то оно опять будет спать дальше*/
			up(&device->sem);
			wake_up_interruptible(&device->outq);
			if (wait_event_interruptible(device->inq, (pBuf->length < BUFFER_SIZE)))
				return -ERESTARTSYS;
			if(down_interruptible(&device->sem))
				return -ERESTARTSYS;
		}
	}
	wake_up_interruptible(&device->outq);
	/*Планировщик даст квант времени просыпающемуся*/
	up(&device->sem);
	printk (KERN_ALERT "\nWRITING: end\n");
	kfree(elem);
	return count;		
}

ssize_t dev_read(struct file *filp, char __user *user_buf, size_t count, 
		loff_t *f_pos) {
	struct buffer * pBuf;
	struct my_fdevice * device = &mfd;
	char * elem;
	elem = kmalloc(sizeof(char), GFP_KERNEL);
	pBuf = device->cbuf;
	if (down_interruptible(&device->sem)){
		printk(KERN_INFO "READING: failed to take semaphore\n");	
		return -ERESTARTSYS;
	} 
	else printk(KERN_INFO "READING: the semaphore from me\n");
	do  {
		if(for_read_size <= 0) {
		   printk (KERN_ALERT "Nothing to read\n");
		   up(&device->sem);
		   wake_up_interruptible(&device->inq);
		   if(wait_event_interruptible(device->outq, for_read_size > 0))
			return -ERESTARTSYS;	
		   if (down_interruptible(&device->sem)){
			printk(KERN_INFO "READING: failed to take semaphore\n");	
			return -ERESTARTSYS;
		    }  
		}	
		if(pBuf->length != 0) {
			*elem = pBuf->buf[pBuf->head];
			if (copy_to_user(user_buf, elem, 1)) {
				up (&device->sem);
				return -EFAULT;
			}
			user_buf++;
			pBuf->length--;
			if(pBuf->head==(BUFFER_SIZE-1))
           			 pBuf->head=0;
        		else
            			 pBuf->head++;
			for_read_size--; 
		}
		else {
			up(&device->sem);
			wake_up_interruptible(&device->inq);
			if (wait_event_interruptible(device->outq, (pBuf->length != 0)))
				return -ERESTARTSYS;
			if(down_interruptible(&device->sem))
				return -ERESTARTSYS;
		}
	}
	while (for_read_size > 0);
	up(&device->sem);
	wake_up_interruptible(&device->inq);
	kfree(elem);
	printk (KERN_ALERT "READING: end\n");
	return 0;	
}

static const struct file_operations dev_fops = { 
   .owner = THIS_MODULE, 
   .open = dev_open,  
   .read  = dev_read, 
   .write = dev_write,
   .release = dev_release,
}; 

static int __init chd_init(void) {
   dev_t dev;
   struct my_fdevice * device = &mfd;
   int res_of_reg, major;
   res_of_reg = -1;
   device->open = 0;
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
   cdev_init(&(device->cdev),&dev_fops);

   (device->cdev).owner = THIS_MODULE;
   (device->cdev).ops = &dev_fops;
   res_of_reg = cdev_add(&(device->cdev), dev, minor_count);
   if (res_of_reg < 0) {
	printk(KERN_ERR "ERROR:kernel doesn't register this chdevice\n");
	goto err;
   }
   sema_init(&(device->sem),1);
   device->cbuf = kmalloc(sizeof(struct buffer), GFP_KERNEL);
   if(!(device->cbuf)) {
	res_of_reg = -1;
	printk(KERN_ERR "ERROR: struct type buffer\n");
	goto err;
   }
   (device->cbuf)->buf = kmalloc(BUFFER_SIZE * sizeof(char), GFP_KERNEL);
   if(!((device->cbuf)->buf)) {
	res_of_reg = -1;
	printk(KERN_ERR "ERROR: char buffer (struct)\n");
	goto err;
   }
   (device->cbuf)->head = 0;
   (device->cbuf)->tail = 0;
   (device->cbuf)->length = 0;
   init_waitqueue_head(&device->inq);
   init_waitqueue_head(&device->outq);
err:
   return res_of_reg;
}

static void __exit my_device_exit(void)
{
    struct my_fdevice * device = &mfd;
    kfree(&(device->cbuf->buf));
    kfree(&(device->cbuf));
    cdev_del(&(device->cdev));
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(chd_init);
module_exit(my_device_exit);

