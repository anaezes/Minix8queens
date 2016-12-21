#ifndef __M_H
#define __M_H

#include <minix/syslib.h>
#include <minix/com.h>
#include <minix/drivers.h>

#include "Driver.h"
#include "i8042.h"
#include "vbe.h"
#include "Common.h"

#define MOUSE_HEIGHT	33
#define MOUSE_WIDTH 	40
#define MOUSE_INIT_X 	512
#define MOUSE_INIT_Y	384

typedef struct mouse_state
{
	int delta_x;
	int delta_y;
	short x_overflow;
	short y_overflow;
	short x_sign;
	short y_sign;
	short r_button_state;
	short l_button_state;
	int curr_position_x;
	int curr_position_y;

} mouse_state;

mouse_state init_mouse_state();

int mouse_subscribe_int(void);
int mouse_unsubscribe_int(void);
void mouse_print_packet(unsigned long*);
void mouse_print_config(unsigned long*);
void xy_abs_values(unsigned long*);
void update_mouse_state(mouse_state*, unsigned long*);

#endif
