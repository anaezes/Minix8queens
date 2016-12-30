#ifndef __M_H
#define __M_H

#include <minix/syslib.h>
#include <minix/com.h>
#include <minix/drivers.h>

#include "Driver.h"
#include "i8042.h"
#include "vbe.h"
#include "Common.h"

/**
 * Mouse pixmap height
 * */
#define MOUSE_HEIGHT	35


/**
 * Mouse pixmap width
 * */
#define MOUSE_WIDTH 	20


/**
 * Mouse initial x coordinate
 * */
#define MOUSE_INIT_X 	512


/**
 * Mouse initial y coordinate
 * */
#define MOUSE_INIT_Y	384


/**
 * Struct that contains current mouse state
 * */
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


/**
 * Init mouse state struct
 * @return mouse state
 * */
mouse_state init_mouse_state();


/**
 * Subscribes and enables mouse interrupts.
 * @return 0 if success
 * */
int mouse_subscribe_int(void);


/**
 * Unsubscribes mouse interrupts.
 * @return 0 if success
 * */
int mouse_unsubscribe_int(void);


/**
 * Print a given mouse packet.
 * @param mouse packet
 * */
void mouse_print_packet(unsigned long*);


/**
 * Print a given mouse config.
 * @param mouse packet
 * */
void mouse_print_config(unsigned long*);


/**
 * Parse mouse packets.
 * @param mouse state
 * @param mouse packet
 * */
void transform_mouse_values(mouse_state*, unsigned long*);


/**
 * Updates current mouse state.
 * @param mouse state
 * @param mouse packet
 * */
void update_mouse_state(mouse_state*, unsigned long*);


/**
 * Verifies if is mouse click.
 * @param mouse state
 * @param previous left button state
 * */
int is_mouse_click(mouse_state*, int);


#endif
