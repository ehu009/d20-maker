#ifndef   MOUSE_H_

  #define MOUSE_H_

  #include  "SDL2/SDL.h"

  typedef unsigned char mouse_data;

  struct mouse_object
  {
    int _x, _y;
    int moving;
    mouse_data buttons;
  };

  typedef struct mouse_object mouse_t;

  mouse_t mouse;
  /*
   *  controls state
   */
  //  call on successfully popping event from queue
  void mouse_update (SDL_Event *e);

  //  call when event queue is empty
  void mouse_reset (void);

#endif  /*  MOUSE_H_  */
