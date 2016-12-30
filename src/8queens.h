#ifndef __8QUEENS_H
#define __8QUEENS_H

#include "video_gr.h"
#include "vbe.h"
#include "i8042.h"
#include "Keyboard.h"
#include "Driver.h"
#include "pixmap.h"
#include "sprite.h"
#include "Mouse.h"
#include "algorithm.h"
#include "rtc.h"


/**
 * Game states
 **/
typedef enum {INIT, SHOW_INSTRUCTIONS, PLAY, WIN, LOSE, SOLUTION, END} state_t;


/**
 * Menu options states
 **/
typedef enum {INSTRUCTIONS, INIT_PLAY, MENU_EXIT} menu_option_t;


/**
 * Struct that contains a description of the current game state.
 * Also contains the matrix that represents the game board, as well
 * as the graphics state.
 **/
typedef struct game_st
{
	state_t curr_state;
	menu_option_t curr_option;
	int board[BOARD_SIZE][BOARD_SIZE];
	int n_queens;
	char* graphics_state;
	int xi, yi, widthR, heightR;
	unsigned int colorR;
	int previous_mouse_left_b;

} game_st;


/**
 * Struct that contains the state of the moveable queen.
 **/
typedef struct queens_st
{
	int x;
	int y;
	unsigned int color;
	pixmap_t px;

} queens_st;


/**
 * Init the game state struct
 * @return game_st struct
 **/
game_st init_game();


/**
 * Init the moveable queen
 * @return queens_st struct
 **/
queens_st init_queens();


/**
 * Main game loop, where interruptions are read and
 * the game state is handled
 * @return 1 when a subscription to an interruption fails
 **/
int game_loop();


/**
 * Returns the coordinate on the Y axis of the given menu option
 * @param menu option
 * @return y coordinate
 **/
int get_menu_y_coordinate(menu_option_t curr_option);


/**
 * Repaint the graphics.
 * @param current game state
 * @param current moveable queen state
 * @param current date as read in rtc
 * Called when the mouse moves
 **/
void repaint(game_st* game_state, queens_st* queens_state, date_t* date);


/**
 * Init the graphics state of the game
 * @param current game state
 * @param current moveable queen state
 **/
void start_game(game_st* game_state, queens_st* queens_state);

/**
 * Change game's state according to the selected menu option.
 * @param current game state
 * @param current mouse state
 * @return 0 when no menu option was clicked
 **/
int mouse_menu_click_handler(game_st* game_state, mouse_state* mouse);


/**
 * Highlight menu option with mouse hover
 * @param current game state
 * @param current mouse state
 * @return -1 when no menu option was hover
 **/
int mouse_menu_handler(game_st* game_state, mouse_state* mouse);


/**
 * Highlight currently selected menu option.
 * @param scancode of pressed key
 * @param current game state
 **/
void highlight_menu_option(unsigned long scancode, game_st* game_state);


/**
 * Keep track of board cell color, where the moveable queen is.
 * @param current moveable queen state
 **/
void switch_color(queens_st* queens_state);


/**
 * Handle mouse click while playing, and change game's state accordingly.
 * @param current mouse state
 * @param current moveable queen state
 * @param current game state
 **/
int mouse_click_handler(mouse_state* mouse, queens_st* queens_state, game_st* game_state);


/**
 * Maps mouse click coordinates to matrix coordinates, and save them in x_coord and y_coord.
 * @param current mouse state
 * @param matrix X coordinate
 * @param matrix Y coordinate
 * @return 0 if mouse click was outside of the game board
 **/
int get_board_coordinates(mouse_state* mouse, unsigned int* x_coord, unsigned int* y_coord);


/**
 * Handle keyboard while playing, and change game's state accordingly.
 * @param keyboard scancode
 * @param current moveable queen state
 * @param current game state
 **/
int kb_move_handler(unsigned long code, queens_st* queens_state, game_st* game_state);


/**
 * Display game over message.
 **/
void show_game_over();


/**
 * Display win message.
 **/
void show_you_win();


/**
 * Display possible solution.
 * Called when play time ends.
 **/
void show_solution();


/**
 * Display board game's state
 * @param current game state
 **/
void print_queens(game_st* game_state);


/**
 * Display time bar.
 * @param current game state
 **/
void print_time_bar(game_st* game_state);

/** @} end of proj */

#endif /* __PROJECT_H */
