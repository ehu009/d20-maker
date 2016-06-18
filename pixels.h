
#include		"SDL2/SDL.h"


#pragma once

uint32_t getPixel (SDL_Surface *src, int x, int y);

void setPixel(SDL_Surface *dst, int x, int y, unsigned color);

