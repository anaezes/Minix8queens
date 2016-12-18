#ifndef __8QUEENS_H
#define __8QUEENS_H

#include "video_gr.h"
#include "vbe.h"
#include "i8042.h"
#include "Keyboard.h"
#include "Driver.h"

typedef enum {INIT, PLAY, END} state_t;
typedef enum {RDOWN, RUP, MOVE} ev_type_t;
#define BOARD_SIZE 8

typedef struct event_t
{
	ev_type_t type;
} event_t;

typedef struct game_state
{
	state_t curr_state;
	int board[BOARD_SIZE][BOARD_SIZE];
	int n_queens;
} game_state;


game_state init_game();

int game_loop();

/** @} end of lab5 */


#endif /* __PROJECT_H */
