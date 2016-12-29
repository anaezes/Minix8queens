#ifndef __KB_H
#define __KB_H

#include <minix/syslib.h>
#include <minix/com.h>
#include <minix/drivers.h>

#include "Driver.h"
#include "i8042.h"

int kb_subscribe_int(void);
int kb_unsubscribe_int(void);
int kb_int_handler_asm(unsigned long);

// this function is called when a keyboard
// interrupted is triggered and reads the
// scancode from KB_OUT_BUFFER
int kb_int_handler(unsigned long*);


// given a scancode checks if it`s a makecode
int is_break_code(unsigned long);
void kb_print_scancode(unsigned long, int);

#endif
