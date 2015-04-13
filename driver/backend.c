#include "backend.h"

#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/irqreturn.h>
#include <linux/kernel.h>

#define PIO_BASE_                  0x01C20800   // PIO base address

#define PG_DAT                     0xE8         // DATABUS data register
#define DATABUS_DATA_SIZE          2
#define PG_CFG0                    0xD8         // DATABUS configure register
#define DATABUS_CONF_SIZE          5
#define PG_PULL0                   0xF4         // DATABUS pull register
#define DATABUS_PULLUP_SIZE        3

#define IRQ_RSP_PIN                14           // IRQ_RSP pin number
#define PH_DAT                     0x10C        // IRQ_RSP data register
#define IRQ_RSP_DATA_SIZE          1
#define PH_CFG1                    0x100        // IRQ_RSP configure register
#define IRQ_RSP_CONF_SIZE          1
#define EINTx                      0x6          // EINT mode
#define PULL_DWN                   0x2          // pull-down mode
#define EINT_P_ADG                 0x0          // positive adge mode
#define PH_PULL0                   0x118        // IRQ_RSP pull register
#define DATA_MASK                  0x3FF        // bitmask 0b1111111111

#define IRQ_                       60           // interrupt number in the global table
#define PIO_INT_CFG1               0x204        // PIO interrupt configure register
#define PIO_INT_CTL                0x210        // PIO interrupt control register
#define PIO_INT_STATUS             0x214        // POI interrupt status register

#define MSK_4                      0xF          // bit-mask 1111
#define MSK_3                      0x7          // bit-mask 111
#define MSK_2                      0x3          // bit-mask 11

void* gpio_map;
static int dev_id;

static irqreturn_t intr_handler( int irq, void *dev_id );

static int request_GPIO(u32 offset, u32 size) {
    if (check_region(PIO_BASE_ + offset, size)) {
        printk("DA_driver: Allocation io ports at address %x failed. The region is busy!\n", PIO_BASE_ + offset);
        return -EBUSY;
    }
    if (request_region(PIO_BASE_ + offset, size, "DA_driver") == NULL) {
        printk("DA_driver: IO request failed!\n");
        return -EBUSY;
    }
    return 0;
}

int configure_DATABUS(void) {
	int err;
	u32 conf;
    err = request_GPIO(PG_DAT, DATABUS_DATA_SIZE);
    if (err) {
        return err;
    }
    err = request_GPIO(PG_CFG0, DATABUS_CONF_SIZE);
    if (err) {
        return err;
    }
    err = request_GPIO(PG_PULL0, DATABUS_PULLUP_SIZE);
    if (err) {
        return err;
    }
    iowrite32(0, gpio_map + PG_CFG0);  // set as input
    iowrite8(0, gpio_map + PG_CFG0 + 0x20);  // set as input
    conf = ioread32(gpio_map + PG_PULL0);
    conf &= (0x7FF << 20);  // zero out bits
    conf |= 0xAAAAA;  // set as pull down ([10] mode)
    iowrite32(conf, gpio_map + PG_PULL0);
    return 0;
}

int configure_IRQ_RSP(void) {
	int err;
	u32 conf;
	err = request_GPIO(PH_DAT, IRQ_RSP_DATA_SIZE);
    if (err) {
        return err;
    }
	err = request_GPIO(PH_CFG1, IRQ_RSP_CONF_SIZE);
    if (err) {
        return err;
    }
    conf = ioread32(gpio_map + PH_CFG1);
    conf = (conf & ~(MSK_3 << 24)) | (EINTx << 24);  // configure as an interrupt source
    iowrite32(conf, gpio_map + PH_CFG1);
    conf = ioread32(gpio_map + PH_PULL0);
    conf = (conf & ~(MSK_2 << 28)) | (PULL_DWN << 28);  // set as pull down
    iowrite32(conf, gpio_map + PH_PULL0);
	err = request_irq(IRQ_, intr_handler, IRQF_DISABLED, "DA_driver_interrupt", &dev_id);
    if (err) {
    	printk("DA_driver: request irq failed\n");
        return err;
    }
    conf = ioread32(gpio_map +PIO_INT_CFG1);
    conf = (conf & ~(MSK_4 << 24)) | (EINT_P_ADG << 24);  // set positive adge
    iowrite32(conf, gpio_map + PIO_INT_CFG1);
    conf = ioread32(gpio_map + PIO_INT_CTL);
    conf |= (1 << IRQ_RSP_PIN); // enable the intr
    iowrite32(conf, gpio_map + PIO_INT_CTL);
    return 0;
}

u16 readData(void) {
	return ioread16(gpio_map + PG_DAT) & DATA_MASK;
}

static void free_GPIO(u32 offset, u32 size) {
    release_region(PIO_BASE_ + offset, size);
}

static void finish_intr(void) {
	u32 conf_w;
	synchronize_irq(IRQ_);
	conf_w = ioread32(gpio_map + PIO_INT_CTL);
    conf_w &= ~(1 << IRQ_RSP_PIN); // disable the intr
    iowrite32(conf_w, gpio_map + PIO_INT_CTL);
    free_irq(IRQ_, &dev_id);
}

static irqreturn_t intr_handler( int irq, void *dev_id ) {
	u32 status = ioread32(gpio_map + PIO_INT_STATUS);
    if (!(status & (1 << IRQ_RSP_PIN))) {
    	return IRQ_NONE;
    }
    status |= (1 << IRQ_RSP_PIN); // clear the flag
    iowrite32(status, gpio_map + PIO_INT_STATUS);
    //*************IRQ_LOGIC**************
    printk("Data: %d\n", readData());
    //************************************
    return IRQ_HANDLED;

}

int map_GPIO(void) {
    gpio_map = ioremap(PIO_BASE_, 4096);
    if (gpio_map == NULL) {
        printk("DA_driver: ioremap failed\n");
        return -EIO;
    }
    return 0;
}

void free_spaces() {
    free_GPIO(PG_DAT, DATABUS_DATA_SIZE);
    free_GPIO(PG_CFG0, DATABUS_CONF_SIZE);
    free_GPIO(PG_PULL0, DATABUS_PULLUP_SIZE);
    free_GPIO(PH_DAT, IRQ_RSP_DATA_SIZE);
    free_GPIO(PH_CFG1, IRQ_RSP_CONF_SIZE);
    finish_intr();
    iounmap(gpio_map);
}
