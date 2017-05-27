#include "vertex.h"
#include <stdlib.h>

void get_vtx2i_from_vtx2d (vtx2d_t *src, vtx2i_t *dst)
{
  dst->pts[0] = (int) src->pts[0];
  dst->pts[1] = (int) src->pts[1];
}

vtx2i_t *make_vtx2i_from_vtx2d (vtx2d_t *src)
{
  vtx2i_t *ptr = calloc (1, sizeof (vtx2i_t));
  if (ptr != NULL)
  {
    ptr->pts[0] = (int) src->pts[0];
    ptr->pts[1] = (int) src->pts[1];
  }
  return ptr;
}
