#ifndef   SCREEN_TRIANGLE_H_

  #define SCREEN_TRIANGLE_H_


  #include    "lines.h"


  SDL_Rect *get_bounds_of_triangle (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c);

  void draw_triangle (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c, plot_func plot, COLOR color);

  void fill_triangle (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c, plot_func plot, COLOR color);

  int triangle_contains (vtx2d_t *a, vtx2d_t *b, vtx2d_t *c, vtx2i_t point);


#endif  /*  SCREEN_TRIANGLE_H_  */
