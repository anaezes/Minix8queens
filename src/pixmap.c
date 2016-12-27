#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/pixmap.h"
#include "../lib/vbe.h"

static pixmap_t *pixmaps;

/**
 * Static load of all xpm images needed
 * */
void load_pixmaps()
{
	int width, height;
	pixmaps = malloc(sizeof(pixmap_t) * N_PIXMAP);

	int i;
	for(i = 0; i < N_PIXMAP; i++) {
		pixmaps[i].pixmap = read_xpm(pixmap_get_image(i+1), &width, &height);
		pixmaps[i].width = width;
		pixmaps[i].height = height;
	}
}

pixmap_t get_pixmap(int pos)
{
	return pixmaps[pos-1];
}

char** pixmap_get_image(unsigned long xpm_code)
{

	if (xpm_code == 1)
		return queenW;
	else if(xpm_code == 2)
		return square;
	else if(xpm_code == 3)
		return logo;
	else if(xpm_code == 4)
		return menu;
	else if(xpm_code == 5)
		return logo1;
	else if(xpm_code == 6)
		return mouse;
	else if(xpm_code == 7)
		return gameOver;
	else if(xpm_code == 8)
		return win;
	else if(xpm_code == 9)
		return selected;
	else if(xpm_code == 10)
		return instructions;
	else if(xpm_code == 11)
		return instructions1;
	else if(xpm_code == 12)
		return instructions2;
	else if(xpm_code == 13)
		return instructions3;
	else
		return NULL;
}


/**
 *  @author Joao Cardoso (jcard@fe.up.pt) ????
 *  Added by pfs@fe.up.pt
 */

char *read_xpm(char *map[], int *wd, int *ht)
{
	int width, height, colors;
	char sym[256];
	int  col;
	int i, j;
	char *pix, *pixtmp, *tmp, *line;
	char symbol;

	/* read width, height, colors */
	if (sscanf(map[0],"%d %d %d", &width, &height, &colors) != 3) {
		printf("read_xpm: incorrect width, height, colors\n");
		return NULL;
	}
#ifdef DEBUG
	printf("%d %d %d\n", width, height, colors);
#endif
	if (width > H_RES || height > V_RES || colors > 256) {
		printf("read_xpm: incorrect width, height, colors\n");
		return NULL;
	}

	*wd = width;
	*ht = height;

	for (i=0; i<256; i++)
		sym[i] = 0;

	/* read symbols <-> colors */
	for (i=0; i<colors; i++) {
		if (sscanf(map[i+1], "%c %d", &symbol, &col) != 2) {
			printf("read_xpm: incorrect symbol, color at line %d\n", i+1);
			return NULL;
		}
#ifdef DEBUG
		printf("%c %d\n", symbol, col);
#endif
		if (col > 256) {
			printf("read_xpm: incorrect color at line %d\n", i+1);
			return NULL;
		}
		sym[col] = symbol;
	}

	/* allocate pixmap memory */
	pix = pixtmp = malloc(width*height);

	/* parse each pixmap symbol line */
	for (i=0; i<height; i++) {
		line = map[colors+1+i];
#ifdef DEBUG
		printf("\nparsing %s\n", line);
#endif
		for (j=0; j<width; j++) {
			tmp = memchr(sym, line[j], 256);  /* find color of each symbol */
			if (tmp == NULL) {
				printf("read_xpm: incorrect symbol at line %d, col %d\n", colors+i+1, j);
				return NULL;
			}
			*pixtmp++ = tmp - sym; /* pointer arithmetic! back to books :-) */
#ifdef DEBUG
			printf("%c:%d ", line[j], tmp-sym);
#endif
		}
	}

	return pix;
}
