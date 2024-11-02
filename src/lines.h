#ifndef   LINES_H_
  #define LINES_H_

  #include "pixels.h"


  extern SDL_Surface *canvas;

  //  a type for functions that plot a single pixel
  typedef void (*plot_func) (SDL_Surface *, vtx2i_t *, COLOUR);

  void colourPixel (SDL_Surface *dst, vtx2i_t *p, COLOUR color);
  void invertPixel (SDL_Surface *dst, vtx2i_t *p, COLOUR color);

  extern double *colour_negate_rate;
  void divertPixel (SDL_Surface *dst, vtx2i_t *p, unsigned color);


  /*
   *
   */
  void draw_line (SDL_Surface *surface,
      vtx2i_t *A, vtx2i_t *B,
      plot_func plot,
      COLOUR color);


  struct line
  {
    vtx2d_t *A, *B;
  };

  int equal_lines (struct line *A, struct line *B);


#endif    /*  LINES_H_  */