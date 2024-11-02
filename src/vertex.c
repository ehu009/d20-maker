#include <stdlib.h>
#include <math.h>

#include "memory.h"
#include "vertex.h"

void get_vtx2i_from_vtx2d (vtx2d_t *src, vtx2i_t *dst)
{ //  Transfer FP two-point vertex value to a two-point integer vertex
  dst->pts[0] = (int) src->pts[0] + 0.5;
  dst->pts[1] = (int) src->pts[1] + 0.5;
}


vtx2i_t *make_vtx2i_from_vtx2d (vtx2d_t *src)
{ //  Create two-point integer vertex from existing two-point FP vertex
  vtx2i_t *ptr = (vtx2i_t *) CALLOC (1, sizeof (vtx2i_t));
  if (ptr != NULL)
  {
    get_vtx2i_from_vtx2d (src, ptr);
  }
  return ptr;
}


double distance (vtx2d_t *A, vtx2d_t *B)
{ //  Pythagorean root of squared sides summed
  double dx = B->pts[0] - A->pts[0], dy = B->pts[1] - A->pts[1];
  dx *= dx;
  dy *= dy;
  return sqrt (dx + dy);
}


int equal_vertices (vtx2d_t *A, vtx2d_t *B, double acc)
{
  return (distance(A, B) <= acc);
}

vtx2d_t *find_vector_opposing (vtx2d_t *anchor1, vtx2d_t *anchor2, vtx2d_t *point)
{
  vtx2d_t middle = {.pts = {anchor2->pts[0] - anchor1->pts[0], anchor2->pts[1] - anchor1->pts[1]}};
  middle.pts[0] /= 2;
  middle.pts[1] /= 2;
  middle.pts[0] += anchor1->pts[0];
  middle.pts[1] += anchor1->pts[1];

  vtx2d_t *vector = MALLOC (sizeof(vtx2d_t));
  *vector = (vtx2d_t) {.pts = {point->pts[0] - middle.pts[0], point->pts[1] - middle.pts[1]}};
  vector->pts[0] *= -1;
  vector->pts[1] *= -1;
  vector->pts[0] += middle.pts[0];
  vector->pts[1] += middle.pts[1];
  return vector;
}
