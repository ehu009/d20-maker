#include "trekanter.h"

struct equilateral_triangle;
typedef struct equilateral_triangle trekant_t;

struct equilateral_triangle
{
	int x, y;
	int rotation;
	double	radius;
};



trekant_t *make_triangle (double radius)
{
	trekant_t *tri = calloc(sizeof(trekant_t), 1);
	if (tri !=NULL)
	{
		tri->radius = radius;
	}
	return tri;
}

void free_triangle (trekant_t *tri)
{
	free(tri);
}

int get_triangle_rotation(trekant_t *tri)
{
	return tri->rotation;
}

void resize_triangle (trekant_t *tri, double add)
{
	tri->radius += add;
	if (tri->radius < 0)
		tri->radius *= -1;
}

void position_triangle (trekant_t *tri, int x, int y)
{
	tri->x = x;
	tri->y = y;
}

void rotate_triangle (trekant_t *tri, int add)
{
	tri->rotation += add;
}

void draw_triangle (trekant_t *tri, SDL_Surface *surface, plot_func plot, unsigned color)
{

	int rotation = tri->rotation;
	while (rotation < 0)
		rotation += 4;
	rotation %= 4;

	double sides = 3 * tri->radius;
	sides /= sqrt(3);
	sides /= 2;

    int x1, y1,
		x2, y2,
		x3, y3;

    switch(rotation)
    {
		case 0:
			x1 = - (0.5 + sides);	y1 = 0.5 + (tri->radius / 2);
			x2 = 0.5 + sides;		y2 = 0.5 + (tri->radius / 2);
			x3 = 0;		y3 = - (0.5 + tri->radius);
			break;

		case 1:
			x1 = - (0.5 + (tri->radius / 2));	y1 = - (0.5 + sides);
			x2 = - (0.5 + (tri->radius / 2));	y2 = (0.5 + sides);
			x3 = 0.5 + tri->radius;	y3 = 0;
			break;

		case 2:
			x1 = - (0.5 + sides);	y1 = - (0.5 + (tri->radius / 2));
			x2 = 0.5 + sides;		y2 = - (0.5 + (tri->radius / 2));
			x3 = 0;		y3 = (0.5 + tri->radius);
			break;

		case 3:
			x1 = 0.5 + (tri->radius / 2);	y1 = - (0.5 + sides);
			x2 = 0.5 + (tri->radius / 2);	y2 = (0.5 + sides);
			x3 = - (0.5 + tri->radius);	y3 = 0;
			break;
    }

	draw_line (surface, tri->x + x1, tri->y + y1, tri->x + x2, tri->y + y2, plot, color);
	draw_line (surface, tri->x + x2, tri->y + y2, tri->x + x3, tri->y + y3, plot, color);
	draw_line (surface, tri->x + x3, tri->y + y3, tri->x + x1, tri->y + y1, plot, color);

}
