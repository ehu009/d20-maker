#ifndef   LINES_H_
  #define LINES_H_

  #include "SDL2/SDL.h"

  #include "pixels.h"

  extern SDL_Surface *canvas;

  //  a type for functions that plot a single pixel
  typedef void (*plot_func) (SDL_Surface *, vtx2_t *, COLOR);

  void colourPixel (SDL_Surface *dst, vtx2_t *p, COLOR color);
  void invertPixel (SDL_Surface *dst, vtx2_t *p, COLOR color);


  /*
   *
   */
  void draw_line (SDL_Surface *surface,
      int x1, int y1,
      int x2, int y2,
      plot_func plot,
      COLOR color);

  void draw_line2 (SDL_Surface *surface,
      vtx2_t *A, vtx2_t *B,
      plot_func plot, COLOR color);


#endif  /*  LINES_H_  */
