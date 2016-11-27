#ifndef __M_H
#define __M_H

#include <minix/syslib.h>
#include <minix/com.h>
#include <minix/drivers.h>

#include "Driver.h"
#include "i8042.h"

typedef enum {INIT, DRAW, COMP} state_t;
typedef enum {RDOWN, RUP, MOVE} ev_type_t;

typedef struct mouse_state
{
	int delta_x;
	int delta_y;
	short r_button_state;
} mouse_state;


int mouse_subscribe_int(void);
int mouse_unsubscribe_int(void);
void mouse_print_packet(unsigned long*);
void mouse_print_config(unsigned long*);
short absolute_length(short);
void xy_abs_values(unsigned long*);
mouse_state generate_state(unsigned long*);

#endif
