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
    unsigned int color)
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
  plot (surface, x, y, color);
  while (*v != *v2)
  {
    bresenham_adjustment (u,v, su, sv, du, dv, &frac);
    plot (surface, x, y, color);
  }
}

/*
 *  plotting functions
 */

#include "pixels.h"

void colourPixel (SDL_Surface *dst, int x, int y, unsigned color)
{
  setPixel (dst, x, y, color);
}

void invertPixel (SDL_Surface *dst, int x, int y, unsigned color)
{
  int x2 = x, y2 = y;
  SDL_Rect clip_rect;
  SDL_GetClipRect (canvas, &clip_rect);
  if (clip_rect.x != 0)
    x2 += clip_rect.x;
  if (clip_rect.y != 0)
    y2 += clip_rect.y;

  unsigned clr = getPixel (canvas, x2, y2);
  clr ^= 0x00ffffff;
  clr &= 0x00ffffff;
  setPixel (dst, x, y, clr | 0xff000000);
}

