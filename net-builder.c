#include "net-builder.h"

#include    <math.h>

//  distance between two points
double distance (double x1, double y1, double x2, double y2)
{ //  pythagorean root of squared sides summed
  double dx = x2 - x1, dy = y2 - y1;
  dx *= dx;
  dy *= dy;
  return sqrt (dx + dy);
}


int approximates (double test, double source, double accuracy)
{
  return (source + accuracy >= test)
      && (test >= source - accuracy);
}
#define FLOAT_ACC 2


typedef enum {APP_START, APP_MAIN, APP_END, APP_FREE} app_status;

static struct
{
  //  togglers
  char rotate_root;
  char use_slider;
  //  other things
  //  ...
  int mX, mY;
  app_status status;
} application;


int change_root_size (void)
{ //  toggle between rotation and resizing of root triangle
  if((mouse_middle() == -1)
      ||  (mouse_right() == -1))
  {
    application.rotate_root ^= 1;
  }
  return application.rotate_root;
}

int use_scroll_selector(void)
{ //  toggle between using mouse wheel or position for selections
  if(mouse_middle() == -1)
  {
    application.use_slider ^= 1;
  }
  return application.use_slider;
}


/*
 *  model of vertices in an icosahedron
 */
#define NUM_D20_VTX   12

vtx5i_t d20_model[NUM_D20_VTX] =
{
 {.pts = {1,2,3,4,5}},
 {.pts = {0,2,5,6,7}},
 {.pts = {0,1,7,8,3}},
 {.pts = {0,2,4,8,9}},
 {.pts = {0,3,5,9,10}},
 {.pts = {0,1,4,6,10}},
 {.pts = {1,5,7,10,11}},
 {.pts = {1,2,6,8,11}},
 {.pts = {2,3,7,9,11}},
 {.pts = {3,4,8,10,11}},
 {.pts = {4,5,6,10,11}},
 {.pts = {7,8,3,10,11}}
};

#define NUM_VTX_POS   5



struct vertex_slot
{
  char pinned;
  vtx2d_t *pos[NUM_VTX_POS];
  vtx2d_t *invalid;
  struct vertex_slot *links[NUM_VTX_POS];
};

typedef struct vertex_slot slot_t;


struct triangle_face
{
  slot_t *A, *B, *C;
  int pos_A, pos_B, pos_C;
};


typedef struct triangle_face triangle_t;

static struct
{
  int x, y;
  double radius;
  int rotation;
  int num_faces;

  triangle_t *current;
  slot_t vertex[NUM_D20_VTX];
} d20;



int rotate_root (int diff)
{ // rotate
  if (diff)
  {
    d20.rotation += diff;
    while (d20.rotation < 0)
      d20.rotation += 4;
    d20.rotation %= 4;
    return 1;;
  }
  return 0;
}

int resize_root (int diff)
{ // increase radius
  if (diff)
  {
    d20.radius += diff*1.75;
    return 1;
  }
  return 0;
}

void app_start (void)
{
  application.rotate_root = 1;
  application.use_slider = 1;
  application.status = APP_START;

  // initialize structures
  int j = 0, k = 0;
  slot_t *s;
  for (; j < NUM_D20_VTX; j ++)
  {
    s = &d20.vertex[j];
    s->pinned = -1;
    for (; k < NUM_VTX_POS; k++)
    {
      s->pos[k] = NULL;
      s->links[k] = &d20.vertex[d20_model[j].pts[k]];
    }
    s->invalid = NULL;
  }

  d20.num_faces = 0;
  d20.radius = 30.0;
  d20.x = 50;
  d20.y = 50;

  triangle_t *t = malloc(sizeof(triangle_t));
  t->A = &d20.vertex[0];
  t->B = &d20.vertex[1];
  t->C = &d20.vertex[2];
  t->pos_A = 0;
  t->pos_B = 0;
  t->pos_C = 0;
  t->A->pos[0] = calloc (1, sizeof(vtx2d_t));
  t->B->pos[0] = calloc (1, sizeof(vtx2d_t));
  t->C->pos[0] = calloc (1, sizeof(vtx2d_t));
  d20.current = t;
}

void app_free (void)
{
  //  free initialized structures
  int j = 0, k = 0;
  slot_t *s;
  free(d20.current);
  for (; j < NUM_D20_VTX; j ++)
  {
    s = &d20.vertex[j];
    for (; k < NUM_VTX_POS; k++)
    {
      if (s->pos[k] != NULL)
        free (s->pos[k]);
    }
    if (s->invalid != NULL)
      free (s->invalid);
  }
}


SDL_Rect *get_bounds_of_triangle (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c)
{
  SDL_Rect *ptr = malloc (sizeof (SDL_Rect));
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

    ptr->x = lowest.pts[0];
    ptr->y = lowest.pts[1];
    ptr->w = highest.pts[0] + 2.5 - lowest.pts[0];
    ptr->h = highest.pts[1] + 2.5 - lowest.pts[1];

  }
  return ptr;
}

void fill_triangle (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c, plot_func plot, COLOR color)
{
  SDL_Rect *rect = get_bounds_of_triangle (a, b, c);

  SDL_Surface *surf = NULL;

  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    surf = SDL_CreateRGBSurface (0, rect->w, rect->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
  #else
    surf = SDL_CreateRGBSurface (0, rect->w, rect->h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
  #endif

  SDL_SetSurfaceBlendMode(surf,SDL_BLENDMODE_BLEND);

  SDL_Rect clip_rect;
  SDL_GetClipRect(canvas, &clip_rect);
  SDL_SetClipRect(canvas, rect);

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

  COLOR pixel, fill = color, markup = SDL_MapRGBA(surf->format, 0xff,0xff,0,0);
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

  SDL_SetClipRect(canvas, &clip_rect);
  SDL_BlitSurface (surf, NULL, canvas, rect);

  SDL_FreeSurface (surf);
  free(rect);
}

int triangle_contains (triangle_t *t, vtx2i_t point)
{

  vtx2d_t *a = t->A->pos[t->pos_A],
      *b = t->B->pos[t->pos_B],
      *c = t->C->pos[t->pos_C];

  SDL_Rect *rect = get_bounds_of_triangle (a, b, c);
  if ((point.pts[0] < rect->x)
      ||  (point.pts[0] > rect->x + rect->w)
      ||  (point.pts[1] < rect->y)
      ||  (point.pts[1] > rect->y + rect->h))
    return 0;

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

  COLOR pixel, markup = SDL_MapRGBA(surf->format, 0xff, 0xff, 0xff, 0xff);

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
      if (upper == -1)
        upper = j;
      else
      {
        lower = j;
        break;
      }
    }
  }
  SDL_FreeSurface (surf);
  free(rect);
  if (lower == -1)
  {
    return point.pts[1] == upper;
  }
  else
  {
    return ((point.pts[1] >= upper) && (point.pts[1] <= lower));
  }
}





void draw_unpinned (triangle_t *t)
{
  fill_triangle (t->A->pos[t->pos_A], t->B->pos[t->pos_B], t->C->pos[t->pos_C], invertPixel, 0);
}

void draw_selected (triangle_t *t, COLOR colour)
{
  fill_triangle (t->A->pos[t->pos_A], t->B->pos[t->pos_B], t->C->pos[t->pos_C], colourPixel, colour);
}






void app_draw (void)
{
  SDL_FillRect (canvas, NULL, 0x0ff);
  SDL_BlitSurface (src_image, NULL, canvas, NULL);

  if (!d20.num_faces)
  {
    draw_unpinned (d20.current);
  }
}

void relocate_and_undo (void)
{
  if (mouse_middle() == -1)
  {
    //  relocate
  }
  if (mouse_right() == -1)
  {
    //  undoing
  }
}

void reposition_root_vertices (void)
{
  double radi = d20.radius;
  double half = 0.5;
  double root3 = sqrt(3.0);

  double aX = radi, aY = radi,
      bX = radi, bY = radi,
      cX = radi, cY = radi;

  switch (d20.rotation)
  {
    case 0:
      aX *= root3*half;   aY *= -half;
      bX *= -root3*half;  bY *= -half;
      cX *= 0;   cY *= 1;
      break;
    case 1:
      aX *= 1;   aY *= 0;
      bX *= -half;  bY *= -root3*half;
      cX *= -half;  cY *= root3*half;
      break;
    case 2:
      aX *= root3*half;   aY *= half;
      bX *= 0;   bY *= -1;
      cX *= -root3*half;  cY *= half;
      break;
    case 3:
      aX *= half; aY *= -root3*half;
      bX *= -1;   bY *= 0;
      cX *= half; cY *= root3*half;
      break;
  }

  vtx2d_t *ptr;
  ptr = d20.current->A->pos[0];
  ptr->pts[0] = d20.x + aX;
  ptr->pts[1] = d20.y + aY;
  ptr = d20.current->B->pos[0];
  ptr->pts[0] = d20.x + bX;
  ptr->pts[1] = d20.y + bY;
  ptr = d20.current->C->pos[0];
  ptr->pts[0] = d20.x + cX;
  ptr->pts[1] = d20.y + cY;
}






void app_usage ()
{


  if (!d20.num_faces)
  { //  root triangle is not pinned
    int change = 0;
    if (mouse_moves())
    { // set root position
      d20.x = application.mX;
      d20.y = application.mY;
      mouse_position (&application.mX, &application.mY);
      ++ change;
    }
    else
    {
      if (mouse_left() == -1)
      {
        // pin
      }
      int scroll = mouse_scroll();
      if (change_root_size())
      { change += resize_root(scroll);  }
      else
      { change += rotate_root(scroll);  }
    }
    if (change)
      reposition_root_vertices ();
  }
  else
  {
    //  root triangle has been pinned

    relocate_and_undo();

    if (mouse_left() == -1)
    {
      //  pinning
    }

    int scroll = mouse_scroll();
    if (use_scroll_selector())
    {
      // selecting a triangle with the wheel
      if (scroll)
      {
        if (scroll == -1)
        {
          // backwards
        }
        if (scroll == 1)
        {
          // forwards
        }
      }
    }
    else
    {
      //selecting a triangle with position
      if (mouse_moves ())
      {
        mouse_position (&application.mX, &application.mY);

      }
    }
  }
}



