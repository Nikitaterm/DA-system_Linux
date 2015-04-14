#include "backend.h"

#include <linux/string.h>

#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikila Lazarev");
MODULE_DESCRIPTION("DA_protocol driver");

#define BUFF_LEN 2

static ssize_t dev_read( struct file * file, char * buf,
                        size_t count, loff_t *ppos ) {
    if( count < BUFF_LEN ) return -EINVAL;
    if( *ppos != 0 ) {
        return 0;
    }
    u16 data = getData();
    char* info_str = (char*)(&data);
    if(copy_to_user(buf, info_str, BUFF_LEN) ) {
        return -EINVAL;
    }
    *ppos = BUFF_LEN;
    return BUFF_LEN;
}

static const struct file_operations DA_fops = {
    .owner  = THIS_MODULE,
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
    if (err) {
        return err;
	}
    err = configure_DATABUS();
    if (err) {
        return err;
	}
	err = configure_IRQ_RSP();
	if (err) {
        return err;
	}
    err = misc_register(&DA_dev);
    if (err) {
        printk("DA_driver: device registration failed\n");
        return err;
    }
	printk("DA_driver:DEBUG: driver started\n");
    return 0;
}

void module_stop(void) {
    free_spaces();
    misc_deregister(&DA_dev);
    printk("DA_driver:DEBUG: driver finished\n");
    return;
}

module_init(module_start);
module_exit(module_stop);
