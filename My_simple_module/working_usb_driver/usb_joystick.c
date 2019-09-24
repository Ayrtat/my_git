/* This module is workable, but hasn't been finished yet. 
   It's possible to read data from the input (you'll see
   some consequence of byte), but it's neccessary to 
   transoform these bytes to readable buttons on using
   input_dev */
   
   #include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/module.h>
#include <linux/usb/input.h>
#include <linux/usb.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define my_js_vendor_id 0x0079
#define my_js_product_id 0x0006


#define BULK_EP_OUT 0x01
#define BULK_EP_IN 0x81
#define MAX_PKT_SIZE 32
#define MIN(a,b) (((a) <= (b)) ? (a) : (b))


static struct usb_device_id table[] =
{
    { USB_DEVICE(my_js_vendor_id, my_js_product_id) },
    {} /* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, table);

static struct devicec {
	struct usb_device *udev;
	struct usb_interface *intf;
	struct urb *irq_in; /*urb handler*/;
	struct usb_class_driver class;

	unsigned char idata[MAX_PKT_SIZE];
	dma_addr_t idata_dma;

	char phys[64]; 
} *j;

static int p_open(struct inode *i, struct file *f)
{
    printk(KERN_ERR "Open has been opened\n");
    return 0;
}

static int p_close(struct inode *i, struct file *f)
{
    printk(KERN_ERR "Close has been closed\n");
    return 0;
}

static ssize_t read(struct file *f, char __user *buf, size_t cnt, loff_t *off) {
	int retval;
	int read_cnt;

	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;

    	iface_desc = j->intf->cur_altsetting;
	endpoint = &iface_desc->endpoint[0].desc;

	memset(j->idata,'\0',sizeof(j->idata));
	
	printk(KERN_ERR "READ:\n");
	
	memset(j->idata,'\0',sizeof(j->idata));
	retval = usb_bulk_msg(j->udev, usb_rcvbulkpipe(j->udev, BULK_EP_IN),
        		      j->idata, MAX_PKT_SIZE, &read_cnt, 30000);//endpoint->bInterval);

	printk(KERN_ERR "%s", j->idata);
	j->idata[31] = '\n';	

	if (retval)
	    {
		printk(KERN_ERR "Bulk message returned %d\n", retval);
		return retval;
	    }


	if(read_cnt > 1) {
		if (copy_to_user(buf, j->idata, MIN(cnt, read_cnt)))
		    {
			return -EFAULT;
		    }

	}
	else {
		if (copy_to_user(buf, "nothing\n", sizeof("nothing\n") ))
		    {
			return -EFAULT;
		    }

	}
	 return MIN(cnt, read_cnt);

}

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = p_open,
    .release = p_close,
    .read = read,
};


/*static void irq_hh(struct urb *urb)
{
	struct devicec *j = urb->context;//В какую структура из всех процессов дб заполнена после приёма данных???
        char *data = j->idata;l
	printk(KERN_INFO "irq_hh: %s", data);
}*/

static int probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    struct usb_device *udev; //!
    struct usb_endpoint_descriptor *ep_irq_in, *endpoint;	
    struct usb_host_interface *iface_desc;

    iface_desc = intf->cur_altsetting;

    int ep_irq_in_idx, error, i;
    printk(KERN_INFO "probe...%X",id->idVendor);
    j = kzalloc(sizeof(struct devicec), GFP_KERNEL);
    udev = interface_to_usbdev(intf);
    j->irq_in = usb_alloc_urb(0, GFP_KERNEL); 
    //--------------------------------------
    j->udev = udev;
    j->intf = intf;
    //--------------------------------------
    usb_make_path(udev, j->phys, sizeof(j->phys));
    strlcat(j->phys, "/input101", sizeof(j->phys));
    printk(KERN_INFO "%s",j->phys);
   
    ep_irq_in_idx = 0;
    ep_irq_in = &intf->cur_altsetting->endpoint[ep_irq_in_idx].desc;

    /*usb_fill_int_urb(j->irq_in, udev,
		     usb_rcvintpipe(udev, ep_irq_in->bEndpointAddress), //!
		     j->idata, 32, irq_hh,
		     j, ep_irq_in->bInterval); */
	
    j->irq_in->transfer_dma = j->idata_dma;
    j->irq_in->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

    j->class.name = "usb/driver#1";
    j->class.fops = &fops;

   //  error = usb_submit_urb(j->irq_in, GFP_KERNEL);
    // printk(KERN_INFO "%d",error);
   
     if ((i = usb_register_dev(intf, &(j->class) )) < 0)
	    {
		/* Something prevented us from registering this driver */
		printk(KERN_ERR "Not able to get a minor for this device.");
	    }
    else
	    {
		printk(KERN_INFO "Minor obtained: %d\n",intf->minor);
	    }

     printk(KERN_ERR "%s", udev->devpath);

     for (i = 0; i < iface_desc->desc.bNumEndpoints; i++)
    {
        endpoint = &iface_desc->endpoint[i].desc;

        printk(KERN_INFO "ED[%d]->bEndpointAddress: 0x%02X\n",
                i, endpoint->bEndpointAddress);
        printk(KERN_INFO "ED[%d]->bmAttributes: 0x%02X\n",
                i, endpoint->bmAttributes);
        printk(KERN_INFO "ED[%d]->wMaxPacketSize: 0x%04X (%d)\n",
                i, endpoint->wMaxPacketSize,
                endpoint->wMaxPacketSize);
    }


    return 0;
}

static void disconnect(struct usb_interface *intf)
{
    
    struct devicec *j = usb_get_intfdata(intf);
    printk(KERN_INFO "GOG...\n");

    usb_free_urb(j->irq_in);
    usb_free_coherent(j->udev, 32, j->idata, j->idata_dma);

    kfree(j);
    
    usb_set_intfdata(intf, NULL);
}

static struct usb_driver driver =
{
    .name = "Driver#1",
    .id_table = table,
    .probe = probe,
    .disconnect = disconnect,
};

static int __init jinit(void)
{
    printk(KERN_INFO "driver is installing...\n");
    return usb_register(&driver); 
}
 
static void __exit jexit(void)
{
    usb_deregister(&driver);
}
 
module_init(jinit);
module_exit(jexit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yarik");
MODULE_DESCRIPTION("USB Pen Registration Driver");
