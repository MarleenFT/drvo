#include <linux/module.h>
#include <linux/init.h>

MODULE_VERSION("0.1");
MODULE_DESCRIPTION("Test module");
MODULE_AUTHOR("You");
MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
    printk(KERN_ALERT "Hld\n");
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
}

module_init(hello_init);
module_exit(hello_exit);
