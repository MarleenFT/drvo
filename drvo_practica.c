#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/device.h>

/////////////////////////////////////////////////////////////////////////////////////////////

MODULE_VERSION("1");
MODULE_DESCRIPTION("Test module");
MODULE_AUTHOR("Marleen");
MODULE_LICENSE("Dual BSD/GPL");

/////////////////////////////////////////////////////////////////////////////////////////////

static unsigned int var_base_minor = 1;
module_param_named(base_minor, var_base_minor, int, 0664);

static unsigned int var_minor_amount = 1;
module_param_named(minor_amount, var_minor_amount, int, 0664);

/////////////////////////////////////////////////////////////////////////////////////////////

static int      drvo_init(void);
static void     drvo_exit(void);
static ssize_t  drvo_read(struct file*, char*, size_t, loff_t*);
static ssize_t  drvo_write(struct file*, const char*, size_t, loff_t*);
static int      drvo_open(struct inode*, struct file*);
static int      drvo_release(struct inode*, struct file*);

/////////////////////////////////////////////////////////////////////////////////////////////

static struct cdev*    drvo_cdev = NULL;
static dev_t           drvo_first_dev = 0;
static struct class*   drvo_class = NULL;
static struct device*  drvo_device = NULL;


struct file_operations drvo_file_ops = {
    .owner      = THIS_MODULE,
    .read       = drvo_read,
    .write      = drvo_write,
    .open       = drvo_open,
    .release    = drvo_release
};

/////////////////////////////////////////////////////////////////////////////////////////////

static int drvo_init(void)
{
    printk(KERN_WARNING "Driver initialization\n");

    alloc_chrdev_region(&drvo_first_dev, var_base_minor, var_minor_amount, "Drvo_practica");
    drvo_cdev = cdev_alloc();
    drvo_cdev->ops = &drvo_file_ops;
    cdev_add(drvo_cdev, drvo_first_dev, var_minor_amount);

    printk(KERN_INFO "Registered %d device(s) on major number: %d \n", drvo_cdev->count, MAJOR(drvo_cdev->dev));

    drvo_class = class_create(THIS_MODULE, "Drvo_class");
    if(IS_ERR(drvo_class)) {
        cdev_del(drvo_cdev);
        unregister_chrdev_region(drvo_first_dev, var_minor_amount);
        printk(KERN_ALERT "Could not create class, exiting!\n");
        return PTR_ERR(drvo_class);
    }

    drvo_device = device_create(drvo_class, NULL, drvo_first_dev, NULL, "Drvo_device");
    if(IS_ERR(drvo_device)) {
        class_destroy(drvo_class);
        cdev_del(drvo_cdev);
        unregister_chrdev_region(drvo_first_dev, var_minor_amount);
        printk(KERN_ALERT "Could not create device, exiting!\n");
        return PTR_ERR(drvo_class);
    }

    printk(KERN_INFO "Driver successfully initiated.\n");

    return 0;
}

static void drvo_exit(void)
{
    device_destroy(drvo_class, drvo_first_dev);
    class_destroy(drvo_class);
    cdev_del(drvo_cdev);
    unregister_chrdev_region(drvo_first_dev, var_minor_amount);

    printk(KERN_WARNING "Driver shutting down\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////

static ssize_t drvo_read(struct file* pf, char* buf, size_t size, loff_t* lof) {
    printk(KERN_INFO "Reading %ld bytes of data\n", size);
    return 0;
}

static ssize_t drvo_write(struct file* pf, const char* buf, size_t size, loff_t* lof) {
    printk(KERN_INFO "Writing %ld bytes of data\n", size);
    return size;
}

static int drvo_open(struct inode* iNode, struct file* fp) {
    printk(KERN_INFO "Driver opening initiated\n");
    return 0;
}

static int drvo_release(struct inode* iNode, struct file* fp) {
    printk(KERN_INFO "Driver release initiated\n");
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

module_init(drvo_init);
module_exit(drvo_exit);

/////////////////////////////////////////////////////////////////////////////////////////////