#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#include "vbe.h"
#include "pixmap.h"

//#include "lmlib.h"

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);


/**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);


/**
 * Display initial graphics state
 * @return 0 if success
 * */
int vg_start();


/**
 * Display game graphics state
 * @return 0 if success
 * */
int vg_game();


/**
 * Display box around selected menu option
 * @param menu option x coordinate
 * @param menu option y coordinate
 * */
void show_selected_menu(int x, int y);


/**
 * Display game instructions
 * */
void show_instructions();


/**
 * Display rectangle
 * @param x coordinate
 * @param y coordinate
 * @param width
 * @param height
 * @param color
 * */
void vg_draw_rectangle(unsigned short x, unsigned short y, unsigned short sizex, unsigned short sizey, unsigned long color);


/**
 * Display mouse pointer
 * @param x coordinate
 * @param y coordinate
 * */
void vg_draw_mouse_pointer(int x, int y);


/**
 * Set the given pixel with the given color
 * @param x coordinate
 * @param y coordinate
 * @param color
 * */
void set_pixel(unsigned short x, unsigned short y, unsigned long color);


/**
 * Display the given pixmap
 * @param x coordinate
 * @param y coordinate
 * @param pixmap
 * @param width
 * @param height
 * */
void vg_draw_pixmap(unsigned short xi, unsigned short yi, char* pixmap, int width, int height);


/**
 * Copy double buffer to vram
 * */
void vg_display();

#endif /* __VIDEO_GR_H */
