#include "pixels.h"

//  Knowing where pixels aren't
#define   OUT_OF_BOUNDS(x, y, surface)  \
    (x >= surface->w)         \
        ||  (x < 0)           \
        ||  (y >= surface->h) \
        ||  (y < 0)
//  Knowing how pixels are represented in memory
#define   PIXEL_AT(x, y, surface)       \
    (COLOUR *) surface->pixels + y*surface->pitch/4 + x


COLOUR getPixel (SDL_Surface *src,
    vtx2i_t *p)
{ //  Read pixel value from surface
  COLOUR *bufp;
  int x = p->pts[0], y = p->pts[1];
  if (OUT_OF_BOUNDS(x, y, src))
    return 0;
  bufp = PIXEL_AT(x, y, src);
  return *bufp;
}

void setPixel (SDL_Surface *dst,
    vtx2i_t *p,
    COLOUR color)
{ //  Change pixel value for surface
  COLOUR *bufp;
  int x = p->pts[0], y = p->pts[1];
  if (OUT_OF_BOUNDS(x, y, dst))
    return;
  bufp = (COLOUR *) dst->pixels + y*dst->pitch/4 + x;
  *bufp = color;
}
