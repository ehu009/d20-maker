#ifndef   SCREEN_TRIANGLE_H_

  #define SCREEN_TRIANGLE_H_


  #include    "lines.h"


  typedef struct screen_triangle
  {
    vtx2d_t *pts[3];
  } triangle_t;

  triangle_t *copy_triangle(triangle_t *t);

  int triangle_contains (triangle_t *t, vtx2i_t point);


  void draw_triangle (triangle_t *t, plot_func plot, COLOUR color);

  void fill_triangle (triangle_t *t, plot_func plot, COLOUR color);

  void transfer_triangle (triangle_t *t, SDL_Surface *dst, SDL_Rect *dst_rect);

#endif  /*  SCREEN_TRIANGLE_H_  */
