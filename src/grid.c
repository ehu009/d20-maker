#include <stdlib.h>
#include "grid.h"
#include "vertex.h"
#include "pixels.h"

static struct 
{
  char rotation;
  double distance;
  vtx2i_t bounds, size;
  vtx2d_t offset;
  vtx2d_t **coords;
} grid;


void grid_rotate (void)
{
  grid.rotation ++;
  grid.rotation &= 1;
}

void calc_distance (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c)
{
  double d= 0;
  if (a->pts[0] == b->pts[0])
  {
    d = a->pts[1] - b->pts[1];
  }
  if (a->pts[0] == c->pts[0])
  {
    d = a->pts[1] - c->pts[1];
  }
  if (b->pts[0] == c->pts[0])
  {
    d = b->pts[1] - c->pts[1];
  }
  if (a->pts[1] == b->pts[1])
  {
    d = a->pts[0] - b->pts[0];
  }
  if (a->pts[1] == c->pts[1])
  {
    d = a->pts[0] - c->pts[0];
  }
  if (b->pts[1] == c->pts[1])
  {
    d = b->pts[0] - c->pts[0];
  }
  
  if (d < 0)
  {
    d *= -1;
  }
  grid.distance = d;
}

void calc_offset (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c)
{
  vtx2d_t *leftmost = a, *topmost = a;
  if (leftmost->pts[0] > b->pts[0])
  {
    leftmost = b;
  }
  if (leftmost->pts[0] > c->pts[0])
  {
    leftmost = c;
  }

  if (topmost->pts[1] > b->pts[1])
  {
    topmost = b;
  }
  if (topmost->pts[1] > c->pts[1])
  {
    topmost = c;
  }
  
  double x = grid.distance, y = x * sqrt(3.0);
  if (grid.rotation)
  {
    y = grid.distance;
    x = y * sqrt(3.0);
  }

  double *ptr =  &grid.offset.pts[0];
  *ptr = leftmost->pts[0];
  while (*ptr >= 0.0)
  {
    *ptr -= x;
  }
  *ptr += x;
  ptr =  &grid.offset.pts[1];
  *ptr = topmost->pts[1];
  while (*ptr >= 0.0)
  {
    *ptr -= y;
  }
  *ptr += y;
}

void calc_size (void)
{
  grid.size.pts[0] = (grid.bounds.pts[0] - grid.offset.pts[0]) / grid.distance;
  grid.size.pts[1] = (grid.bounds.pts[1] - grid.offset.pts[1]) / grid.distance;
}

void free_coordinates (vtx2d_t **coords)
{
  if (coords == NULL)
  {
    return;
  }
  unsigned int i, j;
  for (i = 0; i < grid.size.pts[0]; i ++)
  {
    if (coords[i] != NULL)
    {
      for (j = 0; j < grid.size.pts[1]; j ++)
      {
        free(coords[i][j]);
      }
      free(coords[i]);
    }
  }
  free(coords);
}

vtx2d_t **init_coordinates ()
{
  char error = 0;
  vtx2d_t **new = (vtx2d_t **) malloc((sizeof(vtx2d_t **)) * grid.size.pts[0]);
  if (new == NULL)
  {
    return NULL;
  }
  unsigned int i;
  for (i = 0; i < grid.size.pts[0]; i ++)
  {
    new[i] = NULL;
  }
  for (i = 0; i < grid.size.pts[0]; i ++)
  {
    vtx2d_t **col = (vtx2d_t **) malloc((sizeof(vtx2d_t *)) * grid.size.pts[1]);
    if (col == NULL)
    {
      error ++;
      break;
    }
    unsigned int j = 0;
    for (; j < grid.size.pts[1]; j ++)
    {
      new[i][j] = NULL;
    }
    new[i] = col;
  }
  if (error)
  {
    free_coordinates(new);
    new = NULL;
  }
  return new;
}

unsigned int get_row (vtx2d_t *coord)
{
  
}

unsigned int get_col (vtx2d_t *coord)
{

}

void populate_coordinates (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c)
{

}

void grid_init (vtx2i_t *bounds)
{
  grid.bounds.pts[0] = bounds->pts[0];
  grid.bounds.pts[1] = bounds->pts[1];
  grid.coords = NULL;
  grid.rotation = 0;
}

void grid_reset (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c)
{
  if (grid.coords != NULL)
  {
    free_coordinates(grid.coords);
  }
  calc_distance(a, b, c);
  calc_offset(a, b, c);
  calc_size();
  vtx2d_t **coords = init_coordinates();
  if (coords != NULL)
  {
    grid.coords = coords;
    populate_grid(a, b, c);
  }
}

void draw_rect (vtx2d_t *pos)
{
  #define DOT_SIZE 8
  int x = pos->pts[0], y = pos->pts[1];
  int i, j;
  for (i = x; i < x + DOT_SIZE; i ++)
  {
    for (j = y; j < y + DOT_SIZE; j ++)
    {
      setPixel(canvas, i, j, 0x00FF00);
    }
  }
}

void draw_grid (void)
{
  int i, j;
  for (i = 0; i < grid.size.pts[0]; i ++)
  {
    for (j = 0; j < grid.size.pts[1]; j ++)
    {
      if (grid.coords[i][j] != NULL)
      {
        draw_rect(grid.coords[i][j]);
      }
    }
  }
}