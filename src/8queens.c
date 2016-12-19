#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <math.h>

#include "../lib/8queens.h"
#include "../lib/pixmap.h"
#include "../lib/sprite.h"
#include "../lib/Mouse.h"

unsigned long packet[MOUSE_PACKET_SIZE];
unsigned long config[MOUSE_CONFIG_SIZE];
unsigned int pos = 0;

game_st init_game()
{
	game_st state;
	state.n_queens = 0;
	state.curr_state = INIT;
	state.graphics_state = NULL;

	int i, j;
	for(i = 0; i < BOARD_SIZE; i++)
		for(j = 0; j < BOARD_SIZE; j++)
			state.board[i][j] = 0;

	return state;
}


int game_loop() {

	char* vram_address = vg_init(GRAPHIC_MODE);
	if (vram_address == NULL) {
		printf("Error\n");
		vg_exit();
		return 1;
	}

	int timer_irq = timer_subscribe_int();
	if (timer_irq == -1) {
		printf("ERROR: to subscribe timer");
		vg_exit();
		return 1;
	}

	int irq_set = kb_subscribe_int();
	if (irq_set == -1) {
		printf("ERROR: to subscribe kb");
		timer_unsubscribe_int();
		vg_exit();
		return 1;
	}

	int irq_set_mouse = mouse_subscribe_int();
	if (irq_set_mouse == -1) {
		printf("ERROR: to subscribe mouse");
		kb_unsubscribe_int();
		timer_unsubscribe_int();
		vg_exit();
		return 1;
	}

	kbc_mouse_init();

	vg_start();

	game_st game_state = init_game();
	//game_state.graphics_state = vg_get_area_state(MOUSE_INIT_X, MOUSE_INIT_Y,  MOUSE_WIDTH, MOUSE_HEIGHT);

	int count = 0;
	int finished = 0;
	int second_time = 0;
	int start_time = timer_get_ellapsed_time();

	int packet_async = 1;
	short curr_length = 0;

	mouse_state state = init_mouse_state();

	// if graphics need to be repainted
	int graphics_invalidated = 0;

	int x = 251;
	int y = 35;
	unsigned int color = 63;
	int width;
	int height;
	char** queen = pixmap_get_image(1);
	char* pixmap = read_xpm(queen, &width, &height);

	//rectangle time
	int xi = 30;
	int yi = 716;
	int  widthR = 964;
	int heightR = 30;
	unsigned colorR = 6;

	unsigned long scancode = 0x00;
	int ipc_status, r;
	message msg;
	while (game_state.curr_state != END) {
		if (driver_receive(ANY, &msg, &ipc_status) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {

			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set_mouse) {

					sys_inb(KBC_DATA_BUFFER, &packet[pos]);

					if ((count == 0 && ((packet[pos] & BIT(3))&& (packet[pos] != KBC_ACK))) != 0) {
						packet_async = 0;
					}

					if (packet_async == 0) {
						pos = (pos + 1) % MOUSE_PACKET_SIZE;
						if (pos == 2) {

							event_t evt;

							//vg_reset_area_state(game_state.graphics_state, state.curr_position_x, state.curr_position_y,  MOUSE_WIDTH, MOUSE_HEIGHT);
							update_mouse_state(&state, packet);
							graphics_invalidated = 1;

							if(game_state.curr_state == INIT)
							{
								if (state.l_button_state == 1)
								{
									if ((state.curr_position_x >= 296 && state.curr_position_x <= 666)
											&& (state.curr_position_y >= 541 && state.curr_position_y <= 593))
									{
										evt.type = MOVE;
										game_state.curr_state = PLAY;
										start_time = timer_get_ellapsed_time();
									}
								}
							}

							count++;
						}
					}
				}

				if (msg.NOTIFY_ARG & timer_irq)
				{
					timer_int_handler();
//					if(game_state.curr_state == INIT)
//						vg_start();
//					else if(game_state.curr_state == PLAY)
//						vg_game();
				}

				if (msg.NOTIFY_ARG & irq_set) {
					int n_bytes = kb_int_handler(&scancode);
					kb_print_scancode(scancode, n_bytes);

					//set state machine in the last state
					if (scancode == DRIVER_END_SCODE)
						game_state.curr_state = END;
					if (game_state.curr_state == INIT && scancode == 0x1C)
					{
						game_state.curr_state = PLAY;
						vg_game();
						vg_draw_rectangle(xi, yi, widthR, heightR, colorR);
						vg_draw_pixmap(x+3, y+5, pixmap, width, height);
						start_time = timer_get_ellapsed_time();
					}
					if(game_state.curr_state == PLAY)
						move_handler(scancode, &x, &y, &color);

				}

				break;
			default:
				break;
			}
		}

		if(graphics_invalidated == 1)
		{
			//free(game_state.graphics_state);
			//game_state.graphics_state = vg_get_area_state(state.curr_position_x, state.curr_position_y,  MOUSE_WIDTH, MOUSE_HEIGHT);
			vg_draw_mouse_pointer(state.curr_position_x,state.curr_position_y);
			graphics_invalidated = 0;
		}

		if(game_state.curr_state == PLAY)
		{
			if((timer_get_ellapsed_time() - start_time) == 1)
			{
				if(widthR > 0)
				{
					xi += 2;
					widthR -= 2;

					vg_draw_rectangle(30, 716, 964, 30, 56);
					vg_draw_rectangle(xi, yi, widthR, heightR, colorR);
					start_time = timer_get_ellapsed_time();
				}
				else
				{
					//game over ?
				}
			}
		}
	}

	vg_exit();
	kb_unsubscribe_int();
	timer_unsubscribe_int();

	if (kbc_write_to_mouse(MOUSE_DISABLE_DR) != 0)
		return 1;

	mouse_unsubscribe_int();

	return 0;
}

int move_handler(unsigned long code, int* x, int* y, unsigned int* color) {

	int width;
	int height;
	char** queen = pixmap_get_image(1);

	char* pixmap = read_xpm(queen, &width, &height);

	switch (code) {
	case 0xCD: //right
		vg_draw_rectangle(*x, *y, 82, 82, *color);
		*x += 81;
		if(*x > 850)
			*x = 251;
		vg_draw_pixmap(*x+3, *y+5, pixmap, width, height);
		break;
	case 0xCB://left
		if(*x-81 < 251)
			break;
		vg_draw_rectangle(*x, *y, 82, 82, *color);
		*x -= 81;
		vg_draw_pixmap(*x+3, *y+5, pixmap, width, height);
		break;
	case 0xC8: //up
		if(*y-81 < 35)
			break;
		vg_draw_rectangle(*x, *y, 82, 82, *color);
		*y -= 81;
		vg_draw_pixmap(*x+3, *y+5, pixmap, width, height);
		break;
	case 0xD0: //down
		vg_draw_rectangle(*x, *y, 82, 82, *color);
		*y += 81;
		if(*y > 650)
			*y = 35;
		vg_draw_pixmap(*x+3, *y+5, pixmap, width, height);
		break;
	case 0x1C: //ENTER
		//check algorithm
		*x = 251;
		*y = 35;
		*color = 63;

		pixmap = read_xpm(queen, &width, &height);
		vg_draw_pixmap(*x+3, *y+5, pixmap, width, height);

		return 1;
		break;
	default:
		return 0;
	}

	if(*color == 56)
		*color = 63;
	else
		*color = 56;

	return 0;
}




