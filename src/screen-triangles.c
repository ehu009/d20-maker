#include    "screen-triangles.h"

#ifdef _WIN32
  #include    <malloc.h>
#endif

SDL_Rect *get_bounds_of_triangle (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c)
{
  SDL_Rect *ptr = (SDL_Rect *) malloc (sizeof (SDL_Rect));
  if (ptr != NULL)
  {
    vtx2d_t lowest = *a, highest = *a;

    if (lowest.pts[0] >= b->pts[0])
      lowest.pts[0] = b->pts[0];
    if (lowest.pts[1] >= b->pts[1])
      lowest.pts[1] = b->pts[1];
    if (lowest.pts[0] >= c->pts[0])
      lowest.pts[0] = c->pts[0];
    if (lowest.pts[1] >= c->pts[1])
      lowest.pts[1] = c->pts[1];

    if (highest.pts[0] <= b->pts[0])
      highest.pts[0] = b->pts[0];
    if (highest.pts[1] <= b->pts[1])
      highest.pts[1] = b->pts[1];
    if (highest.pts[0] <= c->pts[0])
      highest.pts[0] = c->pts[0];
    if (highest.pts[1] <= c->pts[1])
      highest.pts[1] = c->pts[1];
    highest.pts[0] -= lowest.pts[0];
    highest.pts[1] -= lowest.pts[1];

    ptr->x = lowest.pts[0];
    ptr->y = lowest.pts[1];
    ptr->w = 2.5 + highest.pts[0];
    ptr->h = 2.5 + highest.pts[1];

  }
  return ptr;
}

void draw_triangle (triangle_t *t, plot_func plot, COLOR color)
{
  vtx2i_t A, B, C;
  get_vtx2i_from_vtx2d (t->pts[0], &A);
  get_vtx2i_from_vtx2d (t->pts[1], &B);
  get_vtx2i_from_vtx2d (t->pts[2], &C);

  draw_line2 (canvas, &A, &B, plot, color);
  draw_line2 (canvas, &B, &C, plot, color);
  draw_line2 (canvas, &C, &A, plot, color);
}


void fill_triangle (triangle_t *t, plot_func plot, COLOR color)
{
  vtx2d_t *a = t->pts[0],
      *b = t->pts[1],
      *c = t->pts[2];
  SDL_Rect *rect = get_bounds_of_triangle (a, b, c);


  SDL_Surface *surf = SDL_CreateRGBSurface (0, rect->w, rect->h, 32, canvas->format->Rmask, canvas->format->Gmask, canvas->format->Bmask, 0xff000000);

  SDL_Rect clip_rect;
  /*
  if ((plot == invertPixel) || (plot == divertPixel))
  {*/
  SDL_GetClipRect(canvas, &clip_rect);
  SDL_SetClipRect(canvas, rect);
  //}

  vtx2i_t A, B, C;
  get_vtx2i_from_vtx2d (a, &A);
  get_vtx2i_from_vtx2d (b, &B);
  get_vtx2i_from_vtx2d (c, &C);
  A.pts[0] -= rect->x;
  B.pts[0] -= rect->x;
  C.pts[0] -= rect->x;
  A.pts[1] -= rect->y;
  B.pts[1] -= rect->y;
  C.pts[1] -= rect->y;

  COLOR pixel, fill = color,
      markup = SDL_MapRGBA(surf->format, 0xff,0xff,0,0),
      bg = SDL_MapRGBA(surf->format, 0,0,0,0);
  if (plot != invertPixel)
  {
    unsigned R = (color & 0x00ff0000)>>16,
        G = (color & 0x0000ff00)>>8,
        B = (color & 0x000000ff),
        A = (color & 0xff000000)>>24;
    fill = SDL_MapRGBA(surf->format, R, G, B, A);
    if (markup == fill)
      markup = SDL_MapRGBA(surf->format, 0xff,0xee,0,0);
  }

  SDL_FillRect(surf, NULL, bg);

  draw_line2 (surf, &A, &B, colourPixel, markup);
  draw_line2 (surf, &B, &C, colourPixel, markup);
  draw_line2 (surf, &C, &A, colourPixel, markup);

  vtx2i_t p;
  int i = 0, j = 0;

  for (i=0; i <= rect->w; i++)
  {
    p.pts[0] = i;
    int upper = -1, lower = -1;

    for (j = 0; j <= rect->h; j++)
    {
      p.pts[1] = j;
      pixel = getPixel (surf, &p);
      if (pixel != markup)
        continue;
      upper = j;
      break;

    }
    if (upper != -1)
    {
      lower = upper;
      for (j = rect->h; j >=upper; j --)
      {
        p.pts[1] = j;
        pixel = getPixel (surf, &p);
        if (pixel != markup)
          continue;
        lower = j;
        break;
      }
      p.pts[1] = upper;
      vtx2i_t p2 = {.pts = {i, lower}};
      draw_line2 (surf, &p, &p2, plot, fill);
    }
  }

//  if ((plot == invertPixel) || (plot == divertPixel))
    SDL_SetClipRect(canvas, &clip_rect);

  SDL_BlitSurface (surf, NULL, canvas, rect);
  SDL_FreeSurface (surf);
  free ((void *) rect);
}

int triangle_contains (triangle_t *t, vtx2i_t point)
{
  vtx2d_t *a = t->pts[0],
      *b = t->pts[1],
      *c = t->pts[2];
  SDL_Rect *rect = get_bounds_of_triangle (a, b, c);
  if ((point.pts[0] < rect->x)
      ||  (point.pts[0] > rect->x + rect->w)
      ||  (point.pts[1] < rect->y)
      ||  (point.pts[1] > rect->y + rect->h))
  {
    free (rect);
    return 0;
  }

  SDL_Surface *surf = NULL;

  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    surf = SDL_CreateRGBSurface (0, rect->w, rect->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
  #else
    surf = SDL_CreateRGBSurface (0, rect->w, rect->h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
  #endif

  vtx2i_t A, B, C;
  get_vtx2i_from_vtx2d (a, &A);
  get_vtx2i_from_vtx2d (b, &B);
  get_vtx2i_from_vtx2d (c, &C);
  A.pts[0] -= rect->x;
  B.pts[0] -= rect->x;
  C.pts[0] -= rect->x;
  A.pts[1] -= rect->y;
  B.pts[1] -= rect->y;
  C.pts[1] -= rect->y;

  point.pts[0] -= rect->x;
  point.pts[1] -= rect->y;

  COLOR pixel, markup = SDL_MapRGBA(surf->format, 0xff, 0x00, 0xff, 0x00);
  draw_line2 (surf, &A, &B, colourPixel, markup);
  draw_line2 (surf, &B, &C, colourPixel, markup);
  draw_line2 (surf, &C, &A, colourPixel, markup);

  vtx2i_t p;
  int j = 0, upper = -1, lower = -1;

  p.pts[0] = point.pts[0];
  for (j = 0; j <= rect->h; j++)
  {
    p.pts[1] = j;
    pixel = getPixel (surf, &p);
    if (pixel == markup)
    {
      if (lower == -1)
      {
        lower = j;
        break;
      }
    }
  }
  for (j = rect->h; j > 0; j--)
  {
    p.pts[1] = j;
    pixel = getPixel (surf, &p);
    if (pixel == markup)
    {
      if (upper == -1)
      {
        upper = j;
        break;
      }
    }
  }
  SDL_FreeSurface (surf);
  free ((void *) rect);

  return ((point.pts[1] > lower) && (point.pts[1] < upper));
}

void transfer_triangle (triangle_t *t, SDL_Surface *dst, SDL_Rect *dst_rect)
{
  vtx2d_t *a = t->pts[0],
      *b = t->pts[1],
      *c = t->pts[2];
  SDL_Rect *rect = get_bounds_of_triangle (a, b, c);

  SDL_Surface *surf = SDL_CreateRGBSurface (0, rect->w, rect->h, 32, canvas->format->Rmask, canvas->format->Gmask, canvas->format->Bmask, 0xff000000);


  vtx2i_t A, B, C;
  get_vtx2i_from_vtx2d (a, &A);
  get_vtx2i_from_vtx2d (b, &B);
  get_vtx2i_from_vtx2d (c, &C);
  A.pts[0] -= rect->x;
  B.pts[0] -= rect->x;
  C.pts[0] -= rect->x;
  A.pts[1] -= rect->y;
  B.pts[1] -= rect->y;
  C.pts[1] -= rect->y;

  COLOR pixel,
      markup = SDL_MapRGBA(surf->format, 0xff,0xff,0,0),
      bg = SDL_MapRGBA(surf->format, 0,0,0,0);
  SDL_FillRect(surf, NULL,bg);

  draw_line2 (surf, &A, &B, colourPixel, markup);
  draw_line2 (surf, &B, &C, colourPixel, markup);
  draw_line2 (surf, &C, &A, colourPixel, markup);

  vtx2i_t p;
  int i = 0, j = 0;

  for (i=0; i <= rect->w; i++)
  {
    p.pts[0] = i;
    int upper = -1, lower = -1;

    for (j = 0; j <= rect->h; j++)
    {
      p.pts[1] = j;
      pixel = getPixel (surf, &p);
      if (pixel != markup)
        continue;
      upper = j;
      break;

    }
    if (upper != -1)
    {
      lower = upper;
      for (j = rect->h; j >=upper; j --)
      {
        p.pts[1] = j;
        pixel = getPixel (surf, &p);
        if (pixel != markup)
          continue;
        lower = j;
        break;
      }

      for (j = upper; j <= lower; j++)
      {
        p.pts[1] = j;
        vtx2i_t p2 = {.pts = {i + rect->x, j + rect->y}};
        uint32_t clr = getPixel(canvas, &p2);
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
          clr |= 0xff;
        #else
          clr |= 0xff000000;
        #endif
        setPixel(surf, &p, clr);
      }
    }
  }
  rect->x -= dst_rect->x;
  rect->y -= dst_rect->y;

  SDL_BlitSurface (surf, NULL, dst, rect);

  SDL_FreeSurface (surf);
  free ((void *) rect);

}

