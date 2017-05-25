#include "pixels.h"

//  knowing where pixels aren't
#define   OUT_OF_BOUNDS(x, y, surface)  \
    (x >= surface->w)         \
        ||  (x < 0)           \
        ||  (y >= surface->h) \
        ||  (y < 0)
//  knowing how pixels are represented in memory
#define   PIXEL_AT(x, y, surface)       \
    (COLOR *) surface->pixels + y*surface->pitch/4 + x


COLOR getPixel (SDL_Surface *src,
    vtx2_t *p)
{ //  read pixel value from surface
  COLOR *bufp;
  int x = p->pts[0], y = p->pts[1];
  if (OUT_OF_BOUNDS(x, y, src))
    return 0;
  bufp = PIXEL_AT(x, y, src);
  return *bufp;
}

void setPixel (SDL_Surface *dst,
    vtx2_t *p,
    COLOR color)
{ //  change pixel value for surface
  COLOR *bufp;
  int x = p->pts[0], y = p->pts[1];
  if (OUT_OF_BOUNDS(x, y, dst))
    return;
  bufp = (COLOR *) dst->pixels + y*dst->pitch/4 + x;
  *bufp = color;
}
