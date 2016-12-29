#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <math.h>

#include "8queens.h"

unsigned long packet[MOUSE_PACKET_SIZE];
unsigned long config[MOUSE_CONFIG_SIZE];
unsigned int pos = 0;

game_st init_game()
{
	game_st state;
	state.curr_state = INIT;
	state.curr_option = INSTRUCTIONS;
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
	state.px = get_pixmap(PXMAP_QUEEN2);

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
	load_pixmaps();
	vg_start();
	show_selected_menu(X_INIT_MENU, Y_INIT_MENU);

	game_st game_state = init_game();
	queens_st queens_state = init_queens();

	//RTC
	date_t curr_date = get_curr_date();
	show_date(&curr_date);
	int curr_time = timer_get_ellapsed_time();


	int start_time = timer_get_ellapsed_time();
	mouse_state state = init_mouse_state();

	// if graphics need to be repainted
	int graphics_invalidated = 0;
	int show_inst = 0;

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

						if (pos == 2) {
							update_mouse_state(&state, packet);
							graphics_invalidated = 1;

							if(mouse_menu_click_handler(&game_state, &state) == 1)
							{
								if(game_state.curr_option == INIT_PLAY)
								{
									game_state = init_game();
									queens_state = init_queens();
									start_time = timer_get_ellapsed_time();

									game_state.curr_state = PLAY;
									start_game(&game_state, &queens_state);
								}
								else if(game_state.curr_option == INSTRUCTIONS)
								{
									game_state.curr_state = SHOW_INSTRUCTIONS;
									show_inst = 0;
								}
								else if(game_state.curr_option == MENU_EXIT)
									game_state.curr_state = END;
							}

							//Show highlight in menu when pass the mouse
							int option = mouse_menu_handler(&game_state, &state);
							if(option == 1)
								show_selected_menu(X_INIT_MENU, get_menu_y_coordinate(INSTRUCTIONS));
							else if(option == 2)
								show_selected_menu(X_INIT_MENU, get_menu_y_coordinate(INIT_PLAY));
							else if(option == 3)
								show_selected_menu(X_INIT_MENU, get_menu_y_coordinate(MENU_EXIT));
						}
					}
				}


				if (msg.NOTIFY_ARG & timer_irq)
					timer_int_handler();

				if (msg.NOTIFY_ARG & irq_set) {
					int n_bytes = kb_int_handler(&scancode);
					kb_print_scancode(scancode, n_bytes);
					//set state machine in the last state
					if(game_state.curr_state == INIT &&
							((scancode == DRIVER_END_SCODE) || (game_state.curr_option == MENU_EXIT && scancode == KEY_ENTER)))
						game_state.curr_state = END;

					else if(game_state.curr_state != INIT && scancode == DRIVER_END_SCODE)
					{
						game_state.curr_state = INIT;
						vg_start();
						show_selected_menu(X_INIT_MENU, Y_INIT_MENU	);
					}
					// start or restart game
					else if ((game_state.curr_state == INIT && game_state.curr_option == INIT_PLAY && scancode == KEY_ENTER) ||
							(game_state.curr_state == PLAY && scancode == KEY_R) ||
							(game_state.curr_state == END_PLAY && scancode == KEY_R))
					{
						game_state = init_game();
						queens_state = init_queens();
						start_time = timer_get_ellapsed_time();
						state = init_mouse_state();

						game_state.curr_state = PLAY;
						start_game(&game_state, &queens_state);
					}
					else if(game_state.curr_state == INIT && game_state.curr_option == INSTRUCTIONS && scancode == KEY_ENTER)
					{
						game_state.curr_state = SHOW_INSTRUCTIONS;
						show_inst = 0;
					}
					else if(game_state.curr_state == PLAY)
						move_handler(scancode, &queens_state, &game_state);
					else if(game_state.curr_state == INIT && (scancode == KEY_DOWN || scancode == KEY_UP))
						highlight_menu_option(scancode, &game_state);

					break;
			default:
				break;
				}
			}
		}

		if(game_state.curr_state == INIT && graphics_invalidated == 1)
		{
			repaint(&game_state,&queens_state, &curr_date);
			vg_draw_mouse_pointer(state.curr_position_x,state.curr_position_y);
			graphics_invalidated = 0;
		}

		if(game_state.curr_state == SHOW_INSTRUCTIONS && graphics_invalidated == 1)
		{
			repaint(&game_state, &queens_state, &curr_date);
			vg_draw_mouse_pointer(state.curr_position_x,state.curr_position_y);
			graphics_invalidated = 0;
		}

		if(game_state.curr_state == PLAY)
		{
			if((timer_get_ellapsed_time() - start_time) == 1)
			{
				// update time bar
				if(game_state.widthR > 0)
				{
					game_state.xi += 5;
					game_state.widthR -= 5;
					print_time_bar(&game_state);
					start_time = timer_get_ellapsed_time();
				}
				else
					game_state.curr_state = LOSE;
			}
		}

		if(game_state.curr_state == PLAY &&  graphics_invalidated == 1)
		{
			repaint(&game_state, &queens_state, &curr_date);
			vg_draw_mouse_pointer(state.curr_position_x,state.curr_position_y);
			graphics_invalidated = 0;
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

		if((timer_get_ellapsed_time() - curr_time) >= 1)
		{
			curr_date = get_curr_date();
			curr_time = timer_get_ellapsed_time();

			if(game_state.curr_state == INIT || game_state.curr_state == SHOW_INSTRUCTIONS)
				show_date(&curr_date);
		}

		vg_display();
	}

	vg_exit();
	kb_unsubscribe_int();
	timer_unsubscribe_int();

	if (kbc_write_to_mouse(MOUSE_DISABLE_DR) != 0)
		return 1;

	mouse_unsubscribe_int();

	return 0;
}

void highlight_menu_option(unsigned long scancode, game_st* game_state)
{
	menu_option_t new_option;
	int moved = 0;


	if( game_state->curr_option == INIT_PLAY && scancode == KEY_DOWN)
	{
		moved = 1;
		new_option = MENU_EXIT;
	}
	else if(game_state->curr_option == INSTRUCTIONS && scancode == KEY_DOWN)
	{
		moved = 1;
		new_option = INIT_PLAY;
	}
	else if( game_state->curr_option == INIT_PLAY && scancode == KEY_UP)
	{
		moved = 1;
		new_option = INSTRUCTIONS;
	}
	else if( game_state->curr_option == MENU_EXIT && scancode == KEY_UP)
	{
		moved = 1;
		new_option = INIT_PLAY;
	}

	if(moved)
	{
		//clean previous menu selection
		vg_draw_rectangle(295, 395, 400, 220, 0);

		pixmap_t px = get_pixmap(PXMAP_MENU);
		vg_draw_pixmap(X_INIT_MENU, Y_INIT_MENU, px.pixmap, px.width, px.height);

		//new selected menu
		show_selected_menu(X_INIT_MENU, get_menu_y_coordinate(new_option));
		game_state->curr_option = new_option ;
	}
}


int get_menu_y_coordinate(menu_option_t curr_option)
{
	if(curr_option == INIT_PLAY)
		return Y_INIT_MENU + 76;

	else if(curr_option == INSTRUCTIONS)
		return Y_INIT_MENU;

	else if(curr_option == MENU_EXIT)
		return Y_INIT_MENU+2*76;
}


void repaint(game_st* game_state, queens_st* queens_state, date_t* date)
{
	if(game_state->curr_state == INIT)
	{
		vg_start();
		show_date(date);
		show_selected_menu(X_INIT_MENU, get_menu_y_coordinate(game_state->curr_option));
	}
	else if(game_state->curr_state == SHOW_INSTRUCTIONS)
	{
		vg_start();
		show_date(date);
		show_instructions();

	}
	else if(game_state->curr_state == PLAY)
	{
		vg_game();
		print_time_bar(game_state);
		print_queens(game_state);
		vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->px.pixmap, queens_state->px.width, queens_state->px.height);
	}
	else if(game_state->curr_state == WIN)
	{

	}
	else if(game_state->curr_state == LOSE)
	{

	}
	else if(game_state->curr_state == END_PLAY)
	{

	}
}


void start_game(game_st* game_state, queens_st* queens_state)
{
	// draw board
	vg_game();

	// draw time bar
	vg_draw_rectangle(game_state->xi, game_state->yi, game_state->widthR, game_state->heightR, game_state->colorR);

	// draw queen
	vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->px.pixmap, queens_state->px.width, queens_state->px.height);

}

/**
 * return 1 if mouse clicked
 * */
int mouse_menu_click_handler(game_st* game_state, mouse_state* mouse)
{
	if(game_state->curr_state == INIT)
	{
		if (mouse->l_button_state == 1)
		{
			if((mouse->curr_position_x >= 300 && mouse->curr_position_x <= 660) &&
					(mouse->curr_position_y >= 405 && mouse->curr_position_y <= 447))
			{
				game_state->curr_option = INSTRUCTIONS;
				return 1;
			}
			else if((mouse->curr_position_x >= 300 && mouse->curr_position_x <= 660) &&
					(mouse->curr_position_y >= 477 && mouse->curr_position_y <= 522))
			{
				game_state->curr_option = INIT_PLAY;
				return 1;
			}
			else if((mouse->curr_position_x >= 300 && mouse->curr_position_x <= 660) &&
					(mouse->curr_position_y >= 554 && mouse->curr_position_y <= 600))
			{
				game_state->curr_option = MENU_EXIT;
				return 1;
			}
		}
	}

	return 0;
}

int mouse_menu_handler(game_st* game_state, mouse_state* mouse)
{
	if(game_state->curr_state == INIT)
	{
		if((mouse->curr_position_x >= 300 && mouse->curr_position_x <= 660) &&
				(mouse->curr_position_y >= 405 && mouse->curr_position_y <= 447))
		{
			game_state->curr_option = INSTRUCTIONS;
			return 1;
		}
		else if((mouse->curr_position_x >= 300 && mouse->curr_position_x <= 660) &&
				(mouse->curr_position_y >= 477 && mouse->curr_position_y <= 522))
		{
			game_state->curr_option = INIT_PLAY;
			return 2;
		}
		else if((mouse->curr_position_x >= 300 && mouse->curr_position_x <= 660) &&
				(mouse->curr_position_y >= 554 && mouse->curr_position_y <= 600))
		{
			game_state->curr_option = MENU_EXIT;
			return 3;
		}
	}
	return -1;
}
int move_handler(unsigned long code, queens_st* queens_state, game_st* game_state) {

	int x_coord;
	int y_coord;

	int tmp_x = queens_state->x;
	int tmp_y = queens_state->y;

	switch (code) {
	case KEY_RIGHT:
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

			vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->px.pixmap, queens_state->px.width, queens_state->px.height);
			switchColor(queens_state);
		}
		break;
	case KEY_LEFT:
		if(tmp_x-81 < X_INIT_QUEEN)
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

			vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->px.pixmap, queens_state->px.width, queens_state->px.height);
			switchColor(queens_state);
		}
		break;
	case KEY_UP:
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

			vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->px.pixmap, queens_state->px.width, queens_state->px.height);
			switchColor(queens_state);
		}
		break;
	case KEY_DOWN: //down
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

			vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->px.pixmap, queens_state->px.width, queens_state->px.height);
			switchColor(queens_state);
		}

		break;
	case KEY_ENTER: //ENTER
		//check algorithm

		x_coord = (queens_state->x - X_INIT_QUEEN)/81;
		y_coord = (queens_state->y - Y_INIT_QUEEN)/81;

		game_state->board[y_coord][x_coord] = 1;
		printBoard(game_state->board);
		if(!is_valid(game_state->board, x_coord, y_coord))
		{
			game_state->board[y_coord][x_coord] = 0;
			vg_draw_rectangle(queens_state->x, queens_state->y, SQUARE_SIZE, SQUARE_SIZE, COLOR_RED);
			vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->px.pixmap, queens_state->px.width, queens_state->px.height);
		}
		else
		{
			pixmap_t px = get_pixmap(PXMAP_QUEEN);
			vg_draw_pixmap(queens_state->x+3, queens_state->y+5, px.pixmap, px.width, px.height);

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
				vg_draw_pixmap(queens_state->x+3, queens_state->y+5, queens_state->px.pixmap, queens_state->px.width, queens_state->px.height);
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
	pixmap_t px = get_pixmap(PXMAP_GAME_OVER);
	vg_draw_pixmap(297, 330, px.pixmap, px.width, px.height);
}

void showYouWin()
{
	pixmap_t px = get_pixmap(PXMAP_WIN);
	vg_draw_pixmap(354, 338, px.pixmap, px.width, px.height);
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

	pixmap_t px = get_pixmap(PXMAP_QUEEN);

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

				vg_draw_pixmap(x+3, y+5, px.pixmap, px.width, px.height);
			}
		}
	}
}

void print_queens(game_st* game_state)
{
	pixmap_t px = get_pixmap(PXMAP_QUEEN);
	int i, j;
	int x, y;
	for(i = 0; i < BOARD_SIZE; i++)
	{
		for(j = 0; j < BOARD_SIZE; j++)
		{
			if(game_state->board[i][j] == 1)
			{
				x = (j*81)+X_INIT_QUEEN;
				y = (i*81)+Y_INIT_QUEEN;

				vg_draw_pixmap(x+3, y+5, px.pixmap, px.width, px.height);
			}
		}
	}
}

void print_time_bar(game_st* game_state)
{
	vg_draw_rectangle(30, 716, 964, 30, 56);
	if(game_state->widthR < 0)
		game_state->widthR = 0;
	vg_draw_rectangle(game_state->xi, game_state->yi, game_state->widthR, game_state->heightR, game_state->colorR);
}
