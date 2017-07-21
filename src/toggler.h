#ifndef			UI_TOGGLE_H_
#define			UI_TOGGLE_H_

#include	<stdlib.h>
// #include 	<stdio.h>
#include	<string.h>

#include	"SDL2/SDL.h"
#include	"SDL2/SDL_ttf.h"


struct UI_toggle_switch;

typedef struct UI_toggle_switch UI_toggler;


UI_toggler *UI_toggler_create (const char *txt1, const char *txt2, unsigned start);

void UI_toggler_free (UI_toggler *toggler);

void UI_toggler_setPosition (UI_toggler *button, unsigned _x, unsigned _y);

unsigned UI_toggler_update (UI_toggler *button, SDL_Surface *screen, unsigned mouseX, unsigned mouseY, unsigned mouseIsDown);

#endif		/*	UI_TOGGLE_H_	*/
