#ifndef __8QUEENS_H
#define __8QUEENS_H

#include "video_gr.h"
#include "vbe.h"
#include "i8042.h"
#include "Keyboard.h"
#include "Driver.h"

typedef enum {INIT, PLAY, WIN, LOSE, END_PLAY, END} state_t;
typedef enum {RDOWN, RUP, MOVE} ev_type_t;
#define BOARD_SIZE 8

typedef struct event_t
{
	ev_type_t type;
} event_t;

typedef struct game_st
{
	state_t curr_state;
	int board[BOARD_SIZE][BOARD_SIZE];
	int n_queens;
	char* graphics_state;
	int xi, yi, widthR, heightR;
	unsigned int colorR;

} game_st;

game_st init_game();

int game_loop();

void switchColor(unsigned int *color);

void showGameOver();

void showYouWin();

void showSolution();

void showOptions();

/** @} end of proj */

#endif /* __PROJECT_H */
