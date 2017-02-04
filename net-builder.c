#include "net-builder.h"




double distance (double x1, double y1, double x2, double y2)
{
  double dx = x2 - x1, dy = y2 - y1;
  dx *= dx;
  dy *= dy;
  return sqrt (dx + dy);
}






#define   TRIANGLES_TOTAL 20
unsigned triangle = 0;

unsigned pins[TRIANGLES_TOTAL];
triangle_t *triangles[TRIANGLES_TOTAL];
triangle_t *work_triangle;

void init_triangles (void)
{
  int i = 0;
  for (;i < TRIANGLES_TOTAL;i++)
  {
    triangles[i] = NULL;
    pins[i] = 0;
  }

}



void redraw (void)
{

  plot_func plot = setInvPixel;
  int i = 0;
  for (;  i < TRIANGLES_TOTAL; i++)
  {
    if (triangles[i] == NULL)
      continue;

    if (i != 0)
      printf("drawing: %d\n",i);

    draw_screen_triangle (triangles[i], canvas, plot, 0xffffffff);
  }
//  draw_triangle (triangles[i], canvas, plot, 0xffffffff);
}


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



void f1 (void)
{

//printf("LINE %d\n",__LINE__);
//  draw_triangle(triangles[0], canvas, setInvPixel, 0xff0000);
redraw ();
/*
printf("LINE %d\n",__LINE__);

  if (!pins[triangle])
  {
//      if (triangles[triangle] == NULL)
  //    triangles[triangle] = make_triangle(60.0);




//  setPixel(canvas, triangles[triangle]->x, triangles[triangle]->y, 0xffffff);

  }

printf("LINE %d\n",__LINE__);
*/
//find_possible_neighbors()


//  SDL_BlitSurface(draw_surface, NULL, canvas, NULL);

//  rotate_triangle(triangles[0], 1);

}

void f2 (void)
{


    /*
    if
      case SDL_MOUSEBUTTONDOWN:*/
        /*
        printf("MOUSE DOWN:\t");
        if (mouseDown)
          mouseRepeat = 1;

        else
          mouseRepeat = 0;
          */

/*
        printf("LINE: %d \n", __LINE__);
        pins[triangle] = 1;
                if (!mouseRepeat)
                {
                    mouseDown = 1;
                    ++ triangle;
                    ++ mouseRepeat;
                }
printf("LINE: %d \n", __LINE__);
*/
      //  printf("mRep: %d\n",mouseRepeat);
  /*
        fflush(stdout);
  */
        /*break;
      case SDL_MOUSEBUTTONUP:*/
        /*
        printf("MOUSE UP:\t");
        if (!mouseDown)
          mouseRepeat = 1;
        else
          mouseRepeat = 0;
        printf("mRep: %d\n",mouseRepeat);
        */
    /*
        if (mouseRepeat)
          mouseRepeat = 0;
        mouseDown = 0;
        */
        /*
        break;

      case SDL_MOUSEMOTION:
      * */
  /*    mouseX = event.motion.x;
        mouseY = event.motion.y;
        printf("LINE: %d \n", __LINE__);

fflush(stdout);
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


void f3 (void)
{
  /*
  init_triangles();
*/


/*  Two lines for testing if the triangles'll still draw properly

 triangles[0] = make_triangle(150);
position_triangle(triangles[0], 100, 40);

 */
}

void f4 (void)
{
  /*

  int j = 0;
  for (;  j < TRIANGLES_TOTAL;  j++)
  {
    if (triangles[j] == NULL)
      continue;
    free_triangle(triangles[j]);
  }
*/
}
