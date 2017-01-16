#include    <stdlib.h>
#include    <stdio.h>



#include    "SDL2/SDL_image.h"
#include    "SDL2/SDL_ttf.h"



#include "mouse.h"

#include "net-builder.h"


int init (void);
void unload (void);
static uint32_t timer_callback(uint32_t interval, void *param);


#define WINDOW_TITLE  "Icosahedron maker\0"
SDL_Window *myWindow = NULL;

SDL_Surface *canvas = NULL,
      *src_image = NULL,
      *draw_surface = NULL;

unsigned screenWidth, screenHeight;

int mouseX, mouseY;
unsigned char mouse_state = 0;






void timerfunc (void *param)
{
  SDL_FillRect(canvas, NULL, 0x0ff);
  SDL_BlitSurface (src_image, NULL, canvas, NULL);

  f1();

  SDL_UpdateWindowSurface (myWindow);
}


int main (int argc, char *arg[])
{
  if (!(init ()))
  {
    unload ();
    printf ("Something went wrong- exiting.\n");
    return 1;
  }

  f3();

  SDL_SetSurfaceBlendMode(draw_surface,SDL_BLENDMODE_ADD);

  SDL_TimerID myTimer = SDL_AddTimer (1000/32, timer_callback, NULL);


  SDL_Event event;
  do
  {
    if (!SDL_PollEvent (&event))
    {

      mouse_reset();
    }
    else
    {


    if (event.type == SDL_USEREVENT)
    {   /*  User defined timed events */
        void (*p) (void*) = event.user.data1;
        p(event.user.data2);
    }

    mouse_update(&event);

    f2();

    }
  }
  while (event.type != SDL_QUIT);

  SDL_RemoveTimer(myTimer);


  f4();

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

  if (TTF_Init())
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


  screenWidth = src_image->w;
  screenHeight = src_image->h;
  myWindow = SDL_CreateWindow (WINDOW_TITLE,
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      src_image->w, src_image->h,
      0);



  if (myWindow == NULL)
    SDL_ERROR("SDL_CreateWindow\0")

  canvas = SDL_GetWindowSurface (myWindow);
  if (canvas == NULL)
    SDL_ERROR("SDL_GetWindowSurface\0")


  draw_surface = SDL_CreateRGBSurface (0, canvas->w, canvas->h, 32,
                    0, 0, 0, 0);
  if (draw_surface == NULL)
    SDL_ERROR("SDL_CreateRGBSurface\0")

  return 1;
}


static uint32_t timer_callback(uint32_t interval, void *param)
{
  SDL_Event event;
  SDL_UserEvent userevent;

  userevent.type = SDL_USEREVENT;
  userevent.code = 0;
  userevent.data1 = &timerfunc;
  userevent.data2 = NULL;

  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
  return (interval);
}


void unload (void)
{


  SDL_FreeSurface (draw_surface);

  SDL_FreeSurface (canvas);
  SDL_DestroyWindow (myWindow);

  TTF_Quit ();
  SDL_Quit ();
}
