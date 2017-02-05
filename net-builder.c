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

  triangle_t *positions[3];
  unsigned pinned;
  double x, y;

  unsigned visited;
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
      init_slots(ptr);
    else
    {
      free(ptr);
      ptr = NULL;
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
        printf("\tfreeing #%d\n",j);
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


char toggle = 0;

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

void f2 ()
{
  if (d20->num_pinned == 0)
  {
    set_screen_triangle_position (d20->work_triangle, mouse._x, mouse._y);


    if(mouse_right(&mouse) == 1)
    {
      toggle ^= 1;
    }

    int scroll = mouse_scroll(&mouse);

    if (toggle)
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
      d20->slots[0].x = mouse._x;
      d20->slots[0].y = mouse._y;
      d20->slots[0].positions[0] = d20->work_triangle;
      d20->work_triangle = NULL;
      d20->slots[0].pinned = 1;
      d20->num_pinned += 1;

    }


  }
  else
  {


/*
                if (triangle > 1)
                {

                    //  find nearest triangle
                    int i = 0;
                    double shortest = distance (mouseX,mouseY, triangles[i]->x,triangles[i]->y);
                    printf("LINE: %d \n", __LINE__);
                    trekant_t *nearest = triangles[i];
                    printf("LINE: %d \n", __LINE__);


                    if (shortest >= nearest->radius)
                    {

  fflush(stdout);
            for ( ++i   ; i < triangle; i++)
            {
              if (!pins[i])
              {
                printf("i broke out\n");
                break;
              }

              double dist = distance (mouseX,mouseY, triangles[i]->x,triangles[i]->y);
              if (dist < shortest)
              {
                nearest = triangles[i];
                shortest = dist;
              }
            }

*/                      //  position_triangle(triangles[triangle],mouseX, mouseY);

/*
                        draw_line(canvas, nearest->x,nearest->y,
                                    mouseX, mouseY,
                                    setInvPixel, 0);
*/
  /*                  }

                printf("LINE: %d \n", __LINE__);
          if (nearest != NULL)
            find_possible_neighbors(nearest, NULL,NULL,NULL);
        printf("LINE: %d \n", __LINE__);

                }

            if (triangles[triangle] != NULL)
                position_triangle(triangles[triangle],mouseX,mouseY);
            else
                printf("LINE: %d \n", __LINE__);
printf("LINE: %d \n", __LINE__);

*/
/*
        break;
    }
*/
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
