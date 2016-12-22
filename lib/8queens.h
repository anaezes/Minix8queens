#ifndef __8QUEENS_H
#define __8QUEENS_H

#include "video_gr.h"
#include "vbe.h"
#include "i8042.h"
#include "Keyboard.h"
#include "Driver.h"

typedef enum {INIT, PLAY, WIN, LOSE, END_PLAY, END} state_t;
typedef enum {INSTRUCTIONS, INIT_PLAY, MENU_EXIT} menu_option_t;


typedef struct game_st
{
	state_t curr_state;
	menu_option_t curr_option;
	int board[BOARD_SIZE][BOARD_SIZE];
	int n_queens;
	char* graphics_state;
	int xi, yi, widthR, heightR;
	unsigned int colorR;

} game_st;

typedef struct queens_st
{
	int x;
	int y;
	unsigned int color;
	int width;
	int height;
	char** queen;
	char* pixmap;

} queens_st;


game_st init_game();

queens_st init_queens();

int game_loop();

void start_game(game_st* game_state, queens_st* queens_state);

void highlight_menu_option(unsigned long scancode, game_st* game_state);

void switchColor(queens_st* queens_state);

void showGameOver();

void showYouWin();

void showSolution();

void showOptions();

/** @} end of proj */

#endif /* __PROJECT_H */
