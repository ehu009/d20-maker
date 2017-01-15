

#include "lines.h"
#include "pixels.h"



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
    int *sA,int *sB,
    int *dA,int *dB,
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
    v2 = &x2;
    u = &y;     v = &x;
    su = &stepy;  sv = &stepx;
    du = &dy;   dv = &dx;
  }
  frac = *du - ((*dv) / 2);

  //  Drawing
  plot (surface, x, y, color);
  while (*v != *v2)
  {
    bresenham_adjustment (u,v,  su,sv,  du,dv,  &frac);
    plot (surface, x, y, color);
  }
}


void setInvPixel (SDL_Surface *dst, int x, int y, unsigned color)
{
  unsigned clr = getPixel (dst, x, y);
  clr ^= 0xffffff;
  setPixel (dst, x, y, clr);
}
