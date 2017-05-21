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

struct application
{
  //  togglers
  char rotate_root;
  char use_slider;
  //  other things
  //  ...
  app_status status;
} app;




int rotate_root_triangle(void)
{ //  toggle between rotation and resizing of root triangle
  if((mouse_middle() == -1)
      ||  (mouse_left() == -1))
  {
    app.rotate_root ^= 1;
  }
  return app.rotate_root;
}

int slide_selector(void)
{ //  toggle between using mouse wheel or position for selections
  if(mouse_middle() == -1)
  {
    app.use_slider ^= 1;
  }
  return app.use_slider;
}



void app_draw (void)
{
  SDL_FillRect (canvas, NULL, 0x0ff);
  SDL_BlitSurface (src_image, NULL, canvas, NULL);
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

  if (1)
  { //  root triangle is not pinned

    // set triangle position

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
      }
    }
    else
    {
      if (scroll)
      {
        // rotate
      }
    }
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


void app_start (void)
{
  app.rotate_root = 1;
  app.use_slider = 1;
  app.status = APP_START;

  // initialize structures
}

void app_free (void)
{
  //  free initialized structures

}
