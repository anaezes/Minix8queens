#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#include "vbe.h"
#include "sprite.h"

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

int vg_start();

int vg_game();

void vg_draw_rectangle(unsigned short x, unsigned short y, unsigned short sizex, unsigned short sizey, unsigned long color);

void vg_draw_mouse_pointer(int x, int y);

void set_pixel(unsigned short x, unsigned short y, unsigned long color);

void vg_draw_line(unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color);

void vg_draw_pixmap(unsigned short xi, unsigned short yi, char* pixmap, int width, int height);

void vg_draw_pixmap_queen(unsigned short xi, unsigned short yi, char* pixmap, int width, int height);

void vg_reset_area_state(int* area_state, unsigned short xi, unsigned short yi, int width, int height);

int* vg_get_area_state(unsigned short xi, unsigned short yi, int width, int height);

//void vg_clear_screen(char* video_copy);
//
//char* vg_get_copy_screen(char* video_mem1);

#endif /* __VIDEO_GR_H */
