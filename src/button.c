#include  <string.h>

#include  "button.h"
#include  "memory.h"
#include  "font.h"
#include  "colours.h"
#include  "mouse_app.h"


#define   TEXT_MARGIN 5

enum {BUTTON_OVER=1, BUTTON_DOWN=2};


struct button_switch
{
  //  drawing
  SDL_Rect rect;
  TTF_Font *font;
  SDL_Surface *surf;

  //  function
  char status;
  unsigned *dst;
};



button_t *make_button (const char *txt1, unsigned *value)
{

  button_t *button = (button_t *) MALLOC(sizeof (button_t));
  if (button == NULL)
  {
    return button;
  }

  TTF_Font *font = FONT_DEFAULT;
  if (font == NULL)
  {
    return button;
  }
  button->font = font;

  SDL_Surface *surface = TTF_RenderText_Solid(font, txt1, FONT_COLOUR_WHITE);
  if (surface == NULL)
  {
    TTF_CloseFont(font);
    return button;
  }
  button->surf = surface;

  button->dst = value;
  button->status = 0;
  button->rect = (SDL_Rect) {
    .x = 0, .y = 0,
    .w = surface->w + TEXT_MARGIN*2,
    .h = surface->h + TEXT_MARGIN*2
  };
  return button;
}

void free_button (button_t *button)
{
  if (button == NULL)
  {
    return;
  }
  TTF_CloseFont(button->font);
  SDL_FreeSurface(button->surf);
  FREE(button);
}

void button_reposition (button_t *button, unsigned _x, unsigned _y)
{
  if (button == NULL)
  {
    return;
  }
  button->rect.x = _x;
  button->rect.y = _y;
}



void button_update (button_t *button)
{
  int mouseX, mouseY;
  mouse_position(&mouseX, &mouseY);

  *button->dst = 0;

  if (!(button->status & BUTTON_OVER))
  {
    if (((mouseX >= button->rect.x)
            &&  (mouseX <= button->rect.x + button->rect.w))
        && ((mouseY >= button->rect.y)
            &&  (mouseY <= button->rect.y + button->rect.h)))
    {
      button->status |= BUTTON_OVER;
    }
  }

  int mouseUp = (mouse_left() == -1),
      mouseDown = (mouse_left() == 1);

  if (button->status & BUTTON_OVER)
  {
    if (mouseDown && !(button->status & BUTTON_DOWN))
    {
      button->status = BUTTON_DOWN;
    }
    if (!(((mouseX >= button->rect.x)
            &&  (mouseX <= button->rect.x + button->rect.w))
        && ((mouseY >= button->rect.y)
            &&  (mouseY <= button->rect.y + button->rect.h))))
    {
      button->status ^= BUTTON_OVER;
    }
  }

  if (mouseUp && (button->status & BUTTON_DOWN))
  {
    button->status ^= BUTTON_DOWN;
    *button->dst = 1;
  }
}

void button_draw (button_t *button)
{
  switch (button->status)
  {
    case BUTTON_OVER:
      SDL_FillRect(canvas, &button->rect, BUTTON_COLOUR_OVER);
      break;
    case BUTTON_DOWN:
      SDL_FillRect(canvas, &button->rect, BUTTON_COLOUR_DOWN);
      break;
    default:
      SDL_FillRect(canvas, &button->rect, BUTTON_COLOUR_DEFAULT);
      break;
  }

  SDL_Rect tmp_rect = {
    .x = button->rect.x + TEXT_MARGIN,
    .y = button->rect.y + TEXT_MARGIN,
    .w = button->surf->w,
    .h = button->surf->h
  };
  SDL_BlitSurface(button->surf, NULL, canvas, &tmp_rect);
}
