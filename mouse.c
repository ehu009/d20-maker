#include "mouse.h"
#include "mouse_app.h"

/*
 *
 */
#define STATE mouse.buttons
//#define XPOS  mouseX
//#define YPOS  mouseY

void mouse_reset (void)
{
  STATE = 0;
}

void mouse_handle_move (SDL_Event *e)
{
  mouse._x = e->motion.x;
  mouse._y = e->motion.y;
}

/*
 *
 */
#define SET(bit)  STATE |= bit
//
#define MASK_L_UP   1
#define MASK_R_UP   2
#define MASK_M_UP   4
#define MASK_L_DOWN 8
#define MASK_R_DOWN 16
#define MASK_M_DOWN 32
#define MASK_WHEEL_UP   64
#define MASK_WHEEL_DOWN 128

/*
 *  event handles
 */
void mouse_handle_up (SDL_Event *e)
{
  if (e->button.state == SDL_RELEASED
      &&  e->button.clicks == 1)
  {
    switch (e->button.button)
    {
      case SDL_BUTTON_LEFT:   { SET(MASK_L_UP); break;  }
      case SDL_BUTTON_RIGHT:  { SET(MASK_R_UP); break;  }
      case SDL_BUTTON_MIDDLE: { SET(MASK_M_UP); break;  }
    }
  }
}

void mouse_handle_down (SDL_Event *e)
{
  if (e->button.state == SDL_PRESSED
      &&  e->button.clicks == 1)
  {
    switch (e->button.button)
    {
      case SDL_BUTTON_LEFT:   { SET(MASK_L_DOWN); break;  }
      case SDL_BUTTON_RIGHT:  { SET(MASK_R_DOWN); break;  }
      case SDL_BUTTON_MIDDLE: { SET(MASK_M_DOWN); break;  }
    }
  }
}

void mouse_handle_scroll (SDL_Event *e)
{
  if (e->wheel.y < 0)
    SET(MASK_WHEEL_DOWN);
  if (e->wheel.y > 0)
    SET(MASK_WHEEL_UP);
}

void mouse_update (SDL_Event *e)
{
  switch (e->type)
  {
    case SDL_MOUSEBUTTONUP:   { mouse_handle_up (e);    break;  }
    case SDL_MOUSEBUTTONDOWN: { mouse_handle_down (e);  break;  }
    case SDL_MOUSEMOTION:     { mouse_handle_move (e);  break;  }
    case SDL_MOUSEWHEEL:      { mouse_handle_scroll (e);break;  }
  }
}


/*
 *
 */
#define IS_BIT_SET(bit, num) ((num & bit) != 0)
//
#define IS_L_UP(x)   IS_BIT_SET(MASK_L_UP, x)
#define IS_L_DOWN(x) IS_BIT_SET(MASK_L_DOWN, x)
#define IS_R_UP(x)   IS_BIT_SET(MASK_R_UP, x)
#define IS_R_DOWN(x) IS_BIT_SET(MASK_R_DOWN, x)
#define IS_M_UP(x)   IS_BIT_SET(MASK_M_UP, x)
#define IS_M_DOWN(x) IS_BIT_SET(MASK_M_DOWN, x)
#define IS_WHEEL_UP(x)    IS_BIT_SET(MASK_WHEEL_UP, x)
#define IS_WHEEL_DOWN(x)  IS_BIT_SET(MASK_WHEEL_DOWN, x)



/*
 *
 */
int mouse_left(mouse_t *m)
{
  return (int)  0 - IS_L_UP(m->buttons) + IS_L_DOWN(m->buttons);
}

int mouse_right(mouse_t *m)
{
  return (int)  0 - IS_R_UP(m->buttons) + IS_R_DOWN(m->buttons);
}

int mouse_middle(mouse_t *m)
{
  return (int)  0 - IS_M_UP(m->buttons) + IS_M_DOWN(m->buttons);
}

int mouse_scroll(mouse_t *m)
{
  return (int)  0 - IS_WHEEL_UP(m->buttons) + IS_WHEEL_DOWN(m->buttons);
}
