#ifndef   MOUSE_H_

  #define MOUSE_H_

  #include  "SDL2/SDL.h"

  /*
   *  controls state
   */
  //  call on successfully popping event from queue
  void mouse_update (SDL_Event *e);

  //  call when event queue is empty
  void mouse_reset (void);

  typedef unsigned char mouse_data;

#endif  /*  MOUSE_H_  */
