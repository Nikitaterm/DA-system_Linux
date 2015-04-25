#ifndef BUFF_H
#define BUFF_H

#include <linux/types.h>

int initBuff(void);              // init the buffer with capacity = size
void b_putData(const u16 data);  // put data into the head
u16 b_getData(void);             // read data from the tail
void freeBuff(void);             // clean the buffer and free allocated memory

#endif
