#include "lines.h"


double *colour_negate_rate = NULL;


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


void _draw_line (SDL_Surface *surface,
    int x1, int y1,
    int x2, int y2,
    plot_func plot,
    COLOUR color)
{ //  Bresenham algorithm
  int frac;
  int x = x1, dx = x2 - x1, stepx = 1,
      y = y1, dy = y2 - y1, stepy = 1;

  int *u = &x, *du = &dx, *su = &stepx,
      *v = &y, *dv = &dy, *sv = &stepy,
      *v2 = &y2;

  unsign(du, su);
  unsign(dv, sv);

  if (dx > dy)
  {
    u = &y;     v = &x;
    su = &stepy;sv = &stepx;
    du = &dy;   dv = &dx;
    v2 = &x2;
  }
  frac = *du - ((*dv) / 2);

  //  Drawing
  vtx2i_t p = {.pts={x,y}};
  plot(surface, &p, color);
  while (*v != *v2)
  {
    p.pts[0] = x;
    p.pts[1] = y;
    bresenham_adjustment(u,v, su, sv, du, dv, &frac);
    plot(surface, &p, color);
  }
}

void draw_line (SDL_Surface *surface,
    vtx2i_t *A, vtx2i_t *B,
    plot_func plot,
    COLOUR color)
{
  _draw_line(surface, A->pts[0], A->pts[1], B->pts[0], B->pts[1], plot, color);
}


int equal_lines (struct line *A, struct line *B)
{
  return ((A->A == B->A && A->B == B->B)
      ||  (A->A == B->B && A->B == B->A));
}






/*
 *  plotting functions
 */


void colourPixel (SDL_Surface *dst, vtx2i_t *p, unsigned color)
{
  setPixel (dst, p, color);
}

void invertPixel (SDL_Surface *dst, vtx2i_t *p, unsigned color)
{
  vtx2i_t p2 = *p;
  SDL_Rect clip_rect;
  SDL_GetClipRect(canvas, &clip_rect);
  if (clip_rect.x != 0)
  {
    p2.pts[0] += clip_rect.x;
  }
  if (clip_rect.y != 0)
  {
    p2.pts[1] += clip_rect.y;
  }

  unsigned clr = getPixel(canvas, &p2);
  clr ^= 0xffffffff;
  clr |= dst->format->Amask;

  setPixel(dst, p, clr);
}


int negate_byte_at (unsigned byte, unsigned mask)
{
  int pt = ((byte ^ 0xffffff) & mask) - (byte & mask);
  pt *= *colour_negate_rate;
  pt += (byte & mask);
  while (mask >0xff)
  {
    mask /= 0x100;
    pt /= 0x100;
  }
  return pt;
}

void divertPixel (SDL_Surface *dst, vtx2i_t *p, unsigned color)
{
  vtx2i_t p2 = *p;
  SDL_Rect clip_rect;
  SDL_GetClipRect(canvas, &clip_rect);
  if (clip_rect.x != 0)
  {
    p2.pts[0] += clip_rect.x;
  }
  if (clip_rect.y != 0)
  {
    p2.pts[1] += clip_rect.y;
  }

  unsigned clr = getPixel(canvas, &p2);
  unsigned R, G, B;
  R = negate_byte_at(clr, canvas->format->Rmask);
  G = negate_byte_at(clr, canvas->format->Gmask);
  B = negate_byte_at(clr, canvas->format->Bmask);

  clr = SDL_MapRGBA(dst->format, R,G,B,0xff);
  setPixel(dst, p, clr);
}
