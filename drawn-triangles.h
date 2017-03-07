#ifndef   DRAWN_TRIANGLES_H_
  #define   DRAWN_TRIANGLES_H_

  #include "lines.h"


  struct screen_triangle;
  typedef struct screen_triangle triangle_t;

  triangle_t *make_screen_triangle (double radius);
  void free_screen_triangle (triangle_t *t);

  void set_screen_triangle_position (triangle_t *t, int x, int y);
  void get_screen_triangle_position (triangle_t *t, int *x, int *y);

  void rotate_screen_triangle (triangle_t *t, int add);
  int get_screen_triangle_rotation(triangle_t *t);

  void resize_screen_triangle (triangle_t *t, double add);

  void draw_screen_triangle (triangle_t *t, SDL_Surface *surface, plot_func plot, unsigned color);

  void fill_invert_screen_triangle (triangle_t *t, SDL_Surface *surface);


#endif  /*  DRAWN_TRIANGLES_H_  */
