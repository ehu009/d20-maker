#include  <stdlib.h>
#include  <string.h>

#include  "SDL2/SDL_ttf.h"

#include  "toggler.h"

enum {TOGGLE_OVER = 1, TOGGLE_DOWN = 2, TOGGLE_SWITCH = 4};

struct toggle_switch
{
  SDL_Rect rect;
  SDL_Surface *text1, *text2;
  char status;
  unsigned *dst;
};



toggler_t *toggler_create (const char *txt1, const char *txt2, unsigned *value)
{

  toggler_t *toggle = NULL;

  TTF_Font *font = TTF_OpenFont ("cour.ttf", 12);
  if (font == NULL)
    return toggle;

  SDL_Surface *t1, *t2;
  t1 = TTF_RenderText_Solid (font, txt1, (SDL_Color) {0,0,0});
  t2 = TTF_RenderText_Solid (font, txt2, (SDL_Color) {0,0,0});
  TTF_CloseFont (font);

  if ((t1 == NULL)  ||  (t2 == NULL))
  {
    SDL_FreeSurface (t1);
    SDL_FreeSurface (t2);
    return toggle;
  }

  toggle = (toggler_t *) malloc (sizeof (toggler_t));
  if (toggle != NULL)
  {
    toggle->dst = value;
    toggle->status = *value;
    toggle->text1 = t1;
    toggle->text2 = t2;

    toggle->rect = (SDL_Rect) {
      .x = 0, .y = 0,
      .w = t1->w, .h = t1->h
    };
    if (t1->w < t2->w)
      toggle->rect.w = t2->w;

    if (t1->h < t2->h)
      toggle->rect.h = t2->h;

    toggle->rect.w += 10;
    toggle->rect.h += 10;
  }
  else
  {
    SDL_FreeSurface (t1);
    SDL_FreeSurface (t2);
  }

  return toggle;
}

void toggler_free (toggler_t *toggler)
{
  SDL_FreeSurface (toggler->text1);
  SDL_FreeSurface (toggler->text2);
  free(toggler);
}

void toggler_setPosition (toggler_t *button, unsigned _x, unsigned _y)
{
  if (button == NULL)
    return;
  button->rect.x = _x;
  button->rect.y = _y;
}





void toggler_update (toggler_t *button)
{
  int mouseX, mouseY;
  mouse_position(&mouseX, &mouseY);
  int mouseUp = (mouse_left() == 1),
      mouseDown = (mouse_left() == -1);


  if (((mouseX >= button->rect.x) &&  (mouseX <= button->rect.x + button->rect.w))
      && ((mouseY >= button->rect.y)  &&  (mouseY <= button->rect.y + button->rect.h)))
  {
    if (!(button->status & TOGGLE_OVER))
      button->status |= TOGGLE_OVER;
    if(button->status & TOGGLE_OVER)
    {
      if (!(button->status & TOGGLE_DOWN))
      {
        if (mouseDown)
        {
          button->status |= TOGGLE_DOWN;
          *button->dst ^= TOGGLE_SWITCH;
        }
      }
      else
      {
        if (!mouseDown)
          button->status ^= TOGGLE_DOWN;
      }
    }
  }
  else
  {
    if (!(button->status & TOGGLE_DOWN))
    {
      if (button->status & TOGGLE_OVER)
        button->status ^= TOGGLE_OVER;
    }
    else
    {
      if (!mouseDown)
        button->status ^= TOGGLE_DOWN;
    }
  }
}

void toggler_draw (toggler_t *button)
{
  if (button->status & TOGGLE_OVER)
  {
    if (button->status & TOGGLE_DOWN)
      SDL_FillRect (canvas, &button->rect, 0x00ffff);
    else
      SDL_FillRect (canvas, &button->rect, 0xffffff);
  }
  else
    SDL_FillRect (canvas, &button->rect, 0xff0000);

  SDL_Rect tmp_rect = {
    .x = button->rect.x+5,.y = button->rect.y+5
  };
  if (*button->dst & TOGGLE_SWITCH)
  {
    tmp_rect.w = button->text2->w;
    tmp_rect.h = button->text2->h;
    SDL_BlitSurface (button->text2, NULL, canvas, &tmp_rect);
  }
  else
  {
    tmp_rect.w = button->text1->w;
    tmp_rect.h = button->text1->h;
    SDL_BlitSurface (button->text1, NULL, canvas, &tmp_rect);
  }
}
