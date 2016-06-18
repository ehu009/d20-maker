#include "SDL2/SDL.h"

#pragma once

extern int mouseX, mouseY;
extern unsigned mouseState;

void mouse_update (SDL_Event *event);

/*
 * returns non-zero if mouse has been clicked, otherwise zero
 */
int mouse_checkout (void);
