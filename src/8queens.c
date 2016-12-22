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
#include "../lib/algorithm.h"

unsigned long packet[MOUSE_PACKET_SIZE];
unsigned long config[MOUSE_CONFIG_SIZE];
unsigned int pos = 0;

game_st init_game()
{
	game_st state;
	state.curr_state = INIT;
	state.graphics_state = NULL;
	state.n_queens = 0;

	int i, j;
	for(i = 0; i < BOARD_SIZE; i++)
		for(j = 0; j < BOARD_SIZE; j++)
			state.board[i][j] = 0;

	//init time bar
	state.xi = 30;
	state.yi = 716;
	state.widthR = 964;
	state.heightR = 30;
	state.colorR = 6;

	return state;
}


queens_st init_queens()
{
	queens_st state;

	state.x = X_INIT_QUEEN;
	state.y = Y_INIT_QUEEN;
	state.color = COLOR_WHITE;
	state.width;
	state.height;
	state.queen = pixmap_get_image(1);
	state.pixmap = read_xpm(state.queen, &state.width, &state.height);

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
	queens_st queens_state = init_queens();

	//game_state.graphics_state = vg_get_area_state(MOUSE_INIT_X, MOUSE_INIT_Y,  MOUSE_WIDTH, MOUSE_HEIGHT);
	int start_time = timer_get_ellapsed_time();
	mouse_state state = init_mouse_state();

	// if graphics need to be repainted
	int graphics_invalidated = 0;

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


					if(((packet[0] & BIT(3)) && (packet[0] != KBC_ACK)) != 0) {
						pos = (pos + 1) % MOUSE_PACKET_SIZE;

//						if (pos == 2) {
//							//vg_reset_area_state(game_state.graphics_state, state.curr_position_x, state.curr_position_y,  MOUSE_WIDTH, MOUSE_HEIGHT);
//							update_mouse_state(&state, packet);
//							graphics_invalidated = 1;
//
//							if(mouse_interrupt_handler(&game_state, &state) == 1)
//								start_time = timer_get_ellapsed_time();
//						}
					}
				}


				if (msg.NOTIFY_ARG & timer_irq)
					timer_int_handler();


				if (msg.NOTIFY_ARG & irq_set) {
					int n_bytes = kb_int_handler(&scancode);
					kb_print_scancode(scancode, n_bytes);
					//set state machine in the last state
					if (game_state.curr_state == INIT && scancode == DRIVER_END_SCODE)
						game_state.curr_state = END;

					else if(game_state.curr_state != INIT && scancode == DRIVER_END_SCODE)
					{
						game_state.curr_state = INIT;
						vg_start();
					}

					else if (game_state.curr_state == INIT && scancode == 0x1C)
					{
						game_state = init_game();
						game_state.curr_state = PLAY;
						queens_state = init_queens();
						start_time = timer_get_ellapsed_time();
						state = init_mouse_state();

						// draw board
						vg_game();

						// draw time bar
						vg_draw_rectangle(game_state.xi, game_state.yi, game_state.widthR, game_state.heightR, game_state.colorR);

						// draw queen
						// draw queen
						vg_draw_pixmap(queens_state.x+3, queens_state.y+5, queens_state.pixmap, queens_state.width, queens_state.height);

					}
					else if(game_state.curr_state == PLAY && scancode == 0x93)
					{
						//init all values
						game_state = init_game();
						queens_state = init_queens();
						start_time = timer_get_ellapsed_time();
						state = init_mouse_state();

						game_state.curr_state = PLAY;

						// draw board
						vg_game();

						// draw time bar
						vg_draw_rectangle(game_state.xi, game_state.yi, game_state.widthR, game_state.heightR, game_state.colorR);

						// draw queen
						vg_draw_pixmap(queens_state.x+3, queens_state.y+5, queens_state.pixmap, queens_state.width, queens_state.height);
					}
					else if(game_state.curr_state == PLAY)
						move_handler(scancode, &queens_state, &game_state);



					break;
			default:
				break;
				}
			}
		}

		//		if(graphics_invalidated == 1)
		//		{
		//			//free(game_state.graphics_state);
		//			//game_state.graphics_state = vg_get_area_state(state.curr_position_x, state.curr_position_y,  MOUSE_WIDTH, MOUSE_HEIGHT);
		//			//vg_draw_mouse_pointer(state.curr_position_x,state.curr_position_y);
		//			graphics_invalidated = 0;
		//		}

		if(game_state.curr_state == PLAY)
		{
			if((timer_get_ellapsed_time() - start_time) == 1)
			{
				// update time bar
				if(game_state.widthR > 0)
				{
					game_state.xi += 5;
					game_state.widthR -= 5;

					vg_draw_rectangle(30, 716, 964, 30, 56);
					if(game_state.widthR < 0)
						game_state.widthR = 0;
					vg_draw_rectangle(game_state.xi, game_state.yi, game_state.widthR, game_state.heightR, game_state.colorR);
					start_time = timer_get_ellapsed_time();
				}
				else
					game_state.curr_state = LOSE;
			}
		}

		if(game_state.curr_state == LOSE)
		{
			if((timer_get_ellapsed_time() - start_time) <= 5)
				showGameOver();
			else
			{
				showSolution();
				game_state.curr_state = END_PLAY;
			}
		}

		if(game_state.curr_state == WIN)
		{
			showYouWin();
			game_state.curr_state = END_PLAY;
		}


		if(game_state.curr_state == END_PLAY)
			showOptions();


	}

	vg_exit();
	kb_unsubscribe_int();
	timer_unsubscribe_int();

	if (kbc_write_to_mouse(MOUSE_DISABLE_DR) != 0)
		return 1;

	mouse_unsubscribe_int();

	return 0;
}

int mouse_interrupt_handler(game_st* game_state, mouse_state* mouse)
{
	if(game_state->curr_state == INIT)
	{
		if (mouse->l_button_state == 1)
		{
			if ((mouse->curr_position_x >= 296 && mouse->curr_position_x <= 666) &&
					(mouse->curr_position_y >= 537 && mouse->curr_position_y <= 590))
			{
				game_state->curr_state = PLAY;
				return 1;
			}
		}
	}
	return 0;
}


int move_handler(unsigned long code, queens_st* queens_state, game_st* game_state) {

	//	int width;
	//	int height;
	//	char** queen = pixmap_get_image(1);
	//	char* pixmap = read_xpm(queen, &width, &height);

	int x_coord;
	int y_coord;

	int tmp_x = queens_state->x;
	int tmp_y = queens_state->y;

	switch (code) {
	case 0xCD: //right
		printf("right\n");
		tmp_x += 81;
		if(tmp_x > 850)
			tmp_x = X_INIT_QUEEN;

		x_coord = (tmp_x - X_INIT_QUEEN)/81;
		y_coord = (tmp_y - Y_INIT_QUEEN)/81;
		if(game_state->board[y_coord][x_coord] != 1)
		{
			vg_draw_rectangle(queens_state->x, queens_state->y, SQUARE_SIZE, SQUARE_SIZE, queens_state->color);
			queens_state->x += 81;
			if(queens_state->x > 850)
				queens_state->x = X_INIT_QUEEN;

			vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->pixmap, queens_state->width, queens_state->height);
			switchColor(queens_state);
		}
		break;
	case 0xCB://left
		printf("left\n");
		if(tmp_x-81 < 251)
			break;
		tmp_x -= 81;

		x_coord = (tmp_x - X_INIT_QUEEN)/81;
		y_coord = (tmp_y - Y_INIT_QUEEN)/81;
		if(game_state->board[y_coord][x_coord] != 1)
		{
			vg_draw_rectangle(queens_state->x, queens_state->y, SQUARE_SIZE, SQUARE_SIZE, queens_state->color);

			if(queens_state->x-81 < 251)
				break;
			queens_state->x -= 81;

			vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->pixmap, queens_state->width, queens_state->height);
			switchColor(queens_state);
		}
		break;
	case 0xC8: //up
		printf("up\n");
		if(tmp_y-81 < Y_INIT_QUEEN)
			break;
		tmp_y -= 81;

		x_coord = (tmp_x - X_INIT_QUEEN)/81;
		y_coord = (tmp_y - Y_INIT_QUEEN)/81;
		if(game_state->board[y_coord][x_coord] != 1)
		{
			vg_draw_rectangle(queens_state->x, queens_state->y, SQUARE_SIZE, SQUARE_SIZE, queens_state->color);

			if(queens_state->y-81 < Y_INIT_QUEEN)
				break;
			queens_state->y -= 81;

			vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->pixmap, queens_state->width, queens_state->height);
			switchColor(queens_state);
		}
		break;
	case 0xD0: //down
		printf("down\n");
		tmp_y += 81;
		if(tmp_y > 650)
			tmp_y = Y_INIT_QUEEN;

		x_coord = (tmp_x - X_INIT_QUEEN)/81;
		y_coord = (tmp_y - Y_INIT_QUEEN)/81;
		if(game_state->board[y_coord][x_coord] != 1)
		{
			vg_draw_rectangle(queens_state->x, queens_state->y, SQUARE_SIZE, SQUARE_SIZE, queens_state->color);

			queens_state->y += 81;
			if(queens_state->y > 650)
				queens_state->y = Y_INIT_QUEEN;

			vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->pixmap, queens_state->width, queens_state->height);
			switchColor(queens_state);
		}

		break;
	case 0x1C: //ENTER
		//check algorithm

		x_coord = (queens_state->x - X_INIT_QUEEN)/81;
		y_coord = (queens_state->y - Y_INIT_QUEEN)/81;

		game_state->board[y_coord][x_coord] = 1;
		printBoard(game_state->board);
		if(!is_valid(game_state->board, x_coord, y_coord))
		{
			game_state->board[y_coord][x_coord] = 0;
			vg_draw_rectangle(queens_state->x, queens_state->y, SQUARE_SIZE, SQUARE_SIZE, COLOR_RED);
			vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->pixmap, queens_state->width, queens_state->height);
		}
		else
		{
			//inicial position of queen
			game_state->n_queens++;

			if(game_state->n_queens < N_QUEENS)
			{
				if(game_state->board[0][0] == 1)
				{
					queens_state->x = X_INIT_QUEEN+81;
					queens_state->color = COLOR_DARK_GREY;
				}
				else
				{
					queens_state->x = X_INIT_QUEEN;
					queens_state->color = COLOR_WHITE;
				}

				queens_state->y = Y_INIT_QUEEN;
				vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->pixmap, queens_state->width, queens_state->height);
			}
			else
				game_state->curr_state = WIN;
		}
		return 0;

		break;
	default:
		return 0;
	}
	return 0;
}

void switchColor(queens_st* queens_state)
{
	if(queens_state->color == COLOR_DARK_GREY)
		queens_state->color = COLOR_WHITE;
	else
		queens_state->color = COLOR_DARK_GREY;
}

void showGameOver()
{
	int width;
	int height;
	char** gameOver = pixmap_get_image(7);
	char* pixmap = read_xpm(gameOver, &width, &height);

	vg_draw_pixmap(297, 330, pixmap, width, height);
}

void showYouWin()
{
	int width;
	int height;
	char** win = pixmap_get_image(8);
	char* pixmap = read_xpm(win, &width, &height);

	vg_draw_pixmap(354, 338, pixmap, width, height);
}

void showOptions()
{
	//	int width;
	//	int height;
	//	char** win = pixmap_get_image(8);
	//	char* pixmap = read_xpm(win, &width, &height);
	//
	//	vg_draw_pixmap(379, 338, pixmap, width, height);
}

void showSolution()
{
	vg_game();
	vg_draw_rectangle(30, 716, 964, 30, 56);


	int width;
	int height;
	char** queen = pixmap_get_image(1);
	char* pixmap = read_xpm(queen, &width, &height);


	int board[BOARD_SIZE][BOARD_SIZE];
	int i, j;
	for(i = 0; i < BOARD_SIZE; i++)
		for(j = 0; j < BOARD_SIZE; j++)
			board[i][j] = 0;

	//put 8 queens
	board[0][3] = 1;
	board[1][1] = 1;
	board[2][6] = 1;
	board[3][4] = 1;
	board[4][0] = 1;
	board[5][7] = 1;
	board[6][5] = 1;
	board[7][2] = 1;

	int x, y;
	for(i = 0; i < BOARD_SIZE; i++)
	{
		for(j = 0; j < BOARD_SIZE; j++)
		{
			if(board[i][j] == 1)
			{
				x = (j*81)+X_INIT_QUEEN;
				y = (i*81)+Y_INIT_QUEEN;

				vg_draw_pixmap(x+3, y+5, pixmap, width, height);
			}
		}
	}
}
