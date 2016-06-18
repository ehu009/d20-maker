#include "lines.h"

#pragma once


struct equilateral_triangle;
typedef struct equilateral_triangle trekant_t;


trekant_t *make_triangle (double radius);
void free_triangle (trekant_t *tri);
int get_triangle_rotation(trekant_t *tri);
void resize_triangle (trekant_t *tri, double add);
void position_triangle (trekant_t *tri, int x, int y);
void rotate_triangle (trekant_t *tri, int add);
void draw_triangle (trekant_t *tri, SDL_Surface *surface, plot_func plot, unsigned color);
