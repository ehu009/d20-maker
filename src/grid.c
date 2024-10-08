
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

void free_coordinates (vtx2d_t **coords)
{
  if (coords == NULL)
  {
    return;
  }
  unsigned int i;
  for (i = 0; i < grid.size.pts[0]; i ++)
  {
    if (coords[i] != NULL)
    {
      free(coords[i]);
    }
  }
  free(coords);
}

vtx2d_t **init_coordinates ()
{
  char error = 0;
  vtx2d_t **new = (vtx2d_t **) malloc((sizeof(vtx2d_t **)) * grid.size.pts[0]);
  if (new != NULL)
  {
    unsigned int i;
    for (i = 0; i < grid.size.pts[0]; i ++)
    {
      new[i] = NULL;
    }
    for (i = 0; i < grid.size.pts[0]; i ++)
    {
      vtx2d_t *col = (vtx2d_t *) calloc((sizeof(vtx2d_t *)) * grid.size.pts[1]);
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

unsigned int get_row (vtx2d_t * coord)
{

}

unsigned int get_col (vtx2d_t * coord)
{
  
}

void populate_coordinates (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c)
{

}

void grid_reset (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c)
{

}

