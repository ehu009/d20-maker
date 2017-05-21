#include    <stdlib.h>
#include    <stdio.h>

#include    "SDL2/SDL_image.h"
#include    "SDL2/SDL_ttf.h"

#include "net-builder.h"


int init (void);
void unload (void);
static uint32_t timer_callback (uint32_t interval, void *param);

#define SIDE_BAR_W  400
#define SIDE_BAR_H  400

SDL_Window *myWindow = NULL;
#define WINDOW_TITLE  "Icosahedron maker\0"


SDL_Surface *canvas = NULL,
    *src_image = NULL;

unsigned canvasW, canvasH;


int exit_condition (SDL_Event *event)
{
  int quit = 0;
  quit |= (event->type == SDL_QUIT);
  quit |= ((event->type == SDL_KEYDOWN) && (event->key.keysym.sym == SDLK_ESCAPE));
  return quit;
}



int main (int argc, char *arg[])
{
  if (!(init ()))
  {
    printf ("Something went wrong- exiting.\n");
    unload ();
    return 1;
  }

  app_start ();

  SDL_TimerID myTimer = SDL_AddTimer (1000/32, timer_callback, NULL);

  SDL_Event event;
  do
  {
    if (!SDL_PollEvent (&event))
    {

      mouse_reset ();
    }
    else
    {
      if (event.type == SDL_USEREVENT)
      {   /*  User defined timed events */
          void (*p) (void*) = event.user.data1;
          p (event.user.data2);
      }

      mouse_update (&event);

      app_usage ();
    }
  }
  while (!exit_condition(&event));

  SDL_RemoveTimer (myTimer);


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

  if (SDL_Init (SDL_INIT_EVENTS | SDL_INIT_VIDEO))
    SDL_ERROR("SDL Init\0")

  if (TTF_Init ())
  {
    print_error ("TTF_Init\0", TTF_GetError);
    return 0;
  }

  src_image = IMG_Load ("image.jpg");
  if (src_image == NULL)
  {
    print_error ("IMG_Load\0", IMG_GetError);
    return 0;
  }

  canvasW = src_image->w;
  canvasH = src_image->h;

  unsigned screenH = canvasH,
      screenW = SIDE_BAR_W + canvasW;
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

void timerfunc (void *param)
{ //  update screen image
  void (*function) (void) = param;
  function ();
  SDL_UpdateWindowSurface (myWindow);
}

static uint32_t timer_callback (uint32_t interval, void *param)
{
  SDL_Event event;
  SDL_UserEvent userevent;

  userevent.type = SDL_USEREVENT;
  userevent.code = 0;
  userevent.data1 = &timerfunc;
  userevent.data2 = &app_draw;

  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent (&event);
  return (interval);
}


void unload (void)
{
  SDL_FreeSurface (src_image);
  SDL_DestroyWindow (myWindow);

  TTF_Quit ();
  SDL_Quit ();
}
