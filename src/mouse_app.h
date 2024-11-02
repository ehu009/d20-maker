#ifndef   MOUSE_APP_H_
  #define MOUSE_APP_H_

  #include "mouse.h"


  extern mouse_t mouse;

  /*
   *  Mouse movement prototypes
   */
  int mouse_moves (void);
  void mouse_position (int *x, int *y);

  /*
   *  Mouse button handle prototypes
   */
  int mouse_left  (void);
  int mouse_right (void);
  int mouse_middle  (void);
  int mouse_scroll  (void);


#endif    /*  MOUSE_APP_H_  */