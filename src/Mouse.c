#include "../lib/Mouse.h"


unsigned int mouse_hook_id = MOUSE_IRQ;


/* Mouse functions */
mouse_state init_mouse_state()
{
	mouse_state state;
	state.l_button_state = 0;
	state.r_button_state = 0;
	state.curr_position_x = 512;
	state.curr_position_y = 384;
	return state;
}

// subscribes and enables mouse interrupts
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



// unsubscribes mouse interrupts
int mouse_unsubscribe_int()
{
	if(sys_irqrmpolicy(&mouse_hook_id) == OK)
		if(sys_irqdisable(&mouse_hook_id) == OK)
			return 0;

	return 1;
}

void mouse_print_packet(unsigned long *packet)
{
	printf("B1=0x%02X  ", packet[0]);
	printf("B2=0x%02X  ", packet[1]);
	printf("B3=0x%02X  ", packet[2]);

	//Left
	if((packet[0] & BIT(0)) != 0)
		printf("LB=1  ");
	else
		printf("LB=0  ");

	//Middle
	if((packet[0] & BIT(2)) != 0)
		printf("MB=1  ");
	else
		printf("MB=0  ");

	//Right
	if((packet[0] & BIT(1)) != 0)
		printf("RB=1  ");
	else
		printf("RB=0  ");


	//XOV
	if((packet[0] & BIT(6)) != 0)
		printf("XOV=1  ");

	else
		printf("XOV=0  ");

	//YOV
	if((packet[0] & BIT(7)) != 0)
		printf("YOV=1  ");
	else
		printf("YOV=0  ");

	printf("X=%d  ", (char)packet[1]);
	printf("Y=%d  \n", (char)packet[2]);
}

void mouse_print_config(unsigned long *config)
{
	// Remote
	if((config[0] & BIT(6)) != 0)
		printf("\nRemote: Remote (polled) mode\n");
	else
		printf("Remote: Stream mode\n");

	//Enable
	if((config[0] & BIT(5)) != 0)
		printf("Enable: Data reporting enabled\n");
	else
		printf("Enable: Disabled\n");

	//scaling
	if((config[0] & BIT(4)) != 0)
		printf("Scaling: 2:1\n");
	else
		printf("Scaling: 1:1\n");

	//Left
	if((config[0] & BIT(2)) != 0)
		printf("Left button: Pressed\n");
	else
		printf("Left button: Released\n");

	//Middle
	if((config[0] & BIT(1)) != 0)
		printf("Middle button: Pressed\n");
	else
		printf("Middle button: Released\n");

	//Right
	if((config[0] & BIT(0)) != 0)
		printf("Right button: Pressed\n");
	else
		printf("Right button: Released\n");

	//Resolution
	printf("Resolution: %d\n", config[1]);

	//Sample rate
	printf("Sample Rate: %d\n", config[2]);

}

void transform_mouse_values(mouse_state* state, unsigned long *packet)
{
	//compute delta_x
	if((packet[0] & BIT(6)) != 0)
	{
		state->x_overflow = 1;
		//printf("X Overflow\n");
		if((packet[0] & BIT(4)) != 0)
			state->delta_x = (-1<<8);
		else
			state->delta_x = (1<<8)-1;
	}
	else
	{
		state->x_overflow = 0;
		if((packet[0] & BIT(4)) != 0)
			state->delta_x = ((-1<<8) | packet[1]);
		else
			state->delta_x = (int)packet[1];
	}

	//Compute delta_y
	if((packet[0] & BIT(7)) != 0)
	{
		state->y_overflow = 1;
		if((packet[0] & BIT(5)) != 0)
			state->delta_y = (-1<<8);
		else
			state->delta_y = (1<<8)-1;
	}
	else
	{
		state->y_overflow = 0;
		if((packet[0] & BIT(5)) != 0)
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

	if((packet[0] & BIT(1)) != 0)
		state->r_button_state = 1;
	else
		state->r_button_state = 0;

	if((packet[0] & BIT(2)) != 0)
		state->l_button_state = 1;
	else
		state->l_button_state = 0;

	transform_mouse_values(state, packet);

//	printf("x %d dx %d\n", state->curr_position_x, state->delta_x);
//	printf("y %d dy %d\n", state->curr_position_y, state->delta_y);
//	printf("yf: %d\n", (state->curr_position_y - state->delta_y + MOUSE_HEIGHT ));
//	printf("xf: %d\n\n", (state->curr_position_x + state->delta_x + MOUSE_WIDTH));

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


