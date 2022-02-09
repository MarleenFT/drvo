#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

/////////////////////////////////////////////////////////////////////////////////////////////

MODULE_VERSION("1");
MODULE_DESCRIPTION("Test module");
MODULE_AUTHOR("You");
MODULE_LICENSE("Dual BSD/GPL");

/////////////////////////////////////////////////////////////////////////////////////////////

static int      drvo_init(void);
static void     drvo_exit(void);
static ssize_t  drvo_read(struct file*, char*, size_t, loff_t*);
static ssize_t  drvo_write(struct file*, const char*, size_t, loff_t*);
static int      drvo_open(struct inode*, struct file*);
static int      drvo_release(struct inode*, struct file*);

/////////////////////////////////////////////////////////////////////////////////////////////

struct cdev* drvo_cdev;
dev_t       drvo_dev = 0;

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
    printk(KERN_ALERT "Initialization process\n");

    alloc_chrdev_region(&drvo_dev, 0, 1, "drvo_dev");

    cdev_init(&drvo_cdev, &drvo_file_ops);
    cdev_add(&drvo_cdev, drvo_dev, 1);

    printk(KERN_ALERT "Dev: %d, %d \n", MAJOR(drvo_cdev->dev), drvo_cdev->count);

    return 0;
}

static void drvo_exit(void)
{
    printk(KERN_ALERT "Exiting\n");
    cdev_del(drvo_cdev);
}

static ssize_t drvo_read(struct file* pf, char* buf, size_t size, loff_t* lof) {
    printk(KERN_ALERT "Reading %ld\n", size);
    return 0;
}

static ssize_t drvo_write(struct file* pf, const char* buf, size_t size, loff_t* lof) {
    printk(KERN_ALERT "Writing %ld\n", size);
    return 0;
}

static int drvo_open(struct inode* iNode, struct file* fp) {
    printk(KERN_ALERT "Opening\n");
    return 0;
}

static int drvo_release(struct inode* iNode, struct file* fp) {
    printk(KERN_ALERT "Releasing\n");
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

module_init(drvo_init);
module_exit(drvo_exit);

/////////////////////////////////////////////////////////////////////////////////////////////