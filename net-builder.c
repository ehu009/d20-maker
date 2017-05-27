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
vtx5i_t d20_model[12] =
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

static struct
{
  double x, y;
  double radius;
  int rotation;
  int num_faces;

  vtx2i_t *pos[12];
  vtx2i_t *invalid[12];
  vtx5i_t linkage[12];
} d20;



void app_start (void)
{
  application.rotate_root = 1;
  application.use_slider = 1;
  application.status = APP_START;

  // initialize structures
  int j = 0;
  for (; j < 12; j ++)
  {
    d20.pos[j] = NULL;
    d20.invalid[j] = NULL;
    d20.linkage[j] = d20_model[j];
  }

  d20.num_faces = 0;
  d20.radius = 30.0;
  d20.x = 50.0;
  d20.y = 50.0;

  d20.pos[0] = calloc (1, sizeof(vtx2i_t));
  d20.pos[1] = calloc (1, sizeof(vtx2i_t));
  d20.pos[2] = calloc (1, sizeof(vtx2i_t));

}

void app_free (void)
{
  //  free initialized structures
  int j = 0;
  for (; j < 12; j ++)
  {
    if (d20.pos[j] != NULL)
      free (d20.pos[j]);
    if (d20.invalid[j] != NULL)
      free (d20.invalid[j]);
  }


}


void app_draw (void)
{
  SDL_FillRect (canvas, NULL, 0x0ff);
  SDL_BlitSurface (src_image, NULL, canvas, NULL);

  if (!d20.num_faces)
  {
    //fill_invert_screen_triangle (d20.pos[0], d20.pos[1], d20.pos[2], canvas);
    draw_line2 (canvas, d20.pos[0], d20.pos[1], invertPixel, 0);
    draw_line2 (canvas, d20.pos[1], d20.pos[2], invertPixel, 0);
    draw_line2 (canvas, d20.pos[2], d20.pos[0], invertPixel, 0);
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



void app_usage ()
{
  int mX, mY;
  mouse_position (&mX, &mY);

  if (!d20.num_faces)
  { //  root triangle is not pinned

    if (mouse_moves())
    { // set triangle position
      d20.x = mX;
      d20.y = mY;
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
      }
    }
    else
    {
      if (scroll)
      {
        // rotate
        d20.rotation += scroll;
      }
    }
    }

    double radi = d20.radius;
    double r1 = (90 - (30 * d20.rotation)),
        r2 = (90 - (30 * d20.rotation) - 120),
        r3 = (90 - (30 * d20.rotation) + 120);
    r1 *= M_PI/180;
    r2 *= M_PI/180;
    r3 *= M_PI/180;

    d20.pos[0]->pts[0] = d20.x + sin(r1) * radi;
    d20.pos[0]->pts[1] = d20.y + cos(r1) * radi;

    d20.pos[1]->pts[0] = d20.x + sin(r2) * radi;
    d20.pos[1]->pts[1] = d20.y + cos(r2) * radi;

    d20.pos[2]->pts[0] = d20.x + sin(r3) * radi;
    d20.pos[2]->pts[1] = d20.y + cos(r3) * radi;

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

      }
    }
  }
}



