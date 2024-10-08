
#include "vertex.h"

static struct 
{
  char rotation;
  double distance;
  vtx2i_t offset, bounds, size;
  vtx2d_t **coords;
} grid;


void grid_rotate (void)
{
  grid.rotation ++;
  grid.rotation &= 1;
}

void calc_distance (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c)
{
  
}

void calc_offset (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c)
{

}

void calc_size (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c)
{

}

void free_coordinates ()
{

}

vtx2d_t **new_coordinates ()
{

}

void grid_reset (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c)
{

}

