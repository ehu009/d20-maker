#ifndef   MOUSE_H_

  #define MOUSE_H_

  #include  "SDL2/SDL.h"

  /*
   *  Keeping track of mouse input
   */
  typedef unsigned char mouse_data;

  typedef struct mouse_object
  {
    int _x, _y;
    int moving;
    mouse_data buttons;
  } mouse_t;

  extern mouse_t mouse;

  /*
   *  Mouse state maintenance prototypes
   */
  int mouse_update (SDL_Event *e);

  void mouse_reset (void);

#endif  /*  MOUSE_H_  */
