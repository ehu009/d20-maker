#ifndef GRID_H_
#define GRID_H_


#include "vertex.h"

static struct grid;

void grid_set_bounds (vtx2i_t bounds);

void grid_rotate (void);

void grid_reset (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c);


#endif  /*  GRID_H_ */