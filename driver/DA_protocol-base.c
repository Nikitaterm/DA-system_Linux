#include "backend.h"

#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikila Lazarev");
MODULE_DESCRIPTION("DA_protocol driver");

#define BUFF_LEN 2

static volatile u8 dev_opend = 0;

static int dev_open(struct inode *inode, struct file *file) {
    if (dev_opend) {  // The device can be opend only ones
        return -EBUSY;
    }
    dev_opend++;
    try_module_get(THIS_MODULE);
    return startReading();
}

static int dev_release(struct inode *inode, struct file *file) {
    stopReading();
    dev_opend--;
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t dev_read(struct file * file, char * buf,
                        size_t count, loff_t *ppos) {
    char* info_str;
    u16 data;
    if( count < BUFF_LEN ) return -EINVAL;
    if( *ppos != 0 ) {
        printk("DA_driver: dev_read failed, file_pos != 0 (%d != 0).\n", (int)*ppos);
        return 0;
    }
    data = getData();
    info_str = (char*)(&data);
    if(copy_to_user(buf, info_str, BUFF_LEN) ) {
        return -EINVAL;
    }
    return BUFF_LEN;
}

static const struct file_operations DA_fops = {
    .owner  = THIS_MODULE,
    .open   = dev_open,
    .release  = dev_release,
    .read   = dev_read,
};

static struct miscdevice DA_dev = {
    MISC_DYNAMIC_MINOR,
    "DA_system",
    &DA_fops
};

int module_start(void) {
    int err;
    err = map_GPIO();
    if (err) goto ERR;
    err = configure_DATABUS();
    if (err) goto ERR;
    err = configure_IRQ_RSP();
    if (err) goto ERR;
    err = misc_register(&DA_dev);
    if (err) {
        printk("DA_driver: device registration failed.\n");
        goto ERR;
    }
    printk("DA_driver:DEBUG: driver started.\n");
    return 0;
    ERR: return err;
}

void module_stop(void) {
    free_spaces();
    misc_deregister(&DA_dev);
    printk("DA_driver:DEBUG: driver finished.\n");
    return;
}

module_init(module_start);
module_exit(module_stop);
