#ifndef   LINES_H_
  #define LINES_H_

  #include "SDL2/SDL.h"

  #include "pixels.h"

  extern SDL_Surface *canvas;

  //  a type for functions that plot a single pixel
  typedef void (*plot_func) (SDL_Surface *, vtx2i_t *, COLOUR);

  void colourPixel (SDL_Surface *dst, vtx2i_t *p, COLOUR color);
  void invertPixel (SDL_Surface *dst, vtx2i_t *p, COLOUR color);


  /*
   *
   */
  void draw_line (SDL_Surface *surface,
      int x1, int y1,
      int x2, int y2,
      plot_func plot,
      COLOUR color);

  void draw_line2 (SDL_Surface *surface,
      vtx2i_t *A, vtx2i_t *B,
      plot_func plot, COLOUR color);


#endif  /*  LINES_H_  */
