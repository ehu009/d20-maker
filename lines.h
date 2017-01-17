#ifndef   LINES_H_
  #define LINES_H_

  #include "SDL2/SDL.h"

  //  a type for functions that plot a single pixel
  typedef void (*plot_func) (SDL_Surface *, int, int, unsigned);

  void colourPixel (SDL_Surface *dst, int x, int y, unsigned color);
  void invertPixel (SDL_Surface *dst, int x, int y, unsigned color);


  /*
   *
   */
  void draw_line (SDL_Surface *surface,
      int x1, int y1,
      int x2, int y2,
      plot_func plot,
      unsigned int color);

#endif  /*  LINES_H_  */
