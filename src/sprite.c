#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>


#include "sprite.h"


/**Creates a new sprite with pixmap "pic", with specified
 *position (within the screen limits) and speed;
 *Does not draw the sprite on the screen
 *returns NULL on invalid pixmap.
 */
Sprite*create_sprite(char*pic[], int x, int y, int xspeed, int yspeed) {
	//allocate space for the "object"
	Sprite*sp = (Sprite*) malloc ( sizeof(Sprite));
	if( sp == NULL )
		return NULL;
	// read the sprite pixmap
	sp->map = read_xpm(pic, &(sp->width), &(sp->height));
	if( sp->map == NULL ) {
		free(sp);
		return NULL;
	}

	sp->x = x;
	sp->y = y;
	sp->xspeed=xspeed;
	sp->yspeed=yspeed;
	return sp;
}


void destroy_sprite(Sprite* sp)
{
	if( sp == NULL )
		return;
	free(sp->map);
	free(sp);
	sp = NULL;

}

int animate_sprite(Sprite* sp) {
	return 0;
}


/*Some useful non-visible functions*/
static int draw_sprite(Sprite*sp, char* base) {
	return 0;
}


static int check_collision(Sprite*sp, char* base) {
	return 1;
}
