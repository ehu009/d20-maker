#ifndef     TOGGLE_H_
#define     TOGGLE_H_

#include  <stdlib.h>
// #include   <stdio.h>
#include  <string.h>

#include  "SDL2/SDL.h"
#include  "SDL2/SDL_ttf.h"


struct toggle_switch;

typedef struct toggle_switch toggler_t;


toggler_t *toggler_create (const char *txt1, const char *txt2, unsigned start);

void toggler_free (toggler_t *toggler);

void toggler_setPosition (toggler_t *button, unsigned _x, unsigned _y);

unsigned toggler_update (toggler_t *button, SDL_Surface *screen, unsigned mouseX, unsigned mouseY, unsigned mouseIsDown);

#endif    /*  TOGGLE_H_ */
