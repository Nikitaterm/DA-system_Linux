#ifndef BACKEND_H
#define BACKEND_H

#include <linux/types.h>

/* Pinout:
   |PG9|PG8|PG7|PG6|PG5|PG4|PG3|PG2|PG1|PG0| : DATA
   |PH14| : IRQ_RSP 

   Interrupts:
   |EINT14|  :  IRQ_RSP */

int configure_DATABUS(void);  // Request and configure the data bus
int configure_IRQ_RSP(void);  // Request and configure IRQ_RSP
int map_GPIO(void);           // Map GPIO address space
u16 getData(void);           // Read data from the data bus
void free_spaces(void);       // Free all requested spaces

#endif
