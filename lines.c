#include "lines.h"


static void unsign (int *value, int *buf)
{ /*  if value is negative
    do buf = sign(value), value = abs(value)*2  */
  if (*value < 0)
  {
    *value = - (*value);
    *buf = -1;
  }
  *value = (*value) * 2;
}

static void bresenham_adjustment (  int *A, int *B,
    int *sA, int *sB,
    int *dA, int *dB,
    int *fraction)
{ /*  main Bresenham function */
  if (*fraction >= 0)
  {
    *A = *A + *sA;
    *fraction = *fraction - *dB;
  }
  *B = *B + *sB;
  *fraction = *fraction + *dA;
}


void draw_line (SDL_Surface *surface,
    int x1, int y1,
    int x2, int y2,
    plot_func plot,
    COLOR color)
{ //  Bresenham algorithm
  int frac;
  int x = x1, dx = x2 - x1, stepx = 1,
      y = y1, dy = y2 - y1, stepy = 1;

  int *u = &x, *du = &dx, *su = &stepx,
      *v = &y, *dv = &dy, *sv = &stepy,
      *v2 = &y2;

  unsign (du, su);
  unsign (dv, sv);

  if (dx > dy)
  {
    u = &y;     v = &x;
    su = &stepy;sv = &stepx;
    du = &dy;   dv = &dx;
    v2 = &x2;
  }
  frac = *du - ((*dv) / 2);

  //  Drawing
  vtx2_t p = {.pts={x,y}};
  plot (surface, &p, color);
  while (*v != *v2)
  {
    p.pts[0] = x;
    p.pts[1] = y;
    bresenham_adjustment (u,v, su, sv, du, dv, &frac);
    plot (surface, &p, color);
  }
}


void draw_line2 (SDL_Surface *surface,
    vtx2_t *A, vtx2_t *B,
    plot_func plot,
    COLOR color)
{ //  Bresenham algorithm
  int frac;
  vtx2_t p1 = *A, p2 = *B;
  int x = p1.pts[0], dx = p2.pts[0] - x, stepx = 1,
      y = p1.pts[1], dy = p2.pts[1] - y, stepy = 1;

  int *u = &x, *du = &dx, *su = &stepx,
      *v = &y, *dv = &dy, *sv = &stepy,
      *v2 = &p2.pts[1];

  unsign (du, su);
  unsign (dv, sv);

  if (dx > dy)
  {
    u = &y;     v = &x;
    su = &stepy;sv = &stepx;
    du = &dy;   dv = &dx;
    v2 = &p2.pts[0];
  }
  frac = *du - ((*dv) / 2);

  //  Drawing
  vtx2_t p = {.pts={x,y}};
  plot (surface, &p, color);
  while (*v != *v2)
  {
    p.pts[0] = x;
    p.pts[1] = y;
    bresenham_adjustment (u,v, su, sv, du, dv, &frac);
    plot (surface, &p, color);
  }
}



/*
 *  plotting functions
 */


void colourPixel (SDL_Surface *dst, vtx2_t *p, unsigned color)
{
  setPixel (dst, p, color);
}

void invertPixel (SDL_Surface *dst, vtx2_t *p, unsigned color)
{
  vtx2_t p2 = *p;
  SDL_Rect clip_rect;
  SDL_GetClipRect (canvas, &clip_rect);
  if (clip_rect.x != 0)
    p2.pts[0] += clip_rect.x;
  if (clip_rect.y != 0)
    p2.pts[1] += clip_rect.y;

  unsigned clr = getPixel (canvas, &p2);
  clr ^= 0x00ffffff;
  clr &= 0x00ffffff;
  setPixel (dst, p, clr | 0xff000000);
}

