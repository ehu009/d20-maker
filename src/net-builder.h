#ifndef   NET_BUILDER_H_
  #define NET_BUILDER_H_

  #include <stdio.h>

  #include "lines.h"
  #include "mouse_app.h"


  #define BORDER_SIZE   7
  
  extern SDL_Surface *canvas, *src_image;

  #define FADER_WIDTH   30
  #define BUTTON_HEIGHT 24
  #define BUTTON_WIDTH  45

  #define SIDE_BAR_W  (BUTTON_WIDTH + 3*BORDER_SIZE)
  #define SIDE_BAR_H  2*BORDER_SIZE


  void app_start (void);
  void app_free (void);

  void app_usage (void);
  void app_draw (void);


#endif    /*  NET_BUILDER_H_  */