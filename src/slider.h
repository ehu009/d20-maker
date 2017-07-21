#ifndef			UI_SLIDER_H_
#define			UI_SLIDER_H_

#include	<stdlib.h>
#include 	<stdio.h>
#include	<string.h>

#include	"SDL2/SDL.h"
#include	"SDL2/SDL_ttf.h"


struct UI_slide_bar;

typedef struct UI_slide_bar	UI_slider;


UI_slider *UI_slider_create (double start, int width, int height);

void 	UI_slider_free (UI_slider *slide);


void UI_slider_setPos (UI_slider *slide, int _x, int _y);

int UI_slider_setFont (UI_slider *slide, const char *font, unsigned font_size, SDL_Color font_color);

double UI_slider_update (UI_slider *slide, unsigned mouseX, unsigned mouseY, unsigned mouseIsDown, SDL_Surface *screen);



#endif		/*	UI_SLIDER_H_	*/
