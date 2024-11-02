#include  "fader.h"

#include  <stdlib.h>
#include  <stdio.h>

#include  "memory.h"
#include  "colours.h"
#include  "font.h"
#include  "mouse_app.h"


enum {SLIDESTATUS_OVER = 1, SLIDESTATUS_DOWN = 2, SLIDESTATUS_UP = 4};


struct fader_bar
{
  SDL_Surface *surface;
  SDL_Rect  bar;
  TTF_Font  *font;
  unsigned  posx, posy;
  double    position;
  double    *value;
  unsigned  status;

  unsigned diff;
};


fader_t *make_fader (double start, int width, int height, double *value)
{
  SDL_Surface *surf = SDL_CreateRGBSurface (0, width, height, 32, 0,0,0,0);
  if (surf == NULL)
    return NULL;
  fader_t *slide  = (fader_t *) malloc (sizeof(fader_t));
  if (slide != NULL)
  {
    slide->posx = 0;
    slide->posy = 0;

    slide->position = start;
    slide->font = NULL;
    slide->bar    = (SDL_Rect) {.x=0, .y=0, .w=width, .h=height};
    slide->surface  = surf;
    slide->status = 0;

    slide->value = value;
    slide->font = FONT_DEFAULT;
  }
  return slide;
}

void fader_reposition (fader_t *slide, int _x, int _y)
{
  if (_x >= 0)
    slide->posx = _x;
  if (_y >= 0)
    slide->posy = _y;
}


void get_button_rect (fader_t *slide, SDL_Rect *buf)
{
  if (slide->bar.w > slide->bar.h)
  {
    *buf = (SDL_Rect) {
      .x = (0.5 + ((slide->bar.w -slide->bar.h) * slide->position) / 100),
      .y = slide->bar.y,    .w = slide->bar.h,    .h = slide->bar.h
    };
  }
  else
  {
    *buf = (SDL_Rect) {
      .y = (0.5 + ((slide->bar.h -slide->bar.w) * slide->position) / 100),
      .x = slide->bar.x,    .w = slide->bar.w,    .h = slide->bar.w
    };
  }
}



void fader_draw (fader_t *slide)
{
  int horizontal = (slide->bar.w > slide->bar.h);
  SDL_FillRect (slide->surface, NULL, FADER_COLOUR_BAR);

  SDL_Rect button_rect;
  get_button_rect(slide, &button_rect);
  COLOUR btn_clr;
  if (SLIDESTATUS_OVER & slide->status)
    btn_clr = FADER_COLOUR_OVER;
  else
    btn_clr = FADER_COLOUR_DEFAULT;
  if (SLIDESTATUS_DOWN & slide->status)
    btn_clr = FADER_COLOUR_DOWN;

  SDL_FillRect (slide->surface, &button_rect, btn_clr);

  if (slide->font != NULL)
  {
    char text[] = {0,0,0,0};
  //  bzero(text, 4);
    if ((slide->position == 100.0) || (slide->position == 0.0))
      sprintf (text, "%.0f", slide->position);
    else
      sprintf (text, "%.1f", slide->position);

    SDL_Surface *tmp_surf = TTF_RenderText_Solid (slide->font, text, FONT_COLOUR_WHITE);
    if (tmp_surf == NULL)
      printf ("TTF_RenderText_Solid: \"%s\"\n", TTF_GetError ());
    else
    {

      SDL_Rect text_rect;
      if (horizontal)
      {
        text_rect = (SDL_Rect) {
          .x = (0.5 + (slide->bar.w - slide->bar.h) * slide->position / 100),
          .y = (0.5 + slide->bar.h / 2) - (0.5 + tmp_surf->h /2),
          .w = tmp_surf->w, .h = tmp_surf->h
        };
      }
      else
      {
        text_rect = (SDL_Rect) {
          .y = 0.5 + (((slide->bar.h - slide->bar.w) * slide->position / 100) + (0.5 + tmp_surf->h/2)),
          .x = (0.5 + slide->bar.w / 2) - (0.5 + tmp_surf->w /2),
          .w = tmp_surf->w, .h = tmp_surf->h
        };
      }
      if (SDL_BlitSurface (tmp_surf, NULL, slide->surface, &text_rect))
      {
        printf ("SDL_Blitscreen: \"%s\"\n", SDL_GetError ());
      }
      SDL_FreeSurface (tmp_surf);
    }
  }
  slide->bar.x += slide->posx;
  slide->bar.y += slide->posy;

  SDL_BlitSurface (slide->surface, NULL, canvas, &slide->bar);

  slide->bar.x -= slide->posx;
  slide->bar.y -= slide->posy;

}

void fader_update (fader_t *slide)
{
  int horizontal = (slide->bar.w > slide->bar.h);
  int mouseX, mouseY;
  mouse_position(&mouseX, &mouseY);
  int mouseIsDown = (mouse_left() == 1),
      mouseIsUp = (mouse_left() == -1);


  SDL_Rect temp_rect;
  get_button_rect (slide, &temp_rect);

  temp_rect.x += slide->posx;
  temp_rect.y += slide->posy;

  if ((mouseX >= temp_rect.x && (mouseX <= temp_rect.x + temp_rect.w))
      &&  (mouseY >= temp_rect.y && (mouseY <= temp_rect.y + temp_rect.h)))
  {
    if (!(SLIDESTATUS_OVER & slide->status))
      slide->status |= SLIDESTATUS_OVER;
    else
    {
      if (mouseIsDown)
      {
        if (!(SLIDESTATUS_DOWN & slide->status))
        {
          if (horizontal)
            slide->diff = mouseX - temp_rect.x;
          else
            slide->diff = mouseY - temp_rect.y;
          slide->status |= SLIDESTATUS_DOWN;
        }
      }
      if (mouseIsUp)
      {
        if (SLIDESTATUS_DOWN & slide->status)
          slide->status ^= SLIDESTATUS_DOWN;
      }
    }
  }
  else
  {
    if (SLIDESTATUS_OVER & slide->status)
        slide->status ^= SLIDESTATUS_OVER;
    if (SLIDESTATUS_DOWN & slide->status)
    {
      if (mouseIsUp)
        slide->status ^= SLIDESTATUS_DOWN;
    }

  }

  if (SLIDESTATUS_DOWN & slide->status)
  {

    int *tester = &mouseY;
    unsigned bMin, bMax;

    if (horizontal)
    {
      tester = &mouseX;
      bMin = slide->posx + slide->diff;
      bMax = slide->bar.w - (slide->bar.h );
    }
    else
    {
      bMin = slide->posy + slide->diff;
      bMax = slide->bar.h - (slide->bar.w );
    }
    bMax += bMin;

    if (*tester < bMin)
    {
      slide->position = 0.0;
    }
    else if (*tester > bMax)
    {
      slide->position = 100.0;
    }
    else
    {
      slide->position = *tester;
      slide->position -= bMin;
      slide->position *= 100;
      if (horizontal)
        slide->position /= (slide->bar.w - slide->bar.h);
      else
        slide->position /= (slide->bar.h - slide->bar.w);
    }
  }
  *slide->value = slide->position;
}


void  free_fader (fader_t *slide)
{
  if (slide != NULL)
  {
  SDL_FreeSurface (slide->surface);
  TTF_CloseFont (slide->font);
}
  free (slide);
}
