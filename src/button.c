#include  <stdlib.h>
#include  <string.h>

#include  "SDL2/SDL_ttf.h"

#include  "button.h"

enum {BUTTON_OVER = 1, BUTTON_DOWN = 2, BUTTON_SWITCH = 4};

struct button_switch
{
  SDL_Rect rect;
  SDL_Surface *text1;
  char status;
  unsigned *dst;
};



button_t *button_create (const char *txt1, unsigned *value)
{

  button_t *button = NULL;

  TTF_Font *font = TTF_OpenFont ("cour.ttf", 12);
  if (font == NULL)
    return button;

  SDL_Surface *t1;
  t1 = TTF_RenderText_Solid (font, txt1, (SDL_Color) {0,0,0});
  TTF_CloseFont (font);

  if (t1 == NULL)
  {
    SDL_FreeSurface (t1);
    return button;
  }

  button = (button_t *) malloc (sizeof (button_t));
  if (button != NULL)
  {
    button->dst = value;
    button->status = *value;
    button->text1 = t1;

    button->rect = (SDL_Rect) {
      .x = 0, .y = 0,
      .w = t1->w, .h = t1->h
    };
    button->rect.w += 10;
    button->rect.h += 10;
  }
  else
  {
    SDL_FreeSurface (t1);
  }

  return button;
}

void button_free (button_t *button)
{
  SDL_FreeSurface (button->text1);
  free(button);
}

void button_setPosition (button_t *button, unsigned _x, unsigned _y)
{
  if (button == NULL)
    return;
  button->rect.x = _x;
  button->rect.y = _y;
}





void button_update (button_t *button)
{
  int mouseX, mouseY;
  mouse_position(&mouseX, &mouseY);
  int mouseUp = (mouse_left() == -1),
      mouseDown = (mouse_left() == 1);
  *button->dst = 0;

  if (((mouseX >= button->rect.x) &&  (mouseX <= button->rect.x + button->rect.w))
      && ((mouseY >= button->rect.y)  &&  (mouseY <= button->rect.y + button->rect.h)))
  {
    if (!(button->status & BUTTON_OVER))
      button->status |= BUTTON_OVER;
    if(button->status & BUTTON_OVER)
    {
      if (!(button->status & BUTTON_DOWN))
      {
        if (mouseDown)
        {
          button->status |= BUTTON_DOWN;
        }
      }
      else
      {
        if (mouseUp)
        {
          button->status ^= BUTTON_DOWN;
          *button->dst = 1;
        }
      }
    }
  }
  else
  {
    button->status = 0;
  }
  /*
  return ((button->status & BUTTON_SWITCH) != 0);
  */
}

void button_draw (button_t *button)
{
  if (button->status & BUTTON_OVER)
  {
    if (button->status & BUTTON_DOWN)
      SDL_FillRect (canvas, &button->rect, 0x00ffff);
    else
      SDL_FillRect (canvas, &button->rect, 0xffffff);
  }
  else
    SDL_FillRect (canvas, &button->rect, 0xff0000);

  SDL_Rect tmp_rect = {
    .x = button->rect.x+5,.y = button->rect.y+5
  };

  tmp_rect.w = button->text1->w;
  tmp_rect.h = button->text1->h;
  SDL_BlitSurface (button->text1, NULL, canvas, &tmp_rect);
}
