#ifndef _DRIVER_H
#define _DRIVER_H

#include <minix/syslib.h>
#include <minix/com.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

#include "i8042.h"
#include "Mouse.h"
#include "Keyboard.h"


int kbc_read_status(unsigned long*);
int kbc_wait_inbuffer();
int kbc_wait_outbuffer(unsigned long*);
int kbc_send_command(unsigned long);
int kbc_signal_mouse_write();
int kbc_read_input(unsigned long*);
int kbc_write_to_mouse(unsigned long);
void kbc_flush_data_buffer(void);
int kbc_mouse_init();




#endif
