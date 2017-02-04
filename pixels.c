#include "pixels.h"

uint32_t getPixel (SDL_Surface *src,
    int x, int y)
{
  unsigned *bufp;
  if (x >= src->w
      ||  x < 0
      ||  y >=src->h
      ||  y < 0)
    return 0;
  bufp = (unsigned*) src->pixels + y*src->pitch/4 + x;
  return *bufp;
}

void setPixel (SDL_Surface *dst,
    int x, int y,
    unsigned color)
{
  unsigned *bufp;

  if (x >= dst->w
      ||  x < 0
      ||  y >= dst->h
      ||  y < 0)
    return;
  bufp = (unsigned *) dst->pixels + y*dst->pitch/4 + x;
  *bufp = color;
}
