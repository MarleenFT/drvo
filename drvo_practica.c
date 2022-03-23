#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

/////////////////////////////////////////////////////////////////////////////////////////////

MODULE_VERSION("1");
MODULE_DESCRIPTION("Test module");
MODULE_AUTHOR("Marleen");
MODULE_LICENSE("Dual BSD/GPL");

/////////////////////////////////////////////////////////////////////////////////////////////

#define DRVO_MAX_BUFFER_SIZE 255

/////////////////////////////////////////////////////////////////////////////////////////////

static unsigned int var_base_minor = 1;
module_param_named(base_minor, var_base_minor, int, 0664);

static unsigned int var_minor_amount = 1;
module_param_named(minor_amount, var_minor_amount, int, 0664);

/////////////////////////////////////////////////////////////////////////////////////////////

static int      drvo_init(void);
static void     drvo_exit(void);
static ssize_t  drvo_read(struct file *, char *, size_t, loff_t *);
static ssize_t  drvo_write(struct file *, const char *, size_t, loff_t *);
static loff_t   drvo_llseek(struct file *, loff_t, int);
static long     drvo_ioctl(struct file *, unsigned int, unsigned long);
static int      drvo_open(struct inode *, struct file *);
static int      drvo_release(struct inode *, struct file *);

/////////////////////////////////////////////////////////////////////////////////////////////

static struct cdev       drvo_cdev;
              dev_t      drvo_first_dev = 0;
static        int        drvo_major_number = 0;
static struct class     *drvo_class = NULL;
static struct device    *drvo_device = NULL;

struct file_operations drvo_file_ops = {
    .owner = THIS_MODULE,
    .read = drvo_read,
    .write = drvo_write,
    .open = drvo_open,
    .release = drvo_release,
    .llseek = drvo_llseek,
    .unlocked_ioctl = drvo_ioctl
};

/////////////////////////////////////////////////////////////////////////////////////////////

static int drvo_open_counter = 0;

/////////////////////////////////////////////////////////////////////////////////////////////

static char *tty_devnode(struct device *dev, umode_t *mode)
{
    if (!mode)
        return NULL;
    if (dev->devt == MKDEV(drvo_major_number, 0) ||
        dev->devt == MKDEV(drvo_major_number, 2))
        *mode = 0666;
    return NULL;
}

static int my_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

static int drvo_init(void)
{
    printk(KERN_WARNING "Driver initialization\n");

    alloc_chrdev_region(&drvo_first_dev, var_base_minor, var_minor_amount, "drvo_practica");
    drvo_major_number = MAJOR(drvo_first_dev);

    cdev_init(&drvo_cdev, &drvo_file_ops); 
    cdev_add(&drvo_cdev, drvo_first_dev, var_minor_amount);

    printk(KERN_INFO "Registered %d device(s) on major number: %d \n", drvo_cdev.count, drvo_major_number);

    drvo_class = class_create(THIS_MODULE, "drvo_class");
    if (IS_ERR(drvo_class))
    {
        cdev_del(&drvo_cdev);
        unregister_chrdev_region(drvo_first_dev, var_minor_amount);
        printk(KERN_ALERT "Could not create class, exiting!\n");
        return PTR_ERR(drvo_class);
    }
    drvo_class->dev_uevent = my_dev_uevent;
    drvo_class->devnode = tty_devnode;

    drvo_device = device_create(drvo_class, NULL, drvo_first_dev, NULL, "drvo_device");
    if (IS_ERR(drvo_device))
    {
        class_destroy(drvo_class);
        cdev_del(&drvo_cdev);
        unregister_chrdev_region(drvo_first_dev, var_minor_amount);
        printk(KERN_ALERT "Could not create device, exiting!\n");
        return PTR_ERR(drvo_device);
    }

    printk(KERN_INFO "Driver successfully initiated.\n");

    return 0;
}

static void drvo_exit(void)
{
    device_destroy(drvo_class, drvo_first_dev);
    class_destroy(drvo_class);
    cdev_del(&drvo_cdev);
    unregister_chrdev_region(drvo_first_dev, var_minor_amount);

    printk(KERN_WARNING "Driver shutting down\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////

static ssize_t drvo_read(struct file *pf, char __user *buf, size_t size, loff_t *lof)
{
    int result = 0;
    int bytes_read = 0;

    printk(KERN_INFO "Reading from drvo initiated\n");

    if (*lof > DRVO_MAX_BUFFER_SIZE || *lof < 0)
    {
        printk(KERN_ALERT "Read position out of bounds, exiting\n");
        return 0;
    }

    bytes_read = ((size + *lof) > DRVO_MAX_BUFFER_SIZE) ? (DRVO_MAX_BUFFER_SIZE - *lof) : size;

    result = copy_to_user(buf, pf->private_data + *lof, bytes_read);
    if (result)
    {
        printk(KERN_ALERT "Could not read %d bytes, exiting\n", result);
        return -EINVAL;
    }

    *lof += bytes_read;

    printk(KERN_INFO "Reading %d bytes of data\n", bytes_read);
    return bytes_read;
}

static ssize_t drvo_write(struct file *pf, const char __user *buf, size_t size, loff_t *lof)
{
    int result = 0;
    int bytes_written = size;

    printk(KERN_INFO "Writing to drvo initiated\n");

    if (*lof + bytes_written > DRVO_MAX_BUFFER_SIZE)
    {
        bytes_written = DRVO_MAX_BUFFER_SIZE - *lof;
    }

    result = copy_from_user(pf->private_data + *lof, buf, bytes_written);
    if (result)
    {
        printk(KERN_ALERT "Could not write %d bytes, exiting\n", result);
        return -EINVAL;
    }

    *lof += bytes_written;

    printk(KERN_INFO "Written %d bytes of data\n", bytes_written);

    return bytes_written;
}

static loff_t drvo_llseek(struct file *fp, loff_t off, int whence) {
    loff_t newpos;

	printk(KERN_INFO "Seeking in data initiated\n");

	switch (whence)
	{
	case 0: /* SEEK_SET */
		newpos = off;
		break;
	case 1: /* SEEK_CUR */
		newpos = fp->f_pos + off;
		break;
	case 2: /* SEEK_END */
		newpos = DRVO_MAX_BUFFER_SIZE - off;
		break;
	default: /* can't happen */
		return -EINVAL;
	}
    if( newpos > DRVO_MAX_BUFFER_SIZE ) newpos = DRVO_MAX_BUFFER_SIZE;
    if( newpos < 0 ) newpos = 0;
	fp->f_pos = newpos;
	return newpos;
}

static long drvo_ioctl(struct file *fp, unsigned int cmd, unsigned long arg) {
    printk(KERN_INFO "Driver ioctl initiated\n");
    printk(KERN_INFO "CMD: %d, ARG: %ld\n", cmd, arg);
    return 0;
}

static int drvo_open(struct inode *iNode, struct file *fp)
{
    printk(KERN_INFO "Driver opening initiated\n");
    printk(KERN_INFO "Open counter: %d\n", ++drvo_open_counter);

    fp->private_data = kmalloc(DRVO_MAX_BUFFER_SIZE, GFP_KERNEL);

    return 0;
}

static int drvo_release(struct inode *iNode, struct file *fp)
{
    printk(KERN_INFO "Driver release initiated\n");
    kfree(fp->private_data);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

module_init(drvo_init);
module_exit(drvo_exit);

/////////////////////////////////////////////////////////////////////////////////////////////