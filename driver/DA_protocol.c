#include "linux/init.h"
#include "linux/module.h"
#include "linux/kernel.h"
#include "linux/types.h"
#include "linux/ioport.h"
#include "asm/io.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikila Lazarev");
MODULE_DESCRIPTION("DA_protocol driver");

/* |PG9|PG8|PG7|PG6|PG5|PG4|PG3|PG2|PG1|PG0| : DATA
   |PH14| : IRQ_RSP */

#define PIO_BASE 0x01C20800

#define DATABUS_DATA 0xE8
#define DATABUS_DATA_SIZE 0x2
#define DATABUS_CONF 0xD8
#define DATABUS_CONF_SIZE 0x5
#define DATABUS_PULLUP 0xF4
#define DATABUS_PULLUP_SIZE 0x3

#define IRQ_RSP_DATA 0x11A  // 0x10C + 0xE
#define IRQ_RSP_DATA_SIZE 0x1
#define IRQ_RSP_CONF 0x118  // 0x100 + 0x18
#define IRQ_RSP_CONF_SIZE 0x1
#define DATA_MASK 0x3FF             // 10 bit

static void* gpio_map;

static int request_GPIO(u32 offset, u32 size) {
    if (check_region(PIO_BASE + offset, size)) {
        printk("DA_driver: Allocation io ports at address %x failed. The region is busy!\n", PIO_BASE + offset);
        return -EBUSY;
    }
    if (request_region(PIO_BASE + offset, size, "DA_driver") == NULL) {
        printk("DA_driver: IO request failed!\n");
        return -EBUSY;
    }
    return 0;
}

static int configure_DATABUS(void) {
	int err;
	u32 data;
    err = request_GPIO(DATABUS_DATA, DATABUS_DATA_SIZE);
    if (err) {
        return err;
    }
    err = request_GPIO(DATABUS_CONF, DATABUS_CONF_SIZE);
    if (err) {
        return err;
    }
    err = request_GPIO(DATABUS_PULLUP, DATABUS_PULLUP_SIZE);
    if (err) {
        return err;
    }
    // write configuration into DATABUS_CONF and DATABUS_PULL registers
    iowrite32(0, gpio_map + DATABUS_CONF);   // set as input
    iowrite8(0, gpio_map + DATABUS_CONF + 0x20);   // set as input
    data = ioread32(gpio_map + DATABUS_PULLUP);
    data &= (0x7FF << 20); // zero out bits
    data |= 0xAAAAA;  // set as pull down ([10] mode)
    iowrite32(data, gpio_map + DATABUS_PULLUP);
    return 0;
}

static int configure_IRQ_RSP(void) {
	int err;
	u8 data;
	err = request_GPIO(IRQ_RSP_DATA, IRQ_RSP_DATA_SIZE);
    if (err) {
        return err;
    }
	err = request_GPIO(IRQ_RSP_CONF, IRQ_RSP_CONF_SIZE);
    if (err) {
        return err;
    }
    // write configuration into READYPIN_CONF registers
    data = ioread8(gpio_map + IRQ_RSP_CONF);
    data |= (1<<0); // set as output
    iowrite8(data, gpio_map + IRQ_RSP_CONF);
    return 0;
}

static u16 readData(void) {
	return ioread16(gpio_map + DATABUS_DATA) & DATA_MASK;
}

static void free_GPIO(u32 offset, u32 size) {
    release_region(PIO_BASE + offset, size);
}

static void finish_module(void) {
    iounmap(gpio_map);
    free_GPIO(DATABUS_DATA, DATABUS_DATA_SIZE);
    free_GPIO(DATABUS_CONF, DATABUS_CONF_SIZE);
    free_GPIO(DATABUS_PULLUP, DATABUS_PULLUP_SIZE);
    free_GPIO(IRQ_RSP_DATA, IRQ_RSP_DATA_SIZE);
    free_GPIO(IRQ_RSP_CONF, IRQ_RSP_CONF_SIZE);
    printk("DA_driver: driver finished\n");
}

int module_start(void) {
	int err;
	gpio_map = ioremap(PIO_BASE, 4096);
	if (gpio_map == NULL) {
        printk("DA_driver: ioremap failed\n");
        return -EIO;
	}
    err = configure_DATABUS();
    if (err) {
        return err;
	}
	err = configure_IRQ_RSP();
	if (err) {
        return err;
	}
	printk("DA_driver: driver started\n");
	printk("Data: %d\n", readData());
    return 0;
}

void module_stop(void) {
    finish_module();
    return;
}

module_init(module_start);
module_exit(module_stop);
