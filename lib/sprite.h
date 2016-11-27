#ifndef __SPRITE_H
#define __SPRITE_H

typedef struct {
int x, y; // current position
int width, height;  // dimensions
int xspeed, yspeed; // current speed
char* map;          // the pixmap
} Sprite;

Sprite* create_sprite(char*pic[], int x, int y, int xspeed, int yspeed);

#endif /* __SPRITE_H */
