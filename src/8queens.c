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

game_state init_game()
{
	game_state state;
	state.n_queens = 0;
	state.curr_state = INIT;

	int i, j;
	for(i = 0; i < BOARD_SIZE; i++)
		for(j = 0; j < BOARD_SIZE; j++)
			state.board[i][j] = 0;

	return state;
}


int game_loop() {

	state_t curr_state = INIT;
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

	printf("******* MOUSE: %d\n", irq_set_mouse);

	vg_start();

	int count = 0;
	int finished = 0;
	int second_time = 0;
	int start_time = timer_get_ellapsed_time();

	int packet_async = 1;
	short curr_length = 0;
	mouse_state state;
	INIT_MOUSE_STATE(mouse_state);

	int x = 251;
	int y = 35;
	unsigned int color = 63;
	int width;
	int height;
	char** queen = pixmap_get_image(1);
	char* pixmap = read_xpm(queen, &width, &height);


	unsigned long scancode = 0x00;
	int ipc_status, r;
	message msg;
	while (curr_state != END) {
		if (driver_receive(ANY, &msg, &ipc_status) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {

			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set_mouse) {
					printf("pim............\n");

					sys_inb(KBC_DATA_BUFFER, &packet[pos]);

					if ((count == 0
							&& ((packet[pos] & BIT(3))
									&& (packet[pos] != KBC_ACK))) != 0) {
						packet_async = 0;
					}

					if (packet_async == 0) {
						pos = (pos + 1) % MOUSE_PACKET_SIZE;
						if (pos == 2) {
							mouse_print_packet(packet);

							event_t evt;
							printf("curr_state_x: %d\n", state.curr_position_x);
							printf("curr_state_y: %d\n\n", state.curr_position_y);

							state = generate_state(packet);

							printf("delta_x: %d\n", state.delta_x);
							printf("delta_y: %d\n\n", state.delta_y);

							printf("delta_x: %d\n", state.curr_position_x);
							printf("delta_y: %d\n\n", state.curr_position_y);

							if ((state.curr_position_x >= 296 && state.curr_position_x <= 666)
									&& (state.curr_position_y >= 541 && state.curr_position_y <= 593)) {
								if (state.r_button_state) {
									evt.type = MOVE;
									curr_state = PLAY;
									vg_game();
								}
							}

							count++;
						}
					}
				}
				if (msg.NOTIFY_ARG & timer_irq)
					timer_int_handler();

				if (msg.NOTIFY_ARG & irq_set) {
					int n_bytes = kb_int_handler(&scancode);
					kb_print_scancode(scancode, n_bytes);
					move_handler(scancode, &x, &y, &color);
					//set state machine in the last state
					if (scancode == DRIVER_END_SCODE)
						curr_state = END;
				}

				break;
			default:
				break;
			}
		}

		if(curr_state == PLAY)
		{
			vg_game();
		}

		//vg_draw_mouse_pointer(state.delta_x,state.delta_y);
		if (second_time == 0)
			if ((timer_get_ellapsed_time() - start_time) >= 5) {
				second_time = 1;
				vg_game();
				vg_draw_pixmap_queen(x+3, y+5, pixmap, width, height);
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
		vg_draw_pixmap_queen(*x+3, *y+5, pixmap, width, height);
		break;
	case 0xCB://left
		if(*x-81 < 251)
			break;
		vg_draw_rectangle(*x, *y, 82, 82, *color);
		*x -= 81;
		vg_draw_pixmap_queen(*x+3, *y+5, pixmap, width, height);
		break;
	case 0xC8: //up
		if(*y-81 < 35)
			break;
		vg_draw_rectangle(*x, *y, 82, 82, *color);
		*y -= 81;
		vg_draw_pixmap_queen(*x+3, *y+5, pixmap, width, height);
		break;
	case 0xD0: //down
		vg_draw_rectangle(*x, *y, 82, 82, *color);
		*y += 81;
		if(*y > 650)
			*y = 35;
		vg_draw_pixmap_queen(*x+3, *y+5, pixmap, width, height);
		break;
	case 0x1C: //ENTER
		//check algorithm
		*x = 251;
		*y = 35;
		*color = 63;

		pixmap = read_xpm(queen, &width, &height);
		vg_draw_pixmap_queen(*x+3, *y+5, pixmap, width, height);

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




