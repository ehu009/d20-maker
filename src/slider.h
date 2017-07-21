#ifndef     SLIDER_H_
#define     SLIDER_H_

#include  <stdlib.h>
#include  <stdio.h>
#include  <string.h>

#include  "SDL2/SDL.h"
#include  "SDL2/SDL_ttf.h"



struct slide_bar;

typedef struct slide_bar  slider_t;


slider_t *slider_create (double start, int width, int height);

void  slider_free (slider_t *slide);


void slider_setPos (slider_t *slide, int _x, int _y);

int slider_setFont (slider_t *slide, const char *font, unsigned font_size, SDL_Color font_color);

double slider_update (slider_t *slide, unsigned mouseX, unsigned mouseY, unsigned mouseIsDown, SDL_Surface *screen);



#endif    /*  SLIDER_H_  */
