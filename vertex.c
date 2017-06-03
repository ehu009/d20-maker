#include "vertex.h"
#include <stdlib.h>

void get_vtx2i_from_vtx2d (vtx2d_t *src, vtx2i_t *dst)
{
  dst->pts[0] = (int) src->pts[0] + 0.5;
  dst->pts[1] = (int) src->pts[1] + 0.5;
}

vtx2i_t *make_vtx2i_from_vtx2d (vtx2d_t *src)
{
  vtx2i_t *ptr = calloc (1, sizeof (vtx2i_t));
  if (ptr != NULL)
  {
    get_vtx2i_from_vtx2d (src, ptr);
  }
  return ptr;
}
