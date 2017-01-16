#include "mouse.h"

/*
 *
 */
#define MASK_DOWN 1
#define MASK_UP   2
#define MASK_HELD 4
#define MASK_WHEEL_DOWN 8
#define MASK_WHEEL_UP   16

/*
 *
 */
#define IS_DOWN (mouse_state & MASK_DOWN)
#define IS_UP   (mouse_state & MASK_UP)
#define IS_HELD (mouse_state & MASK_HELD)
#define IS_WHEEL_DOWN  (mouse_state & MASK_WHEEL_DOWN)
#define IS_WHEEL_UP    (mouse_state & MASK_WHEEL_UP)
//
#define TOGGLE_DOWN mouse_state ^= MASK_DOWN
#define TOGGLE_UP   mouse_state ^= MASK_UP
#define TOGGLE_HELD mouse_state ^= MASK_HELD
#define TOGGLE_WHEEL_DOWN  mouse_state ^= MASK_WHEEL_DOWN
#define TOGGLE_WHEEL_UP    mouse_state ^= MASK_WHEEL_UP

/*
 *
 */
#define SET_DOWN  mouse_state |= MASK_DOWN
#define SET_UP    mouse_state |= MASK_UP
#define SET_HELD  mouse_state |= MASK_HELD
#define SET_WHEEL_DOWN  mouse_state |= MASK_WHEEL_DOWN
#define SET_WHEEL_UP    mouse_state |= MASK_WHEEL_UP
//
#define RESET_DOWN  if IS_DOWN  TOGGLE_DOWN
#define RESET_UP    if IS_UP    TOGGLE_UP
#define RESET_HELD  if IS_HELD  TOGGLE_HELD
#define RESET_WHEEL_DOWN  if IS_WHEEL_DOWN  TOGGLE_WHEEL_DOWN
#define RESET_WHEEL_UP    if IS_WHEEL_UP    TOGGLE_WHEEL_UP

/*
 *  event handles
 */
void mouse_handle_move (SDL_Event *e)
{
  mouseX = e->motion.x;
  mouseY = e->motion.y;
}

void mouse_handle_down ()
{
  SET_DOWN;
}

void mouse_handle_up ()
{
  RESET_HELD;
  SET_UP;
}

void mouse_handle_scroll (SDL_Event *e)
{
  if (e->wheel.y < 0)
    SET_WHEEL_DOWN;
  if (e->wheel.y > 0)
    SET_WHEEL_UP;
}

/*
 *
 */
void mouse_reset (void)
{
  if  IS_DOWN SET_HELD;
  RESET_DOWN;
  RESET_UP;
  RESET_WHEEL_DOWN;
  RESET_WHEEL_UP;
}

void mouse_update (SDL_Event *e)
{
  switch (e->type)
  {
    case SDL_MOUSEBUTTONDOWN:
    { mouse_handle_down ();   break;  }

    case SDL_MOUSEBUTTONUP:
    { mouse_handle_up ();     break;  }

    case SDL_MOUSEMOTION:
    { mouse_handle_move (e);  break;  }

    case SDL_MOUSEWHEEL:
    { mouse_handle_scroll (e);break;  }
  }
}

/*
 *
 */
int mouse_is_down (void)
{
  return (int) IS_DOWN;
}

int mouse_is_up (void)
{
  return (int)  IS_UP;
}

int mouse_is_held (void)
{
  return (int) IS_HELD;
}

int mouse_is_scrolled(void)
{
  return (int) 0 - (IS_WHEEL_DOWN != 0) + (IS_WHEEL_UP != 0);
}
