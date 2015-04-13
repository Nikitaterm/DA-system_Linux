#include "backend.h"

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikila Lazarev");
MODULE_DESCRIPTION("DA_protocol driver");

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
	printk("DA_driver:DEBUG: driver started\n");
    return 0;
}

void module_stop(void) {
    free_spaces();
    printk("DA_driver:DEBUG: driver finished\n");
    return;
}

module_init(module_start);
module_exit(module_stop);
