#include "pixels.h"


#define   OUT_OF_BOUNDS(x, y, surface)  \
    (x >= surface->w)         \
        ||  (x < 0)           \
        ||  (y >= surface->h) \
        ||  (y < 0)

#define   PIXEL_AT(x, y, surface)       (COLOR *) surface->pixels + y*surface->pitch/4 + x


COLOR getPixel (SDL_Surface *src,
    int x, int y)
{
  COLOR *bufp;
  if (OUT_OF_BOUNDS(x, y, src))
    return 0;
  bufp = PIXEL_AT(x, y, src);
  return *bufp;
}

void setPixel (SDL_Surface *dst,
    int x, int y,
    COLOR color)
{
  COLOR *bufp;
  if (OUT_OF_BOUNDS(x, y, dst))
    return;
  bufp = (COLOR *) dst->pixels + y*dst->pitch/4 + x;
  *bufp = color;
}
