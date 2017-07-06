#include    <stdlib.h>
#include    <stdio.h>

#include    "SDL2/SDL_image.h"
#include    "SDL2/SDL_ttf.h"

#include "net-builder.h"

char *default_path = "default.jpg\0";
char *image_path = NULL;

int init (void);
void unload (void);
const uint32_t update_interval = 1000/64;

static uint32_t draw_callback (uint32_t interval, void *param);

#define BORDER_SIZE 7

#define SIDE_BAR_W  400 + BORDER_SIZE
#define SIDE_BAR_H  400 + 2*BORDER_SIZE

SDL_Window *myWindow = NULL;
#define WINDOW_TITLE  "Icosahedron maker\0"


SDL_Surface *canvas = NULL,
    *src_image = NULL;

SDL_Rect draw_area;


static int exit_condition (SDL_Event *event);


int main (int argc, char *argv[])
{
  if (argc > 1)
    image_path = argv[1];
  else
    image_path = default_path;

  if (!(init ()))
  {
    printf ("Something went wrong- exiting.\n");
    unload ();
    return 1;
  }

  app_start ();

  SDL_TimerID drawingTimer = SDL_AddTimer (update_interval, draw_callback, NULL);

  SDL_Event event;
  do
  {
    if (!SDL_PollEvent (&event))
    {
      SDL_Delay (update_interval);
    }
    else
    {
      if (event.type == SDL_USEREVENT)
      {
          void (*p) (void*) = event.user.data1;
          p (NULL);
      }

      if (mouse_update (&event))
      {
        app_usage ();
        mouse_reset();
      }

    }
  }
  while (!exit_condition(&event));


  SDL_RemoveTimer (drawingTimer);

  app_free ();

  unload ();

  return 0;
}



/*
 *  The clutter space
 */



typedef const char * (*error_source) (void);

void print_error (const char *func_name, error_source func)
{
  printf ("%s: \"%s\"\n", func_name, func ());
}



int init (void)
{

  #define SDL_ERROR(x)  {print_error(x, SDL_GetError); return 0;}

  if (SDL_Init (SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER))
    SDL_ERROR("SDL Init\0")

  if (TTF_Init ())
  {
    print_error ("TTF_Init\0", TTF_GetError);
    return 0;
  }

  src_image = IMG_Load (image_path);
  if (src_image == NULL)
  {
    print_error ("IMG_Load\0", IMG_GetError);
    return 0;
  }

  draw_area.x = BORDER_SIZE;
  draw_area.y = BORDER_SIZE;
  draw_area.w = src_image->w;
  draw_area.h = src_image->h;

  unsigned screenH = draw_area.h + 2*BORDER_SIZE,
      screenW = SIDE_BAR_W + draw_area.w;
  if (screenH < SIDE_BAR_H)
    screenH = SIDE_BAR_H;

  myWindow = SDL_CreateWindow (WINDOW_TITLE,
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      screenW, screenH, 0);



  if (myWindow == NULL)
    SDL_ERROR ("SDL_CreateWindow\0")

  canvas = SDL_GetWindowSurface (myWindow);
  if (canvas == NULL)
    SDL_ERROR ("SDL_GetWindowSurface\0")

  return 1;
}

void drawfunc (void *param)
{ //  update screen image
  app_draw();
  SDL_UpdateWindowSurface (myWindow);
}

SDL_UserEvent _drawEvent = {
  .type = SDL_USEREVENT,
  .code = 0,
  .data1 = &drawfunc
};

static uint32_t draw_callback (uint32_t interval, void *param)
{
  SDL_Event event = {
    .type = SDL_USEREVENT,
    .user = _drawEvent
  };
  SDL_PushEvent (&event);

  return (interval);
}

static int exit_condition (SDL_Event *event)
{
  int quit = 0;
  quit |= (event->type == SDL_QUIT);
  quit |= ((event->type == SDL_KEYDOWN)
      && (event->key.keysym.sym == SDLK_ESCAPE));
  return quit;
}

void unload (void)
{
  SDL_FreeSurface (src_image);
  SDL_DestroyWindow (myWindow);

  TTF_Quit ();
  SDL_Quit ();
}
