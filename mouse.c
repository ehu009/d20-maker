#include "mouse.h"


/*
 *
 */
#define STATE mouse_state
#define XPOS  mouseX
#define YPOS  mouseY

void mouse_reset (void)
{
  STATE = 0;
}

void mouse_handle_move (SDL_Event *e)
{
  XPOS = e->motion.x;
  YPOS = e->motion.y;
}

/*
 *
 */
#define MASK_L_UP   1
#define MASK_R_UP   2
#define MASK_M_UP   4
#define MASK_L_DOWN 8
#define MASK_R_DOWN 16
#define MASK_M_DOWN 32
#define MASK_WHEEL_UP   64
#define MASK_WHEEL_DOWN 128

#define SET(bit)  STATE |= bit
#define IS_BIT_SET(bit) ((STATE & bit) != 0)

/*
 *
 */
#define IS_L_UP   IS_BIT_SET(MASK_L_UP)
#define IS_L_DOWN IS_BIT_SET(MASK_L_DOWN)
#define IS_R_UP   IS_BIT_SET(MASK_R_UP)
#define IS_R_DOWN IS_BIT_SET(MASK_R_DOWN)
#define IS_M_UP   IS_BIT_SET(MASK_M_UP)
#define IS_M_DOWN IS_BIT_SET(MASK_M_DOWN)
#define IS_WHEEL_UP    IS_BIT_SET(MASK_WHEEL_UP)
#define IS_WHEEL_DOWN  IS_BIT_SET(MASK_WHEEL_DOWN)

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
int mouse_left_button (void)
{ return (int)  0 - IS_L_UP + IS_L_DOWN;  }

int mouse_right_button (void)
{ return (int)  0 - IS_R_UP + IS_R_DOWN;  }

int mouse_middle_button (void)
{ return (int)  0 - IS_M_UP + IS_M_DOWN;  }

int mouse_is_scrolled(void)
{ return (int)  0 - IS_WHEEL_UP + IS_WHEEL_DOWN;  }


