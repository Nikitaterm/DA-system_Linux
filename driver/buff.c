#include "buff.h"

#include <linux/circ_buf.h>
#include <linux/compiler.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/wait.h>

#define BUFF_SIZE 512

typedef struct {
	u16* data;
	u32 head;
	u32 tail;
} Buffer;

static Buffer buff;

static DEFINE_SPINLOCK(producer_lock);

DECLARE_WAIT_QUEUE_HEAD(WaitQ);
static bool unlocked_;

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
        wake_up(&WaitQ);
        unlocked_ = true;
    } else {
    	// #TODO: if the buffer is overflow?
    }
    spin_unlock(&producer_lock);
}

u16 b_getData(void) {
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
        return data_;
    } else {
    	unlocked_ = false;
    	wait_event(WaitQ, unlocked_);
    	return b_getData();
    }
}
