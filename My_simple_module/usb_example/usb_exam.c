#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#define my_js_vendor_id 0x0079
#define my_js_product_id 0x0006

/*Устройство - Конфигурация - Интерфейс - Конечная точка: ист/пер
  С интерфейсом связан драйвер устройства*/

static int js_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    printk(KERN_INFO "js drive (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
    return 0;
}

static void js_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "js drive removed\n");
}

static struct usb_device_id js_table[] =
{
    { USB_DEVICE(my_js_vendor_id, my_js_product_id) },
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, js_table);

static struct usb_driver js_driver =
{
    .name = "js_driver",
    .id_table = js_table,
    .probe = js_probe,
    .disconnect = js_disconnect,
};

static int __init js_init(void)
{
    printk(KERN_INFO "js drive is installing...\n");
    return usb_register(&js_driver);
}
 
static void __exit js_exit(void)
{
    usb_deregister(&js_driver);
}
 
module_init(js_init);
module_exit(js_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yarik");
MODULE_DESCRIPTION("USB Pen Registration Driver");

