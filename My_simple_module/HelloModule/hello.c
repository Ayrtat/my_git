#include <linux/init.h>
#include <linux/module.h>
#include <moduleparam.h>

MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
    printk(KERN_ALERT "Hello, world\n");
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

static char *whom = "world";
static int  howmany = 1;
/*Параметр переменной, ее тип и маска разрешений*/
module_param(howmany,int,S_IRUGO);
module_param(whom,charp,S_IRUGO);

module_init(hello_init);
module_exit(hello_exit);
