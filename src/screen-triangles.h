#ifndef   SCREEN_TRIANGLE_H_

  #define SCREEN_TRIANGLE_H_


  #include    "lines.h"


  typedef struct screen_triangle
  {
    vtx2d_t *pts[3];
  } triangle_t;


  void draw_triangle (triangle_t *t, plot_func plot, COLOR color);

  void fill_triangle (triangle_t *t, plot_func plot, COLOR color);

  int triangle_contains (triangle_t *t, vtx2i_t point);

void transfer_triangle (triangle_t *t, SDL_Surface *dst, SDL_Rect *dst_rect);

#endif  /*  SCREEN_TRIANGLE_H_  */
