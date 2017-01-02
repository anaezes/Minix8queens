#include "Mouse.h"


unsigned int mouse_hook_id = MOUSE_IRQ;


mouse_state init_mouse_state()
{
	mouse_state state;
	state.l_button_state = 0;
	state.r_button_state = 0;
	state.curr_position_x = 512;
	state.curr_position_y = 384;
	return state;
}


int mouse_subscribe_int(void)
{
	// subscribes to interrupts IRQ 1
	unsigned int tmp = mouse_hook_id;

	if(sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) == OK)
		if(sys_irqenable(&mouse_hook_id) == OK)
			return BIT(tmp);
	printf("Error while subscribing to MOUSE interruptions\n");
	return -1;
}


int mouse_unsubscribe_int()
{
	if(sys_irqrmpolicy(&mouse_hook_id) == OK)
		if(sys_irqdisable(&mouse_hook_id) == OK)
			return 0;

	return 1;
}

void transform_mouse_values(mouse_state* state, unsigned long *packet)
{
	//compute delta_x
	if((packet[0] & X_OVERFLOW_BIT) != 0)
	{
		state->x_overflow = 1;

		if((packet[0] & X_SIGN_BIT) != 0)
			state->delta_x = (-1<<8);
		else
			state->delta_x = (1<<8)-1;
	}
	else
	{
		state->x_overflow = 0;
		if((packet[0] & X_SIGN_BIT) != 0)
			state->delta_x = ((-1<<8) | packet[1]);
		else
			state->delta_x = (int)packet[1];
	}

	//Compute delta_y
	if((packet[0] & Y_OVERFLOW_BIT) != 0)
	{
		state->y_overflow = 1;
		if((packet[0] & Y_SIGN_BIT) != 0)
			state->delta_y = (-1<<8);
		else
			state->delta_y = (1<<8)-1;
	}
	else
	{
		state->y_overflow = 0;
		if((packet[0] & Y_SIGN_BIT) != 0)
			state->delta_y = ((-1<<8) | packet[2]);
		else
			state->delta_y = (int)packet[2];
	}

	if(state->delta_y <= -254)
		state->delta_y = -1;
	if(state->delta_y >= 254)
		state->delta_y = 1;

}


void update_mouse_state(mouse_state* state, unsigned long *packet)
{

	if((packet[0] & RIGHT_BUTTON) != 0)
		state->r_button_state = 1;
	else
		state->r_button_state = 0;

	if((packet[0] & LEFT_BUTTON) != 0)
		state->l_button_state = 1;
	else
		state->l_button_state = 0;

	transform_mouse_values(state, packet);

	if((state->curr_position_x + state->delta_x >= 0) && (state->curr_position_x + state->delta_x + MOUSE_WIDTH <= H_RES))
		state->curr_position_x += state->delta_x;
	else if(state->curr_position_x + state->delta_x < 0)
		state->curr_position_x = 0;
	else if((state->curr_position_x + state->delta_x + MOUSE_WIDTH) > H_RES)
		state->curr_position_x = H_RES - MOUSE_WIDTH;


	if((state->curr_position_y - state->delta_y >= 0) && (state->curr_position_y - state->delta_y + MOUSE_HEIGHT <= V_RES))
		state->curr_position_y -= state->delta_y;
	else if(state->curr_position_y - state->delta_y < 0)
		state->curr_position_y = 0;
	else if((state->curr_position_y - state->delta_y + MOUSE_HEIGHT) > V_RES)
		state->curr_position_y = V_RES - MOUSE_HEIGHT;

}


int is_mouse_click(mouse_state* state, int previous_left_b)
{
	return (state->l_button_state == 1) && (previous_left_b == 0);
}
