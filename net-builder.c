#include "net-builder.h"


#include    <math.h>


double distance (double x1, double y1, double x2, double y2)
{
  double dx = x2 - x1, dy = y2 - y1;
  dx *= dx;
  dy *= dy;
  return sqrt (dx + dy);
}



/*
 *  assigns slots in a net to triangles
 */

struct slot
{
  unsigned pinned;
  double x, y;
  //  possible screen locations
  triangle_t *positions[3];
  //  neighboring triangles
  struct slot *A, *B, *C;

  unsigned visited; //  might not need this one
};


#define   TRIANGLES_TOTAL 20

struct net_builder
{

  unsigned num_pinned;
  int rotation;
  double radius;

  triangle_t *work_triangle;

  struct slot slots[TRIANGLES_TOTAL];
};

typedef struct net_builder net_t;


/*
 *  neighbor relations between triangles
 */
//  five bits needed to count to 20
#define MASK 0b011111
//  so we need a short for 3 numbers, each up to 20
#define NET_MODEL_TYPE short
//  shift amounts for each number
enum {SHAMT_A=10, SHAMT_B=5, SHAMT_C=0};

#define NEIGHBOR_A(x) (((MASK << SHAMT_A) & x) >> SHAMT_A)
#define NEIGHBOR_B(x) (((MASK << SHAMT_B) & x) >> SHAMT_B)
#define NEIGHBOR_C(x) (((MASK << SHAMT_C) & x) >> SHAMT_C)

#define NET_MODEL_ENTRY(a,b,c)  (NET_MODEL_TYPE) 0 + (a<<SHAMT_A) + (b<<SHAMT_B) + (c<<SHAMT_C)

NET_MODEL_TYPE net_model [20] =
{
  NET_MODEL_ENTRY(3,2,4),
  NET_MODEL_ENTRY(1,6,5),
  NET_MODEL_ENTRY(1,8,7),
  NET_MODEL_ENTRY(1,10,9),
  NET_MODEL_ENTRY(2,11,10),
  NET_MODEL_ENTRY(2,7,12),
  NET_MODEL_ENTRY(3,13,6),
  NET_MODEL_ENTRY(3,9,14),
  NET_MODEL_ENTRY(4,15,8),
  NET_MODEL_ENTRY(4,5,16),
  NET_MODEL_ENTRY(5,12,17),
  NET_MODEL_ENTRY(6,18,11),
  NET_MODEL_ENTRY(7,14,18),
  NET_MODEL_ENTRY(8,19,13),
  NET_MODEL_ENTRY(9,16,19),
  NET_MODEL_ENTRY(10,17,15),
  NET_MODEL_ENTRY(11,20,16),
  NET_MODEL_ENTRY(12,13,20),
  NET_MODEL_ENTRY(14,15,20),
  NET_MODEL_ENTRY(17,18,19)
};


void init_slots (net_t *n)
{
  struct slot *s;
  int i = 0;
  for (;i < TRIANGLES_TOTAL;i++)
  {
    s = &n->slots[i];

    int j = 3;
    while (j)
    {
      s->positions[j-1] = NULL;
      j--;
    }
    s->pinned = 0;
    s->x = 0.0;
    s->y = 0.0;

    short neighbors = net_model[i];
    s->A = &n->slots[NEIGHBOR_A(neighbors)];
    s->B = &n->slots[NEIGHBOR_B(neighbors)];
    s->C = &n->slots[NEIGHBOR_C(neighbors)];
  }

}

net_t *make_net_builder(double start_radius)
{
  net_t *ptr = (net_t *) calloc(1, sizeof(net_t));

  int err = 0;
  err |= (ptr == NULL);
  if(!err)
  {
    ptr->radius = start_radius;
    ptr->work_triangle = make_screen_triangle(start_radius);
    err |= (ptr->work_triangle == NULL);
    if (!err)
    {
      init_slots(ptr);
    }
    else
    {
      if (ptr != NULL)
      {
        free(ptr);
        ptr = NULL;
      }
    }
  }
  return ptr;
}


void free_screen_triangles (net_t *n)
{
  struct slot *s;
  triangle_t **t;
  int i = 0;
  for (;i < TRIANGLES_TOTAL;i++)
  {
    s = &n->slots[i];
    int j = 3;
    for(; j; j--)
    {
      t = &s->positions[j-1];
      if (*t != NULL)
      {
        free(*t);
        *t = NULL;
      }
    }
  }
}

void free_net_builder(net_t *n)
{
  free_screen_triangle(n->work_triangle);
  free_screen_triangles(n);
  free(n);
}


char rotate_root_toggler = 1;

net_t *d20 = NULL;



/*

void  find_possible_neighbors (trekant_t *tri, trekant_t *n1, trekant_t *n2, trekant_t *n3)
{
  int x1, x2, x3, y1, y2, y3;
  x1 = tri->x;
  x2 = tri->x;
  x3 = tri->x;

    y1 = tri->y;
  y2 = tri->y;
  y3 = tri->y;

//  trekant_t *q1, *q2, *q3;

  int start;
  int rot = tri->rotation % 4;
  double pi = M_PI, r = tri->radius;

  if ( rot == 0 )
  {
    start = 30;
  }
  else if ( rot == 2 )
  {
    start = 210;
  }
  if ( !( rot % 2 ) )
  {
    x3 += r * cos ( ( double )  ( start + 120 * 2 ) * ( pi / 180 ) );
    x2 += r * cos ( ( double )  ( start + 120 * 1 ) * ( pi / 180 ) );
    x1 += r * cos ( ( double )  ( start + 120 * 0 ) * ( pi / 180 ) );

    y3 -= r * sin ( ( double )  ( ( start + 120 * 2 ) * ( pi / 180 ) ) );
    y2 -= r * sin ( ( double )  ( ( start + 120 * 1 ) * ( pi / 180 ) ) );
    y1 -= r * sin ( ( double )  ( ( start + 120 * 0 ) * ( pi / 180 ) ) );
  }

#define NEW_TRIANGL     0


  void draw_fake_triangle ( unsigned x, unsigned y, double r )
  {
    draw_line ( canvas, x, y - r,
                  x, y,   setInvPixel, 0 );
    draw_line ( canvas, x - r, y,
                  x, y,   setInvPixel, 0 );
  }


  int rect_border (int b_x, int b_y, double x, double y,double w, double h)
  {
        return  ( int ) (
              ( x > ( b_x - w ) )
        |  ( x < w )
        |  ( y > ( b_y - h ) )
        |  ( y < h ) );
  }


  if ( !rect_border ( screenWidth,screenHeight, x1,y1, r, r ) )
    {
draw_fake_triangle ( x1, y1, r );


#if NEW_TRIANGL
        if (n1 != NULL)
        {
           make_triangle (tri->radius);

        }
#endif
    }
  if ( !rect_border ( screenWidth,screenHeight, x2,y2, r, r ) )

    {
        draw_line ( canvas, x2, y2 - r,
                                    x2, y2,   setInvPixel, 0 );
        draw_line ( canvas, x2 - r, y2,
                                    x2, y2,   setInvPixel, 0 );

#if NEW_TRIANGL
        if (n2 != NULL)
        {
           make_triangle (tri->radius);

        }
#endif
    }
  if ( !rect_border ( screenWidth,screenHeight, x3,x3, r, r ) )

    {
        draw_line ( canvas, x3, y3 - r,
                                    x3, y3,   setInvPixel, 0 );
        draw_line ( canvas, x3 - r, y3,
                                    x3, y3,   setInvPixel, 0 );

#if NEW_TRIANGL
        if (n3 != NULL)
        {
           make_triangle (tri->radius);

        }
#endif
    }

}
*/



void draw_pinned_slots(net_t *n)
{
  int i = 0;
  for(; i < TRIANGLES_TOTAL; i++)
  {
    if (d20->slots[i].pinned == 0)
      continue;
    int j = 3;
    triangle_t **ptr = d20->slots[i].positions;
    while(j)
    {
      if (*ptr != NULL)
        draw_screen_triangle(*ptr, canvas, invertPixel, 0);
      ptr ++;
      j --;
    }
  }
}

void f1 (void)
{
  if (d20->work_triangle != NULL)
  {
    draw_screen_triangle(d20->work_triangle, canvas, invertPixel, 0);
  }

  if (d20->num_pinned)
  {
    draw_pinned_slots(d20);

  }

}




int rotate_root_triangle(void)
{ //  toggle rotation and resizing of root triangle
  if(mouse_right(&mouse) == 1)
  {
    rotate_root_toggler ^= 1;
  }
  return rotate_root_toggler;
}

void pin_root_triangle(net_t *n)
{
  n->slots[0].x = mouse._x;
  n->slots[0].y = mouse._y;

  //  advance the structure
  //  ..

  n->slots[0].positions[0] = n->work_triangle;
  n->work_triangle = NULL;

  n->slots[0].pinned = 1;
  n->num_pinned += 1;

  //  ..
}

void unpin_root_triangle(net_t *n)
{
  free_screen_triangles(n);
  init_slots(n);

  n->num_pinned = 0;
  n->work_triangle = make_screen_triangle(n->radius);

  rotate_screen_triangle(n->work_triangle, n->rotation);
  set_screen_triangle_position(n->work_triangle,mouse._x, mouse._y);
}

void f2 ()
{
  if (d20->num_pinned == 0)
  { //  root triangle is not pinned
    set_screen_triangle_position (d20->work_triangle, mouse._x, mouse._y);

    int scroll = mouse_scroll(&mouse);

    if (rotate_root_triangle())
    {
      d20->radius += scroll;
      resize_screen_triangle (d20->work_triangle, scroll);
    }
    else
    {
      d20->rotation += scroll;
      rotate_screen_triangle (d20->work_triangle, scroll);
    }

    if (mouse_left(&mouse) == -1)
    {
      pin_root_triangle(d20);
    }
  }
  else
  {
    if (mouse_left(&mouse) == -1)
    { //  doing
      printf("click\n");
      printf("we have %d triangles pinned\n", d20->num_pinned);


    }

    if (mouse_right(&mouse) == -1)
    { //  undoing
      if (d20->num_pinned == 1)
      {
        unpin_root_triangle(d20);
      }
      else
      {
        printf("undoing for more than just the root triangle has not been implemented yet\n");
      }
    }

    //  ..

    //  ..

    //  ..


  }
}


void f3 (void)
{
  d20 = make_net_builder(35.0);

}

void f4 (void)
{
  free_net_builder(d20);

/*
  int j = 0;
  for (;  j < TRIANGLES_TOTAL;  j++)
  {
    if (triangles[j] == NULL)
      continue;
    free_triangle(triangles[j]);
  }
  * */
}
