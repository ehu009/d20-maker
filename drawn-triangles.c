#include "drawn-triangles.h"

#include "pixels.h"
struct screen_triangle
{
  int x, y;
  int rotation;
  double  radius;
};


triangle_t *make_screen_triangle (double radius)
{
  triangle_t *t = calloc(sizeof(triangle_t), 1);
  if (t !=NULL)
  {
    t->radius = radius;
  }
  return t;
}

void free_screen_triangle (triangle_t *t)
{
  free(t);
}

int get_screen_triangle_rotation(triangle_t *t)
{
  return t->rotation % 4;
}

void get_screen_triangle_position (triangle_t *t, int *x, int *y)
{
  *x = t->x;
  *y = t->y;
}

void set_screen_triangle_position (triangle_t *t, int x, int y)
{
  t->x = x;
  t->y = y;
}

void rotate_screen_triangle (triangle_t *t, int add)
{
  t->rotation += add;
  while (t->rotation < 0)
    t->rotation += 4;
  t->rotation %= 4;
}

void resize_screen_triangle (triangle_t *t, double add)
{
  t->radius += add;
  if (t->radius < 0)
  {
    t->radius *= -1;
    t->rotation += 2;
    t->rotation %= 4;
  }

}


void get_triangle_points_relative (triangle_t *t,
    double *x1, double *y1,
    double *x2, double *y2,
    double *x3, double *y3)
{

  int rotation = t->rotation;
  while (rotation < 0)
    rotation += 4;
  rotation %= 4;

  double sides = 3 * t->radius;
  sides /= sqrt(3);
  sides /= 2;

  switch(rotation)
  {
  case 0:
    *x1 = - (sides); *y1 = (t->radius / 2);
    *x2 = sides;   *y2 = (t->radius / 2);
    *x3 = 0;   *y3 = - (t->radius); //  top
    break;

  case 1:
    *x1 = - (t->radius / 2); *y1 = - (sides);
    *x2 = - (t->radius / 2); *y2 = sides;
    *x3 = t->radius; *y3 = 0; //  right
    break;

  case 2:
    *x1 = - ( sides); *y1 = - (t->radius / 2);
    *x2 = sides;   *y2 = - (t->radius / 2);
    *x3 = 0;   *y3 = (t->radius); //  bottom
    break;

  case 3:
    *x1 = (t->radius / 2); *y1 = - (sides);
    *x2 = (t->radius / 2); *y2 = (sides);
    *x3 = - (t->radius); *y3 = 0; //  left
    break;
  }
}

void draw_screen_triangle (triangle_t *t, SDL_Surface *surface, plot_func plot, unsigned color)
{
  double x1, y1,
  x2, y2,
  x3, y3;

  get_triangle_points_relative(t,&x1,&y1,&x2,&y2,&x3,&y3);
  x1 += t->x;   x2 += t->x;   x3 += t->x;
  y1 += t->y;   y2 += t->y;   y3 += t->y;

  draw_line (surface, x1, y1, x2, y2, plot, color);
  draw_line (surface, x2, y2, x3, y3, plot, color);
  draw_line (surface, x3, y3, x1, y1, plot, color);
}

void fill_invert_screen_triangle (triangle_t *t, SDL_Surface *surface)
{
  int i = 0;

  double x[3], y[3];
  get_triangle_points_relative (t,&x[0],&y[0],&x[1],&y[1],&x[2],&y[2]);

  int b_x = x[0], b_y = y[0];
  int b_w = x[0], b_h = y[0];
  do
  {
    int cx = x[i], cy = y[i];
    if (b_x > cx)
      b_x = cx;
    if (b_w < cx)
      b_w = cx;
    if (b_y > cy)
      b_y = cy;
    if (b_h < cy)
      b_h = cy;
    i ++;
  }
  while (i < 3);

  b_w -= b_x - 1;
  b_h -= b_y - 1;

  SDL_Rect rect =
  {
    .x = b_x + t->x,
    .y = b_y + t->y,
    .w = b_w,
    .h = b_h
  };


  SDL_Surface *surf = NULL;
  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    surf = SDL_CreateRGBSurface (0, b_w, b_h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
  #else
    surf = SDL_CreateRGBSurface (0, b_w, b_h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
  #endif

  SDL_SetSurfaceBlendMode(surf,SDL_BLENDMODE_BLEND);
  SDL_BlitSurface (surface, &rect, surf, NULL);

  uint32_t markup = SDL_MapRGBA(surf->format, 0xff,0xff,0,0xff);


  SDL_Rect clip_rect;
  SDL_GetClipRect(surface, &clip_rect);
  SDL_SetClipRect(surface, &rect);

  for (i = 0; i < 3; i++)
  {
    x[i] -= b_x;
    y[i] -= b_y;
  }

  draw_line (surf, x[0], y[0], x[1], y[1], colourPixel, markup);
  draw_line (surf, x[1], y[1], x[2], y[2], colourPixel, markup);
  draw_line (surf, x[2], y[2], x[0], y[0], colourPixel, markup);


  int j;
  uint32_t pixel;

  for (i = b_x; i <= surf->w; i++)
  {
    int upper = -1, lower = -1;

    for (j = b_y; j <= surf->h; j++)
    {
      pixel = getPixel(surf, i, j);
      if (pixel != markup)
        continue;
      upper = j;
      break;

    }
    if (upper != -1)
    {
      lower = upper;
      for (j = surf->h; j >=upper; j --)
      {
        pixel = getPixel(surf, i, j);
        if (pixel != markup)
          continue;
        lower = j;
        break;
      }
      draw_line (surf, i, upper, i, lower, invertPixel, 0);
    }
  }
  SDL_SetClipRect(surface, &clip_rect);
  SDL_BlitSurface (surf, NULL, surface, &rect);
  SDL_FreeSurface (surf);

}
