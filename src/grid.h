#ifndef GRID_H_
#define GRID_H_

#include "SDL2/SDL.h"
#include "vertex.h"

static struct grid;

void grid_init (vtx2i_t *bounds);

void grid_rotate (void);

void grid_reset (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c);

void grid_free (void);

extern SDL_Surface *canvas;
void draw_grid (void);

#endif  /*  GRID_H_ */