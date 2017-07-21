#include  "slider.h"


enum {SLIDESTATUS_OVER = 1, SLIDESTATUS_DOWN = 2};

struct mouseTrack
{
  unsigned msX, msY;
  int dragx, dragy;
};

struct UI_slide_bar
{
  SDL_Surface *surface;
  SDL_Rect  bar;
  TTF_Font  *font;
  SDL_Color font_color;
  unsigned  posx, posy;
  double    position;
  unsigned  status;
  struct  mouseTrack mouse;
};



UI_slider *UI_slider_create (double start, int width, int height)
{
  SDL_Surface *surf = SDL_CreateRGBSurface (0, width, height, 32, 0,0,0,0);
  if (surf == NULL)
    return NULL;
  UI_slider *slide  = (UI_slider *) malloc (sizeof(UI_slider));
  if (slide != NULL)
  {
    slide->posx = 0;
    slide->posy = 0;
    slide->font_color = (SDL_Color) {0,0,0};
    slide->position = start;
    slide->font = NULL;
    slide->bar    = (SDL_Rect) {.x=0, .y=0, .w=width, .h=height};
    slide->surface  = surf;
    slide->status = 0;
  }
  return slide;
}

void UI_slider_setPos (UI_slider *slide, int _x, int _y)
{
  if (_x >= 0)
    slide->posx = _x;
  if (_y >= 0)
    slide->posy = _y;
}

int UI_slider_setFont (UI_slider *slide, const char *font, unsigned font_size, SDL_Color font_color)
{
  if (slide->font != NULL)
  {
    TTF_CloseFont(slide->font);
  }
  slide->font = TTF_OpenFont (font, font_size);
  slide->font_color = font_color;
  if (slide->font == NULL)
    return -1;
  return 0;
}

void _UI_slider_draw (UI_slider *slide, SDL_Surface *screen, unsigned bar_color, unsigned btn_color)
{

  SDL_Rect temp_rect;
  if (slide->bar.w > slide->bar.h)
  {
    temp_rect = (SDL_Rect) {
      .x = (0.5 + (slide->bar.w -slide->bar.h) * slide->position / 100),
      .y = slide->bar.y,    .w = slide->bar.h,    .h = slide->bar.h
    };
  }
  else
  {
    temp_rect = (SDL_Rect) {
      .y = (0.5 + (slide->bar.h -slide->bar.w) * slide->position / 100),
      .x = slide->bar.x,    .w = slide->bar.w,    .h = slide->bar.w
    };
  }

  SDL_FillRect (slide->surface, NULL, bar_color);
  SDL_FillRect (slide->surface, &temp_rect, btn_color);
  if (slide->font != NULL)
  {
    char text[] = {0,0,0,0};
  //  bzero(text, 4);
    if ((slide->position == 100.0) || (slide->position == 0.0))
      sprintf (text, "%.0f", slide->position);
    else
      sprintf (text, "%.1f", slide->position);

    SDL_Surface *tmp_surf = TTF_RenderText_Solid (slide->font, text, slide->font_color);
    if (tmp_surf == NULL)
      printf ("TTF_RenderText_Solid: \"%s\"\n", TTF_GetError ());
    else
    {

      SDL_Rect text_rect;
      if (slide->bar.w > slide->bar.h)
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
  SDL_BlitSurface (slide->surface, NULL, screen, &slide->bar);
  slide->bar.x -= slide->posx;
  slide->bar.y -= slide->posy;
}

double UI_slider_update (UI_slider *slide, unsigned mouseX, unsigned mouseY, unsigned mouseIsDown, SDL_Surface *screen)
{
  SDL_Rect temp_rect;
  if (slide->bar.w > slide->bar.h)
  {
    temp_rect = (SDL_Rect) {
      .x = (0.5 + (slide->bar.w - slide->bar.h) * slide->position / 100),
      .y = slide->bar.y,    .w = slide->bar.h,    .h = slide->bar.h
    };
  }
  else
  {
    temp_rect = (SDL_Rect) {
      .y = (0.5 + (slide->bar.h - slide->bar.w) * slide->position / 100),
      .x = slide->bar.x,    .w = slide->bar.w,    .h = slide->bar.w
    };
  }
  temp_rect.x += slide->posx;
  temp_rect.y += slide->posy;

  if ((mouseX >= temp_rect.x && (mouseX <= temp_rect.x + temp_rect.w))
      &&  (mouseY >= temp_rect.y && (mouseY <= temp_rect.y + temp_rect.h)))
  {
    if (!(SLIDESTATUS_OVER & slide->status))
      slide->status |= SLIDESTATUS_OVER;
    if ((SLIDESTATUS_OVER & slide->status))
    {
      if (mouseIsDown)
      {
        if (!(SLIDESTATUS_DOWN & slide->status))
        {
          slide->mouse.msX = mouseX;
          slide->mouse.msY = mouseY;
          slide->status |= SLIDESTATUS_DOWN;
        }
      }
      else
      {
        if (SLIDESTATUS_DOWN & slide->status)
          slide->status ^= SLIDESTATUS_DOWN;
      }
    }
  }
  else
  {
    if (!(SLIDESTATUS_DOWN & slide->status))
    {
      if (SLIDESTATUS_OVER & slide->status)
        slide->status ^= SLIDESTATUS_OVER;
    }
    else
    {
      if (!mouseIsDown)
        slide->status ^= SLIDESTATUS_DOWN;
    }

  }


  if (SLIDESTATUS_OVER & slide->status)
  {
    if (SLIDESTATUS_DOWN & slide->status)
    {
      slide->mouse.dragx = mouseX - slide->mouse.msX;
      slide->mouse.dragy = mouseY - slide->mouse.msY;


      int *drag;
      unsigned *track;
      unsigned b1, b2, interval;
      if (slide->bar.w > slide->bar.h)
      {
        drag = &slide->mouse.dragx;
        track = &mouseX;
        b1 = slide->posx;
        b2 = slide->bar.w;
        interval = slide->bar.w - slide->bar.h;
      }
      else
      {
        drag = &slide->mouse.dragy;
        track = &mouseY;
        b1 = slide->posy;
        b2 = slide->bar.h;
        interval = slide->bar.h - slide->bar.w;
      }
      b2 += b1;
      if (((*drag > 0) && (b1 < *track))
          ||  ((*drag < 0) && (b2 > *track)))
      {
        double t = (*drag)*100;
        t /= interval;
        t += slide->position;
        if (t < 0.0)
          t = 0.0;
        if (t > 100.0)
          t = 100.0;
        slide->position = t;
      }
      slide->mouse.msX = mouseX;
      slide->mouse.msY = mouseY;
      _UI_slider_draw (slide, screen, 0xff0000, 0x00ffff);
    }
    else
      _UI_slider_draw (slide, screen, 0xff0000, 0xffffff);
  }
  else
    _UI_slider_draw (slide, screen, 0xff0000, 0xff);
  return slide->position;
}


void  UI_slider_free (UI_slider *slide)
{
  if (slide != NULL)
  {
  SDL_FreeSurface (slide->surface);
  TTF_CloseFont (slide->font);
}
  free (slide);
}
