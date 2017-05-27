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


int rotate_root_triangle(void)
{ //  toggle between rotation and resizing of root triangle
  if((mouse_middle() == -1)
      ||  (mouse_right() == -1))
  {
    application.rotate_root ^= 1;
  }
  return application.rotate_root;
}

int slide_selector(void)
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

struct working_triangle
{
  slot_t *A, *B, *C;
//  vtx2d_t **A, **B, **C;
};


static struct
{
  int x, y;
  double radius;
  int rotation;
  int num_faces;

  struct working_triangle current;
  slot_t vertex[NUM_D20_VTX];
} d20;



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
    s->pinned = 0;
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

  d20.current.A = &d20.vertex[0];
  d20.current.B = &d20.vertex[1];
  d20.current.C = &d20.vertex[2];

  d20.current.A->pos[0] = calloc (1, sizeof(vtx2d_t));
  d20.current.B->pos[0] = calloc (1, sizeof(vtx2d_t));
  d20.current.C->pos[0] = calloc (1, sizeof(vtx2d_t));
}

void app_free (void)
{
  //  free initialized structures
  int j = 0, k = 0;
  slot_t *s;
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

void draw_root (void)
{
  vtx2i_t p1, p2, p3;
  get_vtx2i_from_vtx2d(d20.current.A->pos[0], &p1);
  get_vtx2i_from_vtx2d(d20.current.B->pos[0], &p2);
  get_vtx2i_from_vtx2d(d20.current.C->pos[0], &p3);

  draw_line2 (canvas, &p1, &p2, invertPixel, 0);
  draw_line2 (canvas, &p2, &p3, invertPixel, 0);
  draw_line2 (canvas, &p3, &p1, invertPixel, 0);
}


void app_draw (void)
{
  SDL_FillRect (canvas, NULL, 0x0ff);
  SDL_BlitSurface (src_image, NULL, canvas, NULL);

  if (!d20.num_faces)
  {
    //fill_invert_screen_triangle (d20.pos[0], d20.pos[1], d20.pos[2], canvas);
    draw_root ();
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

void change_root (void)
{
  double radi = d20.radius;
  double r1 = (90 - (30 * d20.rotation)),
      r2 = (90 - (30 * d20.rotation) - 120),
      r3 = (90 - (30 * d20.rotation) + 120);
  r1 *= M_PI/180;
  r2 *= M_PI/180;
  r3 *= M_PI/180;

  vtx2d_t *ptr;
  ptr = d20.current.A->pos[0];
  ptr->pts[0] = d20.x + sin(r1) * radi;
  ptr->pts[1] = d20.y + cos(r1) * radi;
  ptr = d20.current.B->pos[0];
  ptr->pts[0] = d20.x + sin(r2) * radi;
  ptr->pts[1] = d20.y + cos(r2) * radi;
  ptr = d20.current.C->pos[0];
  ptr->pts[0] = d20.x + sin(r3) * radi;
  ptr->pts[1] = d20.y + cos(r3) * radi;
}

void app_usage ()
{


  if (!d20.num_faces)
  { //  root triangle is not pinned
    int change = 0;
    if (mouse_moves())
    { // set triangle position
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
      if (rotate_root_triangle())
      {
        if (scroll)
        {
          // increase radius
          d20.radius += scroll*1.75;
          ++ change;
        }
      }
      else
      {
        if (scroll)
        {
          // rotate
          d20.rotation += scroll;
          ++ change;
        }
      }

    }
    if (change)
      change_root ();


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
    if (slide_selector())
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



