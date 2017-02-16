#include "drawn-triangles.h"


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

void draw_screen_triangle (triangle_t *t, SDL_Surface *surface, plot_func plot, unsigned color)
{

  int rotation = t->rotation;
  while (rotation < 0)
    rotation += 4;
  rotation %= 4;

  double sides = 3 * t->radius;
  sides /= sqrt(3);
  sides /= 2;

    int x1, y1,
    x2, y2,
    x3, y3;

    switch(rotation)
    {
    case 0:
      x1 = - (0.5 + sides); y1 = 0.5 + (t->radius / 2);
      x2 = 0.5 + sides;   y2 = 0.5 + (t->radius / 2);
      x3 = 0;   y3 = - (0.5 + t->radius); //  top
      break;

    case 1:
      x1 = - (0.5 + (t->radius / 2)); y1 = - (0.5 + sides);
      x2 = - (0.5 + (t->radius / 2)); y2 = (0.5 + sides);
      x3 = 0.5 + t->radius; y3 = 0; //  right
      break;

    case 2:
      x1 = - (0.5 + sides); y1 = - (0.5 + (t->radius / 2));
      x2 = 0.5 + sides;   y2 = - (0.5 + (t->radius / 2));
      x3 = 0;   y3 = (0.5 + t->radius); //  bottom
      break;

    case 3:
      x1 = 0.5 + (t->radius / 2); y1 = - (0.5 + sides);
      x2 = 0.5 + (t->radius / 2); y2 = (0.5 + sides);
      x3 = - (0.5 + t->radius); y3 = 0; //  left
      break;
    }

  draw_line (surface, t->x + x1, t->y + y1, t->x + x2, t->y + y2, plot, color);
  draw_line (surface, t->x + x2, t->y + y2, t->x + x3, t->y + y3, plot, color);
  draw_line (surface, t->x + x3, t->y + y3, t->x + x1, t->y + y1, plot, color);

}
