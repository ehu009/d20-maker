#ifndef   MOUSE_APP_H_
  #define MOUSE_APP_H_

  #include "mouse.h"

  struct mouse_object
  {
    int _x, _y;
    mouse_data buttons;
  };

  typedef struct mouse_object mouse_t;



  extern mouse_t mouse;

  int mouse_left  (mouse_t *m);
  int mouse_right (mouse_t *m);
  int mouse_middle  (mouse_t *m);
  int mouse_scroll  (mouse_t *m);

#endif  /*  MOUSE_APP_H_  */
