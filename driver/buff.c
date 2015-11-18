#include "buff.h"

#include <linux/circ_buf.h>
#include <linux/compiler.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/wait.h>

#define BUFF_SIZE 4096  //#TODO: dynamic change via ioctl

typedef struct {
	u16* data;
	u32 head;
	u32 tail;
} Buffer;

static Buffer buff;

static DEFINE_SPINLOCK(producer_lock);

int initBuff(void) {
	buff.data = (u16*)kmalloc(sizeof(u16)*BUFF_SIZE, GFP_KERNEL);
	if (buff.data == NULL) {
    	return -ENOMEM;
	}
    buff.tail = 0;
    buff.head = 0;
	return 0;
}

void freeBuff(void) {
    kfree(buff.data);
}

void b_putData(const u16 data_) {
	u32 head, tail;
	u16* item;
    spin_lock(&producer_lock);
    head = buff.head;
    tail = ACCESS_ONCE(buff.tail);
    if (CIRC_SPACE(head, tail, BUFF_SIZE) >= 1) {
        item = &buff.data[head];
        *item = data_;
        smp_wmb();
        buff.head = (head + 1) & (BUFF_SIZE - 1);
    } else {
        // #TODO: if the buffer is overflow? Send some signal to a board!
        printk("DA_driver: Buffer overflow!\n");
    }
    spin_unlock(&producer_lock);
}

bool b_getData(u16* data) {
	u16 data_ = 0;
	u16* item;
    u32 head = ACCESS_ONCE(buff.head);
    u32 tail = buff.tail;
    if (CIRC_CNT(head, tail, BUFF_SIZE) >= 1) {
        smp_read_barrier_depends();
        item = &buff.data[tail];
        data_ = *item;
        smp_mb();
        buff.tail = (tail + 1) & (BUFF_SIZE - 1);
        *data = data_;
        return true;
    } else {
        return false;
    }
}
