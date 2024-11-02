#include    <string.h>

#include    "screen-triangles.h"
#include    "memory.h"


triangle_t *copy_triangle (triangle_t *t)
{
  triangle_t *tr = MALLOC(sizeof(triangle_t));
  if (tr != NULL)
  {
    memcpy(tr, t, sizeof(triangle_t));
  }
  return tr;
}

SDL_Rect *get_bounds_of_triangle (triangle_t *t)
{
  vtx2d_t *a = t->pts[0],
      *b = t->pts[1],
      *c = t->pts[2];
  SDL_Rect *ptr = (SDL_Rect *) MALLOC(sizeof (SDL_Rect));
  if (ptr != NULL)
  {
    vtx2d_t lowest = *a, highest = *a;

    if (lowest.pts[0] >= b->pts[0])
    {
      lowest.pts[0] = b->pts[0];
    }
    if (lowest.pts[1] >= b->pts[1])
    {
      lowest.pts[1] = b->pts[1];
    }
    if (lowest.pts[0] >= c->pts[0])
    {
      lowest.pts[0] = c->pts[0];
    }
    if (lowest.pts[1] >= c->pts[1])
    {
      lowest.pts[1] = c->pts[1];
    }
    if (highest.pts[0] <= b->pts[0])
    {
      highest.pts[0] = b->pts[0];
    }
    if (highest.pts[1] <= b->pts[1])
    {
      highest.pts[1] = b->pts[1];
    }
    if (highest.pts[0] <= c->pts[0])
    {
      highest.pts[0] = c->pts[0];
    }
    if (highest.pts[1] <= c->pts[1])
    {
      highest.pts[1] = c->pts[1];
    }
    
    {
      double l;
      l = lowest.pts[0];
      ptr->x = l;
      highest.pts[0] -= l;
      l = lowest.pts[1];
      ptr->y = l;
      highest.pts[1] -= l;
    }

    ptr->w = 2.5 + highest.pts[0];
    ptr->h = 2.5 + highest.pts[1];

  }
  return ptr;
}

void fit_vertex_in_rect (vtx2d_t *src, vtx2i_t *p, SDL_Rect *r)
{
  get_vtx2i_from_vtx2d(src, p);
  p->pts[0] -= r->x;
  p->pts[1] -= r->y;
}

void draw_triangle (triangle_t *t, plot_func plot, COLOUR color)
{
  vtx2i_t A, B, C;
  get_vtx2i_from_vtx2d(t->pts[0], &A);
  get_vtx2i_from_vtx2d(t->pts[1], &B);
  get_vtx2i_from_vtx2d(t->pts[2], &C);

  draw_line(canvas, &A, &B, plot, color);
  draw_line(canvas, &B, &C, plot, color);
  draw_line(canvas, &C, &A, plot, color);
}


#define MARKUP(s)  SDL_MapRGBA(s->format, 0xff,0xff,0,0)

SDL_Surface *get_markup_surface(SDL_Rect *r, vtx2i_t *A, vtx2i_t *B, vtx2i_t *C)
{
  SDL_Surface *surf = SDL_CreateRGBSurface (0, r->w, r->h, 32,
      canvas->format->Rmask, canvas->format->Gmask, canvas->format->Bmask, 0xff000000);

  COLOUR markup = MARKUP(surf),
      bg = SDL_MapRGBA(surf->format, 0,0,0,0);

  SDL_FillRect(surf, NULL, bg);
  draw_line(surf, A, B, colourPixel, markup);
  draw_line(surf, B, C, colourPixel, markup);
  draw_line(surf, C, A, colourPixel, markup);

  return surf;
}

int find_upper_markup (SDL_Surface *s, vtx2i_t*buf, COLOUR m, SDL_Rect *r)
{
  int limit = -1, i = 0;
  COLOUR c;
  for (; i <= r->h; i++)
  {
    buf->pts[1] = i;
    c = getPixel(s, buf);
    if (c != m)
    {
      continue;
    }
    limit = i;
    break;
  }
  return limit;
}

void fill_triangle (triangle_t *t, plot_func plot, COLOUR color)
{
  SDL_Rect *rect = get_bounds_of_triangle(t);

  vtx2i_t A, B, C;
  fit_vertex_in_rect(t->pts[0], &A, rect);
  fit_vertex_in_rect(t->pts[1], &B, rect);
  fit_vertex_in_rect(t->pts[2], &C, rect);

  SDL_Surface *surf = get_markup_surface(rect, &A, &B, &C);

  COLOUR pixel, fill = color,
      markup = MARKUP(surf);

  SDL_Rect clip_rect;
  SDL_GetClipRect(canvas, &clip_rect);
  SDL_SetClipRect(canvas, rect);
  vtx2i_t p;
  int i = 0, j = 0;
  for (; i <= rect->w; i++)
  {
    p.pts[0] = i;
    int upper = -1, lower = -1;
    upper = find_upper_markup(surf, &p, markup, rect);

    if (upper != -1)
    {
      lower = upper;
      for (j = rect->h; j >=upper; j --)
      {
        p.pts[1] = j;
        pixel = getPixel(surf, &p);
        if (pixel != markup)
        {
          continue;
        }
        lower = j;
        break;
      }
      p.pts[1] = upper;
      vtx2i_t p2 = {.pts = {i, lower}};
      draw_line(surf, &p, &p2, plot, fill);
    }
  }

  SDL_SetClipRect(canvas, &clip_rect);

  SDL_BlitSurface(surf, NULL, canvas, rect);
  SDL_FreeSurface(surf);
  FREE(rect);
}

void draw_triangle_transparent (triangle_t *t)
{
  fill_triangle(t, invertPixel, 0);
}

void draw_triangle_coloured (triangle_t *t, COLOUR colour)
{
  fill_triangle(t, colourPixel, colour);
}

int triangle_contains (triangle_t *t, vtx2i_t point)
{
  SDL_Rect *rect = get_bounds_of_triangle(t);
  if ((point.pts[0] < rect->x)
      ||  (point.pts[0] > rect->x + rect->w)
      ||  (point.pts[1] < rect->y)
      ||  (point.pts[1] > rect->y + rect->h))
  {
    free(rect);
    return 0;
  }

  vtx2i_t A, B, C;

  fit_vertex_in_rect(t->pts[0], &A, rect);
  fit_vertex_in_rect(t->pts[1], &B, rect);
  fit_vertex_in_rect(t->pts[2], &C, rect);

  point.pts[0] -= rect->x;
  point.pts[1] -= rect->y;
  SDL_Surface *surf = get_markup_surface(rect, &A, &B, &C);

  COLOUR pixel,
      markup = MARKUP(surf);

  vtx2i_t p;
  int j = 0, upper = -1, lower = -1;

  p.pts[0] = point.pts[0];
  upper = find_upper_markup(surf, &p, markup, rect);
  for (j = rect->h; j > 0; j--)
  {
    p.pts[1] = j;
    pixel = getPixel(surf, &p);
    
    if ((pixel == markup)
        && (lower == -1))
    {
      lower = j;
      break;
    }
  }
  SDL_FreeSurface(surf);
  free(rect);

  return ((point.pts[1] > upper) && (point.pts[1] < lower));
}

void transfer_triangle (triangle_t *t, SDL_Surface *dst, SDL_Rect *dst_rect)
{
  SDL_Rect *rect = get_bounds_of_triangle(t);

  vtx2i_t A, B, C;
  fit_vertex_in_rect(t->pts[0], &A, rect);
  fit_vertex_in_rect(t->pts[1], &B, rect);
  fit_vertex_in_rect(t->pts[2], &C, rect);

  SDL_Surface *surf = get_markup_surface(rect, &A, &B, &C);

  COLOUR pixel,
      markup = MARKUP(surf);

  vtx2i_t p;
  int i = 0, j = 0;
  for (; i <= rect->w; i++)
  {
    p.pts[0] = i;

    int upper = -1, lower = -1;
    upper = find_upper_markup(surf, &p, markup, rect);
    if (upper != -1)
    {
      lower = upper;
      for (j = rect->h; j >=upper; j --)
      {
        p.pts[1] = j;
        pixel = getPixel(surf, &p);
        if (pixel != markup)
        {
          continue;
        }
        lower = j;
        break;
      }

      for (j = upper; j <= lower; j++)
      {
        p.pts[1] = j;
        vtx2i_t p2 = {.pts = {i + rect->x, j + rect->y}};
        uint32_t clr = getPixel(canvas, &p2);
        clr |= alpha_mask;

        setPixel(surf, &p, clr);
      }
    }
  }
  rect->x -= dst_rect->x;
  rect->y -= dst_rect->y;
  SDL_BlitSurface(surf, NULL, dst, rect);

  SDL_FreeSurface(surf);
  free (rect);

}

