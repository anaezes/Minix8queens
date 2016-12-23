#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <math.h>

#include "../lib/video_gr.h"



/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */

/* Private global variables */

static char *video_mem = VRAM_PHYS_ADDR;		/* Process address to which VRAM is mapped */
static unsigned h_res = H_RES;		/* Horizontal screen resolution in pixels */
static unsigned v_res = V_RES;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel = BITS_PER_PIXEL; /* Number of VRAM bits per pixel */
static long vram_size = (H_RES * V_RES);

vbe_mode_info_t definitions;

void *vg_init(unsigned short mode)
{


	struct reg86u r;
	r.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
	r.u.w.bx = LINEAR_MODEL_BIT | mode; // set bit 14: linear framebuffer
	r.u.b.intno = 0x10;

	vbe_get_mode_info(mode, &definitions);

	if( sys_int86(&r) == OK ) {

		//code in handout (part 5))
		int r;
		struct mem_range mr;

		/* Allow memory mapping */

		mr.mr_base = (definitions.PhysBasePtr);
		mr.mr_limit = mr.mr_base + vram_size;

		if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		{
			panic("sys_privctl (ADD_MEM) failed: %d\n", r);
			return (void *)1;
		}

		/* Map memory */

		video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

		if(video_mem == MAP_FAILED)
			panic("couldn't map video memory");

		if (video_mem == NULL)
		{
			printf("vg_init: Error!\n");
			vg_exit();
			return NULL;
		}

		return (void *)video_mem;
	}
	else
	{
		return (void *)1;
	}

}

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x00;    /* Set Video Mode function */
	reg86.u.b.al = 0x03;    /* 80x25 text mode*/

	if( sys_int86(&reg86) != OK ) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}


int vg_start()
{
	vg_draw_rectangle(0, 0, H_RES, V_RES, 64);

	//DRAW RECTANGLE LOGO
	vg_draw_rectangle(0, 100, 1024, 200, 56);

	//DRAW LOGO
	int width;
	int height;
	char** logo = pixmap_get_image(5);
	char* pixmap = read_xpm(logo, &width, &height);
	vg_draw_pixmap(50, 120, pixmap, width, height);

	//DRAW MENU
	char** menu = pixmap_get_image(4);
	pixmap = read_xpm(menu, &width, &height);
	vg_draw_pixmap(300, 400, pixmap, width, height);

	// RTC (?)

	return 0;
}


int vg_game()
{
	vg_draw_rectangle(0, 0, H_RES, V_RES, 0);

	//DRAW SMALL LOGO
	int width;
	int height;
	char** logo = pixmap_get_image(3);
	char* pixmap = read_xpm(logo, &width, &height);
	vg_draw_pixmap(25, 15, pixmap, width, height);

	vg_draw_rectangle(244, 30, 663, 663, 63);

	//DRAW MENU
	char** board= pixmap_get_image(2);
	char* pixmap1 = read_xpm(board, &width, &height);

	int xi = 250;
	int yi = 36;
	int i;
	int j;
	for(j = 0; j < 4; j++)
	{
		for(i = 0; i < 4; i++)
		{
			vg_draw_pixmap(xi, yi, pixmap1, width, height);
			xi += 162;
		}

		xi = 250;
		yi += 162;
	}

	return 0;
}
void show_selected_menu(int x, int y)
{

	int width;
	int height;
	char** selected = pixmap_get_image(9);
	char* pixmap = read_xpm(selected, &width, &height);
	vg_draw_pixmap(x,y, pixmap, width, height);
}

void show_instructions()
{
	vg_draw_rectangle(200, 350, 624, 300, COLOR_LIGHT_GREY);
	vg_draw_rectangle(202, 352, 620, 296, COLOR_BLACK);

	int width;
	int height;
	char** selected = pixmap_get_image(10);
	char* pixmap = read_xpm(selected, &width, &height);
	vg_draw_pixmap(210,380, pixmap, width, height);
}

void vg_draw_rectangle(unsigned short x, unsigned short y, unsigned short sizex, unsigned short sizey, unsigned long color)
{
	int i, j;

	char* video_copy = video_mem;
	video_copy += x + (y*H_RES);

	for(i = 0; i < sizey; i++)
	{
		for(j = 0; j < sizex; j++)
		{
			*video_copy = color;
			video_copy++;
		}
		video_copy = video_copy + H_RES - sizex;
	}
}

void vg_draw_mouse_pointer(int x, int y)
{
	int width;
	int height;
	char** mouse= pixmap_get_image(6);
	char* pixmap = read_xpm(mouse, &width, &height);
	vg_draw_pixmap(x, y, pixmap, width, height);

}

void set_pixel(unsigned short x, unsigned short y, unsigned long color)
{
	if(color != 2 )
	{
		char* video_copy = video_mem;
		video_copy = video_copy + ((unsigned short)x + (unsigned short)y * h_res) ;
		*video_copy=color;
	}

}

int get_pixel(unsigned short x, unsigned short y)
{
	//gives a color of a pixel
	return (video_mem + ((unsigned short)x + (unsigned short)y * h_res));
}


void vg_draw_pixmap(unsigned short xi, unsigned short yi, char* pixmap, int width, int height)
{
	int i = 0;
	while(i < width)
	{
		int j = 0;
		while(j < height)
		{

			set_pixel(xi + i, yi + j, *(pixmap + (i + j * width)));
			j++;
		}
		i++;
	}
}


//// returns the state of a rectangle of pixels
//int* vg_get_area_state(unsigned short xi, unsigned short yi, int width, int height)
//{
//	int* area_state = malloc(sizeof(int)*width*height);
//
//	int i, j;
//	for(i = 0; i < height; i++)
//		for(j = 0; j < width; j++)
//			area_state[j + i*width] = get_pixel(xi+j, yi+i);
//
//	return area_state;
//}
//
//
//// resets an area of pixels to the given state
//void vg_reset_area_state(int* area_state, unsigned short xi, unsigned short yi, int width, int height)
//{
//	int i, j;
//	for(i = 0; i < height; i++)
//		for(j = 0; j < width; j++)
//		{
//			printf("xi+j: %d\n", (xi+j));
//			printf("yi+i: %d\n", (yi+i));
//			printf("color: %d\n", area_state[j + i*width]);
//			printf("pos: %d\n", (j + i*width));
//
//			set_pixel(xi+j, yi+i, area_state[j + i*width]);
//		}
//}


