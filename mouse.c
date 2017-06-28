#include "mouse.h"
#include "mouse_app.h"



#define STATE mouse.buttons




void mouse_reset (void)
{ //  Reset mouse state
  STATE = 0;
  mouse.moving = 0;
}

void mouse_handle_move (SDL_Event *e)
{ //  Handle mouse movement
  mouse._x = e->motion.x;
  mouse._y = e->motion.y;
  mouse.moving = 1;
}

/*
 *  Bitmask identities of mouse buttons
 */
#define SET(bit)  STATE |= bit

#define MASK_L_UP   1
#define MASK_R_UP   2
#define MASK_M_UP   4
#define MASK_L_DOWN 8
#define MASK_R_DOWN 16
#define MASK_M_DOWN 32
#define MASK_WHEEL_UP   64
#define MASK_WHEEL_DOWN 128


/*
 *  Mouse event handles
 */

void mouse_handle_up (SDL_Event *e)
{ //  Handle mouse button release
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
{ //  Handle mouse button press
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
{ //  Handle mouse scroll event
  if (e->wheel.y < 0)
    SET(MASK_WHEEL_DOWN);
  if (e->wheel.y > 0)
    SET(MASK_WHEEL_UP);
}

int mouse_update (SDL_Event *e)
{ //  Check for and handle mouse events
  int q = 1;
  mouse.moving = 0;
  switch (e->type)
  {
    case SDL_MOUSEBUTTONUP:   { mouse_handle_up (e);    ++q;  break;  }
    case SDL_MOUSEBUTTONDOWN: { mouse_handle_down (e);  ++q;  break;  }
    case SDL_MOUSEMOTION:     { mouse_handle_move (e);  ++q;  break;  }
    case SDL_MOUSEWHEEL:      { mouse_handle_scroll (e);++q;  break;  }
  }
  return q;
}


/*
 *  Read based on bitmasks
 */
#define IS_BIT_SET(bit, num) ((num & bit) != 0)

#define IS_L_UP(x)   IS_BIT_SET(MASK_L_UP, x)
#define IS_L_DOWN(x) IS_BIT_SET(MASK_L_DOWN, x)
#define IS_R_UP(x)   IS_BIT_SET(MASK_R_UP, x)
#define IS_R_DOWN(x) IS_BIT_SET(MASK_R_DOWN, x)
#define IS_M_UP(x)   IS_BIT_SET(MASK_M_UP, x)
#define IS_M_DOWN(x) IS_BIT_SET(MASK_M_DOWN, x)
#define IS_WHEEL_UP(x)    IS_BIT_SET(MASK_WHEEL_UP, x)
#define IS_WHEEL_DOWN(x)  IS_BIT_SET(MASK_WHEEL_DOWN, x)


/*
 *  Read mouse movement state
 */
int mouse_moves (void)
{
  return mouse.moving;
}

void mouse_position (int *x, int *y)
{
  *x = mouse._x;
  *y = mouse._y;
}


/*
 *  Read mouse button state
 */
int mouse_left (void)
{
  return (int)  0 - IS_L_UP(mouse.buttons) + IS_L_DOWN(mouse.buttons);
}

int mouse_right (void)
{
  return (int)  0 - IS_R_UP(mouse.buttons) + IS_R_DOWN(mouse.buttons);
}

int mouse_middle (void)
{
  return (int)  0 - IS_M_UP(mouse.buttons) + IS_M_DOWN(mouse.buttons);
}

int mouse_scroll (void)
{
  return (int)  0 - IS_WHEEL_UP(mouse.buttons) + IS_WHEEL_DOWN(mouse.buttons);
}
