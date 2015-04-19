#include "buff.h"

#include <linux/circ_buf.h>
#include <linux/compiler.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/wait.h>

#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <asm/siginfo.h>

#define BUFF_SIZE 512

typedef struct {
	u16* data;
	u32 head;
	u32 tail;
} Buffer;

static Buffer buff;

static DEFINE_SPINLOCK(producer_lock);
static DEFINE_SPINLOCK(consumer_lock);

DECLARE_WAIT_QUEUE_HEAD(WaitQ);
static bool unlocked_;

static bool OVF = false;

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

void b_putData(u16 data_) {
    spin_lock(&producer_lock);
    u32 head = buff.head;
    u32 tail = ACCESS_ONCE(buff.tail);
    if (CIRC_SPACE(head, tail, BUFF_SIZE) >= 1) {
        u16* item = &buff.data[head];
        *item = data_;
        smp_wmb();
        buff.head = (head + 1) & (BUFF_SIZE - 1);
        wake_up(&WaitQ);
        unlocked_ = true;
    } else {
    	printk("Buff overflow!");
    }
    spin_unlock(&producer_lock);
}

#define SIG_TEST 44

u16 b_getData(void) {
	u16 data_ = 0;
    u32 head = ACCESS_ONCE(buff.head);
    u32 tail = buff.tail;
    if (CIRC_CNT(head, tail, BUFF_SIZE) >= 1) {
        smp_read_barrier_depends();
        u16* item = &buff.data[tail];
        data_ = *item;
        smp_mb();
        buff.tail = (tail + 1) & (BUFF_SIZE - 1);
        return data_;
    } else {
    	printk("Buffer is empty!!!\n");
    	unlocked_ = false;
    	wait_event(WaitQ, unlocked_);
    	return b_getData();
    }
}
