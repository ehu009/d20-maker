#ifndef   MOUSE_H_

  #define MOUSE_H_

  #include  "SDL2/SDL.h"


  //  not to be touched externally
  extern int mouseX, mouseY;
  extern unsigned char mouse_state;

  //  checks mouse state
  int mouse_left_button (void);
  int mouse_right_button (void);
  int mouse_middle_button (void);
  int mouse_is_scrolled (void);

  /*
   *  controls state
   */

  //  call on successfully popping event from queue
  void mouse_update (SDL_Event *e);

  //  call when event queue is empty
  void mouse_reset (void);

#endif  /*  MOUSE_H_  */
