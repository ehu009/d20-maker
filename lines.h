
#include "SDL2/SDL.h"

#pragma once

typedef void (*plot_func) (SDL_Surface *, int, int, unsigned);



void draw_line (SDL_Surface *surface,
    int x1, int y1,
    int x2, int y2,
    plot_func plot,
    unsigned int color);



void setInvPixel (SDL_Surface *dst, int x, int y, unsigned color);
