#include    <stdlib.h>
#include    <stdio.h>

#include    "SDL2/SDL_image.h"
#include    "SDL2/SDL_ttf.h"

#include "debug.h"
#include "colours.h"
#include "net-builder.h"

#define VERBOSE_CHAR  'v'
#define VERBOSE_DEFAULT 2

unsigned debug = -1;

char *default_path = "default.jpg\0";
char *image_path = NULL;

int init (void);
void unload (void);
const uint32_t draw_interval = 1000/24;
const uint32_t update_interval = 1000/64;

static uint32_t draw_callback (uint32_t interval, void *param);
static uint32_t update_callback (uint32_t interval, void *param);

unsigned alpha_mask;

SDL_Window *myWindow = NULL;
#define WINDOW_TITLE  "Icosahedron maker\0"


SDL_Surface *canvas = NULL,
    *src_image = NULL;

SDL_Rect draw_area;


static int exit_condition (SDL_Event *event);

void parse_args (int n, char *v[])
{
  char verbosity_level = -1;
  #define SET_VERBOSITY_WITH(x)  if (x[1] == VERBOSE_CHAR) verbosity_level = x[2];

  if (n == 2)
  {
    if (v[1][0] == '-')
    {
      image_path = default_path;
      SET_VERBOSITY_WITH(v[1])
    }
    else
      image_path = v[1];
  }
  else
  {
    if (v[1][0] == '-')
    {
      image_path = v[2];
      SET_VERBOSITY_WITH(v[1])
    }
    else if (v[2][0] == '-')
    {
      image_path = v[1];
      SET_VERBOSITY_WITH(v[2])
    }
  }
  if (verbosity_level < 0)
    debug = VERBOSE_DEFAULT;
  else
  {
    verbosity_level -= '0';
    debug = verbosity_level;
  }
}

int main (int argc, char *argv[])
{
  //DEBUG(1,"Starting icosahedron maker.\0")
  if (argc > 1)
  {
    if (argc > 3)
    {
      printf("\t- Too many arguments - exiting.\n");
      return -1;
    }
    else
      parse_args (argc, argv);
  }
  else
    image_path = default_path;


  char str[] = "\t- verbosity was set to 0\0";
  str[strlen(str) - 1] += debug;
  DEBUG(2,str)

  if (!(init ()))
  {
    DEBUG(1,"\t- something went wrong- exiting.\0")
    unload ();
    return 1;
  }

  app_start ();
  DEBUG(2,"\t- initialized application\0")
  SDL_TimerID drawingTimer = SDL_AddTimer (draw_interval, draw_callback, NULL);
  SDL_TimerID updateTimer = SDL_AddTimer (update_interval, update_callback, NULL);
  DEBUG(2,"\t- started drawing and update timers\0")

  DEBUG(VERBOSE_DEFAULT,"Started icosahedron maker.\0")
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

      mouse_update (&event);


    }
  }
  while (!exit_condition(&event));

  DEBUG(1,"Stopping icosahedron maker.\0")
  SDL_RemoveTimer (updateTimer);
  SDL_RemoveTimer (drawingTimer);
  DEBUG(2,"\t- removed timers.\0")

  app_free ();
  DEBUG(2,"\t- deallocated d20 builder structures\0")
  unload ();
  DEBUG(2,"\t- destroyed window\0")

  DEBUG(VERBOSE_DEFAULT, "Exiting icosahedron maker.\0")
  return 0;
}



/*
 *  The clutter space
 */



typedef const char * (*error_source) (void);

void print_error (const char *func_name, error_source func)
{
  printf ("\t- %s: \"%s\"\n", func_name, func ());
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
  DEBUG(2, "\t- started SDL and SDL_ttf\0")

  src_image = IMG_Load (image_path);
  if (src_image == NULL)
  {
    print_error ("IMG_Load\0", IMG_GetError);
    return 0;
  }
  DEBUG(1, "\t- image loaded successfully\0")

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
  alpha_mask = GET_ALPHA_MASK(canvas);

  DEBUG(2, "\t- created a window\0")

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

void updatefunc (void *param)
{
  app_usage ();
  mouse_reset();


}

static uint32_t update_callback (uint32_t interval, void *param)
{
  SDL_UserEvent _updateEvent = {
    .type = SDL_USEREVENT,
    .code = 0,
    .data1 = &updatefunc
  };

  SDL_Event event = {
    .type = SDL_USEREVENT,
    .user = _updateEvent
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
  DEBUG(2,"\t- freed surface and source image.\0")

  TTF_Quit ();
  SDL_Quit ();
  DEBUG(2,"\t- quit SDL and SDL_ttf\0")
}
