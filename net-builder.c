#include "net-builder.h"

#include    <math.h>


double distance (double x1, double y1, double x2, double y2)
{
  double dx = x2 - x1, dy = y2 - y1;
  dx *= dx;
  dy *= dy;
  return sqrt (dx + dy);
}



#include "intstack.h"

#include "chain.h"

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

int slot_has_triangle(struct slot *slot, triangle_t *t)
{
  return (slot->positions[0] == t)
      ||  (slot->positions[1] == t)
      ||  (slot->positions[2] == t);
}


#define   TRIANGLES_TOTAL 20

struct net_builder
{
  int rotation;
  double radius;

  triangle_t *work_triangle;

  chain_t *edge;
  chainslider_t *selector;
  intstack_t *pinned;

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

#define NEIGHBOR_A(x) (-1 + (((MASK << SHAMT_A) & x) >> SHAMT_A))
#define NEIGHBOR_B(x) (-1 + (((MASK << SHAMT_B) & x) >> SHAMT_B))
#define NEIGHBOR_C(x) (-1 + (((MASK << SHAMT_C) & x) >> SHAMT_C))

#define NET_MODEL_ENTRY(a,b,c)  (NET_MODEL_TYPE) 0 + (a<<SHAMT_A) + (b<<SHAMT_B) + (c<<SHAMT_C)

NET_MODEL_TYPE net_model [TRIANGLES_TOTAL] =
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

  intstack_t *s;
  if(!err)
  {
    s = make_integer_stack(TRIANGLES_TOTAL);
    err |= (s == NULL);
  }
  if (!err)
  {
    ptr->work_triangle = make_screen_triangle(start_radius);
    err |= (ptr->work_triangle == NULL);
  }

  if (!err)
  {
    init_slots(ptr);
    ptr->pinned = s;
    ptr->radius = start_radius;
  }
  else
  {
    if (s != NULL)
    {
      free (s);
    }

    if (ptr != NULL)
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
        free(*t);
        *t = NULL;
      }
    }
  }
}

void free_net_builder(net_t *n)
{
  if (n->work_triangle == NULL)
    free_screen_triangle(n->work_triangle);
  free_screen_triangles(n);
  if (n->selector != NULL)
    free_chainslider(n->selector);
  if (n->edge)
    free_chain(n->edge);
  free_stack(n->pinned);
  free(n);
}

struct application
{
  //  togglers
  char rotate_root;
  char use_slider;
  //  other things

} app;
/*
struct application app = {
  .rotate_root = 1,
  .use_slider = 1
};*/
net_t *d20 = NULL;




int rotate_root_triangle(void)
{ //  toggle between rotation and resizing of root triangle
  if(mouse_middle() == -1)
  {
    app.rotate_root ^= 1;
  }
  return app.rotate_root;
}

int slide_selector(void)
{ //  toggle between using mouse wheel or position for selections
  if(mouse_middle() == -1)
  {
    app.use_slider ^= 1;
  }
  return app.use_slider;
}





void draw_slots(net_t *n, SDL_Surface *surface)
{ //  draw slots for a net onto a surface
  if (surface == NULL)
    return;

  struct slot *sptr;
  triangle_t **tptr;
  triangle_t *m;
  int i = 0;
  for(; i < TRIANGLES_TOTAL; i++)
  { //  look for triangles in all slots
    sptr = &n->slots[i];
    tptr = sptr->positions;
    m = tptr[0];
    if (sptr->pinned)
    {
      //  pinned slots
      draw_screen_triangle (m, surface, invertPixel, 0);
      continue;
    }
    //  unpinned ones
    if ((m != NULL) && (m != n->work_triangle))
    {
      fill_invert_screen_triangle (m, surface);
    }
    int k = 1;
    for (; k <3; k ++)
    {
      m = tptr[k];
      if (m == NULL)
        //  skip blanks
        continue;
      if (m == n->work_triangle)
        //  skip work triangle
        continue;
      fill_invert_screen_triangle (m, surface);
    }
  }
}


void draw_net (net_t *n, SDL_Surface *surface)
{
  if (surface == NULL)
    return;


  if (stack_height (n->pinned))
  {
    draw_slots (n, surface);
    draw_screen_triangle (n->work_triangle, surface, colourPixel, 0x00ff00);
  }
  else
  {
    fill_invert_screen_triangle (n->work_triangle, surface);
  }

}


void f1 (void)
{
  draw_net (d20, canvas);
}



void pin_root_triangle(net_t *n)
{
  int x, y;
  mouse_position(&x, &y);

  n->slots[0].x = x;
  n->slots[0].y = y;
  n->rotation = get_screen_triangle_rotation (n->work_triangle);
  n->slots[0].positions[0] = n->work_triangle;
  n->work_triangle = NULL;

  n->slots[0].pinned = 1;

  char err = 0;

  stack_push(n->pinned, &err, 0);

  struct slot *sA, *sB, *sC;

  triangle_t *tA, *tB, *tC;
  if (!err)
  {

    tA = make_screen_triangle(n->radius);
    tB = make_screen_triangle(n->radius);
    tC = make_screen_triangle(n->radius);
    err |= (tA == NULL);
    err |= (tB == NULL);
    err |= (tC == NULL);
  }

  if (!err)
  {
    int rot = n->rotation + 2;


    rotate_screen_triangle (tA, rot);
    rotate_screen_triangle (tB, rot);
    rotate_screen_triangle (tC, rot);
    double a_x, a_y;
    double b_x, b_y;
    double c_x, c_y;

    double thirty = 30, sixty = 60;
    thirty*= M_PI/180;
    sixty*= M_PI/180;



    int R = get_screen_triangle_rotation (tA);
    switch (R)
    {
      case 0:
        a_x = -cos (thirty);
        a_y = sin (thirty);
        b_x = cos (thirty);
        b_y = sin (thirty);
        c_x = 0;
        c_y = -1;
        break;
      case 1:
        a_x = -cos(sixty);
        a_y = sin(sixty);
        b_x = 1;
        b_y = 0;
        c_x = -cos (sixty);
        c_y = -sin (sixty);
        break;
      case 2:
        a_x = -cos (thirty);
        a_y = -sin (thirty);
        b_x = 0;
        b_y = 1;
        c_x = cos (thirty);
        c_y = -sin (thirty);
        break;
      case 3:
        a_x = -1;
        a_y = 0;
        b_x = cos (sixty);
        b_y = sin (sixty);
        c_x = cos (sixty);
        c_y = -sin (sixty);
        break;
      default:
      break;
    }
    double D = 2*cos(sixty)*n->radius;
    a_x *= D; a_y *= D;
    b_x *= D; b_y *= D;
    c_x *= D; c_y *= D;

    int r_x = n->slots[0].x;
    int r_y = n->slots[0].y;

    a_x += r_x; b_x += r_x; c_x += r_x;
    a_y += r_y; b_y += r_y; c_y += r_y;

    set_screen_triangle_position (tA, (int)a_x + 0.5, (int)a_y + 0.5);
    set_screen_triangle_position (tB, (int)b_x + 0.5, (int)b_y + 0.5);
    set_screen_triangle_position (tC, (int)c_x + 0.5, (int)c_y + 0.5);

    sA = n->slots[0].A;
    sA->positions[0] = tA;

    sB = n->slots[0].B;
    sB->positions[0] = tB;

    sC = n->slots[0].C;
    sC->positions[0] = tC;
  }
    //  need to set coordinates on slots
  if (!err)
  {
    if (n->selector != NULL)
    {
      free_chainslider (n->selector);
    }
    if (n->edge != NULL)
    {
      free_chain(n->edge);
    }
    n->edge = make_chain ((void *) tA);
    err |= (n->edge == NULL);
  }
  if (!err)
  {

    n->selector = make_chainslider(n->edge);
    err |= (n->selector == NULL);
  }
  if (!err)
  {

    slider_insert_after(n->selector, (void *) tC);
    slider_insert_after(n->selector, (void *) tB);

    n->work_triangle = slider_current(n->selector);

  }
}




void unpin_root_triangle(net_t *n)
{
  free_screen_triangles(n);
  free_chainslider(n->selector);
  n->selector = NULL;
  free_chain(n->edge);
  n->edge = NULL;

  init_slots (n);

  char err = 0;
  stack_pop (n->pinned,&err);

  n->work_triangle = make_screen_triangle(n->radius);

  rotate_screen_triangle (n->work_triangle, n->rotation);

  int x, y;
  mouse_position (&x, &y);
  set_screen_triangle_position (n->work_triangle, x, y);
}

/*
 *
 * dev place
 *
 */



triangle_t *find_triangle_nearest_to(int x, int y)
{
  int k = chain_size(d20->edge);
  triangle_t *current = (triangle_t *)slider_current(d20->selector),
      *nearest = current;

  int tX, tY;
  get_screen_triangle_position(current, &tX, &tY);
  double shortest = distance(x, y, tX, tY);

  while (k)
  {
    slider_procede(d20->selector);
    current = slider_current(d20->selector);
    get_screen_triangle_position(current, &tX, &tY);
    double length = distance (x, y, tX, tY);
    if (length < shortest)
      nearest = current;
    -- k;
  }
  return nearest;
}

struct slot *get_neighbor_with_triangle (struct slot *s, triangle_t *t)
{
  if (slot_has_triangle (s->A, t))
    return s->A;
  else if (slot_has_triangle (s->B, t))
    return s->B;
  else if (slot_has_triangle (s->C, t))
    return s->C;
  else return NULL;
}

struct slot *find_nearest_neighbor (struct slot **network, int n, triangle_t *position)
{
  if (n > 0)
    return network[0];
  if (n > 1)
  {
    int tx, ty;
    get_screen_triangle_position(position, &tx, &ty);

    struct slot *nearest = network[0];
    int ax, ay;
    int bx, by;
    get_screen_triangle_position(network[0]->positions[0], &ax, &ay);
    get_screen_triangle_position(network[1]->positions[0], &bx, &by);

    double shortest = distance(tx, ty, ax, ay),
        dist = distance(tx, ty, bx, by);

    if (dist < shortest)
    {
      nearest = network[1];
      shortest = dist;
    }
    if (n > 2)
    {
      int ax, ay;
      int bx, by;
      get_screen_triangle_position(network[1]->positions[0], &ax, &ay);
      get_screen_triangle_position(network[2]->positions[0], &bx, &by);
      dist = distance(tx, ty, bx, by);
      if (dist < shortest)
        nearest = network[2];
    }
    return nearest;
  }

  printf("rumpelukt\n");
  return NULL;
}



int approximates (double test, double source, double accuracy)
{
  return (source + accuracy >= test)
      && (test >= source - accuracy);
}
#define FLOAT_ACC 2



triangle_t *slot_has_triangle_near (struct slot *slot, double x, double y)
{
  int t=0;
  triangle_t *tr = NULL;
  for(; t < 3; t ++)
  {
    int tx, ty;
    tr = slot->positions[t];
    if (tr == NULL)
      continue;
    get_screen_triangle_position (tr, &tx, &ty);
    if (approximates(tx, x, FLOAT_ACC+1) && approximates(ty, y, FLOAT_ACC+1))
    {
      return tr;
    }
  }
  return NULL;
}



void find_possible_anchors (triangle_t *t, net_t *n, struct slot **sbuf, int *nbuf)
{
  char error = 0;
printf("looking for neighbors\n");
  intstack_t *tmp_stack = make_integer_stack (TRIANGLES_TOTAL);
  if (tmp_stack == NULL)
    error ++;

  int n_idx = 0;
  *nbuf = -1;

  if (!error)
  { //  pop all
    while (stack_height (n->pinned))
    {
      int popped = stack_pop (n->pinned,&error);
      if (!error)
      {
        stack_push (tmp_stack, &error, popped);
      }
      else { printf ("error :C %d\n",__LINE__);}
      if (!error)
      {
      printf("trying idx:%d,",popped);
        //  check if slot has available neighbors
        // check if available neighbor matches screen triangle
        struct slot *slot = &n->slots[popped];
       /*
        int has_neighbor = 0
            ||  (!slot->A->pinned
                ||  !slot->B->pinned
                ||  !slot->C->pinned);
        if (has_neighbor)
        {
        */
          if (slot_has_triangle (slot->A, t)
              ||  slot_has_triangle (slot->B, t)
              ||  slot_has_triangle (slot->C, t))
          {
            if(!error)
            {
              sbuf[n_idx] = slot;
              if (n_idx == 0)
                printf("\t\t");
              printf("found neighbor");
              if (n_idx < 3-1)
                printf(", ");
              ++ n_idx;
            }
            else { printf ("error :C %d\n",__LINE__);}
          }
      //  }
      }
      else { printf ("error :C %d\n",__LINE__);}
    }
  }
  if (!error)
  { //  push back

    while (stack_height (tmp_stack))
    {
      int popped = stack_pop (tmp_stack,&error);
      if (!error)
      {
        stack_push (n->pinned, &error, popped);
      }
      else { printf ("error :C %d\n",__LINE__);}
    }
  }
  else
  {
    printf ("error :CCC\n");
  }
  if (!error)
    *nbuf = n_idx + 1;
  else
    printf("i dont know what went wrong when finding the anchor yet\n");
  printf("\n");
  fflush(stdout);
}



double triangle_modulation_in_radians (int triangle_rotation)
{
  double modulation = 0.0;
  switch (triangle_rotation)
  {
    case 0:
      printf("rotation 0\n");
      modulation += 90;
    break;
    case 1:
      printf("rotation 1\n");
      modulation += 30;
    break;
    case 2:
      printf("rotation 2\n");
      modulation += 60;
    break;
    case 3:
      printf("rotation 3\n");
//            modulation += 0;
    break;
  }
  modulation *= M_PI;
  modulation /= 180;
  return modulation;
}



int pidgeon_policy (net_t *n, double x, double y)
{
  //  see if net contains a triangle at a position near
  int exists = 0, i = 0;
  struct slot *s = n->slots;
  for (; i < TRIANGLES_TOTAL; i ++)
  {
    exists |= (slot_has_triangle_near (s, x, y) != NULL);
    if (exists)
      break;
    s ++;
  }
  return exists;
}

int ring_policy (net_t *n, double x, double y)
{
  //  see if net will constitute six-triangle rings

  int exists = 0;

  //  first the pidgeon in question
  //  then check three nearby holes
  //  then check nine in outer ones
  return exists;
}





//  finn current slot:
//  finn anker:

//  finn vektor
//  2x:
//    rotér vektor
//    policy:
//      kan ikke ha to trekanter på samme posisjon:
//        sjekk om noen slots har trekanter med liknende posisjon
//      kan ikke være i ring av 6 posisjoner:
//        finn vektor, rotér seks ganger
//        per 5 posisjoner
//          sjekk om noen slots har trekanter med liknende posisjon
//
//
//
//
//
//






void pin (net_t *n, int mouse_x, int mouse_y)
{
  if (stack_height(n->pinned) == TRIANGLES_TOTAL)
  {
  printf("full of triangles\n");
    return;
    }



  triangle_t *st = n->work_triangle;

  int num_neighbors;
  struct slot *neighbors[3] = {NULL, NULL, NULL};
  struct slot *nearest_neighbor;


  printf("hello\n");
  find_possible_anchors (st, n, &neighbors[0], &num_neighbors);



  //  pinned slots should have x, y set
/*
//  find current via array
  int q = -1; // number of possible slots
  int a = -1, b = -1, c = -1; //  indexes
  int i = 0, j;
  for (; i < TRIANGLES_TOTAL; i++)
  {
    if (q == 2)
    {
      break;
    }
    struct slot *t = &n->slots[i];
    if (t->pinned)
      continue;
    for (j = 0; j < 3; j ++)
    {
      if (t->positions[j] == NULL)
        {continue;
        }
      if (t->positions[j] == st)
      {
        if (a == -1)
        {
          a = i;
          ++ q;
          }
        else
        {
          if (b == -1)
           { b = i;
            ++ q;
            }
          else
          {
            if (c == -1)
              {c = i;
          ++ q;
              }
          }
        }
      }
    }
  }
  */

  if (num_neighbors <= 0)
  {
    printf ("unrgh %d\n", __LINE__);
  }
  else
  {


    nearest_neighbor = find_nearest_neighbor (neighbors, num_neighbors, st);



    printf ("slots are at %p, anchor is at %p\n", n->slots, nearest_neighbor);

    struct slot *cur = get_neighbor_with_triangle (nearest_neighbor, st);
    if (cur == NULL)
      printf ("something bent a-bad at %d\n",__LINE__);
    else
    {


      void num_pinned_neighbors (struct slot *s, int *buf)
      {
        *buf += s->A->pinned;
        *buf += s->B->pinned;
        *buf += s->C->pinned;
      }

//    remove me
      int k = 99;  // anchor's # pinned neighbors
      num_pinned_neighbors (nearest_neighbor, &k);
 //     printf ("nearest neighbor has %d pinned neighbors\n", k);

      int p = 0;  //  new slot's # pinned neighbors
      num_pinned_neighbors (cur, &p);
      printf ("current slot has %d pinned neighbors\n", p);


      int sx, sy;
      get_screen_triangle_position (st, &sx, &sy);
      // vector of distance between these slots can be rotated by 2PI/3 degrees
      //  point of origin is position of newly pinned slot
      double anx = nearest_neighbor->x - sx,
          any = nearest_neighbor->y - sy;

      double an_rotation = atan2 (any, anx), rotation  = an_rotation;
      // rotate vector

      printf("SX: %d, SY: %d\n",(int)sx, (int)sy);
      printf("vector from newly pinned to anchor is %d\n", (int)((rotation*180)/M_PI));
      /*
      if (rotation < 0)
      {
        printf ( "\t\t");
        while (rotation < 0)
        {
          printf ( ", modulating rotation");
          rotation += 2*M_PI;
        }
        printf ( "\n\t\tvector was rotated to %d\n", (int)((rotation*180)/M_PI));
      }
      */
//  x' = xcos(f) - ysin(f)
//  y = ycos(f) - xsin(f)











      int anchor_idx = -1;
//
      double new_x [ 3],  new_y [ 3];

      rotate_screen_triangle ( st, 2);
  /*
      int aX, aY,
          bX, bY,
          cX, cY;
*/
//      get_triangle_points_relative (st, &aX, &aY, &bX, &bY, &cX, &cY);
      get_triangle_points_relative (st, &new_x[0], &new_y[0], &new_x[1], &new_y[1], &new_x[2], &new_y[2]);

      int i = 0;

/*
      for(; i < 3; i ++)
      {
        new_x[i] += sx;
        new_y[i] += sy;
      }
*/
//printf("\tlooking for anchor's position in an array:\n");
      i = 0;

      for ( ; i < 3; i ++)
      {
        int a1 = approximates ( new_x[i], anx, FLOAT_ACC+3.0),
            a2 = approximates ( new_y[i], any, FLOAT_ACC+3.0);
  //      int a1 = approximates ( new_x[i], nearest_neighbor->x, FLOAT_ACC+5.0),
  //          a2 = approximates ( new_y[i], nearest_neighbor->y, FLOAT_ACC+5.0);


        if (a1 && a2)
        {
          printf("\t-\tfound: (%f, %f),\n", new_x[i], new_y[i]);
          printf("\t\t   vs. (%f, %f), -> %d\n",anx, any, (a1 && a2));
  //        printf("\t-\tIT HAS BEEN FOUND \n");
          anchor_idx = i;
          break;
        }else
        {

        }
      }
      printf("\t-\n");

      if (anchor_idx == -1)
        {
printf("VERY VERY VERY BAD:\tNO ANCHOR\n");
        return;
        }




//      rotation += triangle_modulation_in_radians ((get_screen_triangle_rotation(st) + 2) % 4);


      struct slot *A = cur->A, *B = cur->B, *C = cur->C;

      triangle_t *tA = NULL, *tC = NULL, *tB = NULL;

      struct slot *next_slot = NULL;
      double onetwenty= 120;
      onetwenty *=M_PI;
      onetwenty /= 180;
      int inserted_behind = 0;
      int num_expansions = 2, expansions = 1;

//      int cur_idx = anchor_idx;

      do
      {
        triangle_t **tp = NULL;



      //  printf ("VECTOR N { X:%f, Y:%f }\n", nx, ny);

 //       cur_idx += 1;
  //      cur_idx %= 3;
      //  if (cur_idx < 0)
      //    cur_idx += 3;

      //  printf ("VECTOR N { X:%f, Y:%f }\n", nx, ny);
        double tmpx, tmpy;  //  triangle positioning
            /*
            if (next_slot == A)
            { tmpx = aX;  tmpy = aY;
            }
            if (next_slot == B)
            { tmpx = bX;  tmpy = bY;
            }
            if (next_slot == C)
            { tmpx = cX;  tmpy = cY;
            }*/

        //    tmpx = nx;
         //   tmpy = ny;

//            tmpx = new_x[cur_idx];
  //          tmpy = new_y[cur_idx];










        // a change of C to B to A should represent a clockwise motion
      //  rotation = an_rotation + (2*onetwenty);
rotation =-onetwenty*expansions;
        if ( rotation >= M_PI)
        {
        //  printf ( "\t\t");
          while ( rotation >= M_PI)
          {
         //   printf ( ", modulating rotation");
            rotation -= 2 * M_PI;
          }
         // printf ( "\n\t\tvector was rotated to %d, %f\n", ( int) ( ( rotation * 180) / M_PI), rotation);
        }
        if ( rotation <= -M_PI)
        {
        //  printf ( "\t\t");
          while ( rotation <= -M_PI)
          {
         //   printf ( ", modulating rotation");
            rotation += 2 * M_PI;
          }
         // printf ( "\n\t\tvector was rotated to %d, %f\n", ( int) ( ( rotation * 180) / M_PI), rotation);
        }
      //  printf ( "rotation to new screen triangle is %d, %f\n", ( int) ( ( rotation * 180) / M_PI), rotation);




        double vecx = new_x[anchor_idx]* cos(rotation),
            vecy = new_y[anchor_idx]*cos(rotation);
        vecx -= new_y[anchor_idx]*sin(rotation);
        vecy += new_x[anchor_idx]*sin(rotation);

        if (pidgeon_policy(n, sx+vecx, sy+vecy))
        {
          printf("PIDGEON\n");
        }
        else
        {

        //  determine what slot we're on, and where to go next
        struct slot *rotator = next_slot;
        if (expansions == 1)
          rotator = nearest_neighbor;
        if (rotator == A)
        {   next_slot = B;

            if (!B->pinned)
            {

              tp = &tB;
            }
        }
        if (rotator == B)
        {   next_slot = C;

            if (!C->pinned)
            {
              tp = &tC;
            }
        }
        if (rotator == C)
        {   next_slot = A;

            if (!A->pinned)
            {
              tp = &tA;
            }
        }



        double nx, ny;
        nx = (anx * cos( rotation));
        nx += (-any * sin( rotation));

        ny = (anx * sin( rotation));
        ny += (any * cos( rotation));

        int pos_idx = -1;
        int ll = 0;
  printf("\tcomparing vector to calc positions in an array:\n");
        for ( ; ll < 3; ll ++)
        {
          if (ll == anchor_idx)
            continue;
          int a1 = approximates ( vecx, new_x[ll], FLOAT_ACC+2.0),
              a2 = approximates ( vecy, new_y[ll], FLOAT_ACC+2.0);

          if (a1 && a2)
          {
            printf("\t-\tfound: (%f,\t%f),\n", vecx, vecy);
            printf("\t\t   vs. (%f,\t%f); -> %d\n", new_x[ll], new_y[ll], (a1 && a2));
            pos_idx = ll;
            break;
          }
        }
  printf("\t-\n");



//tmpx = vecx;tmpy = vecy;


        //  lag et triangel
        if(tp != NULL)
        {
          *tp = make_screen_triangle (n->radius);
          if (*tp != NULL)
          {


            printf ("adding triangle at rotation %d, %f\n ", (int) (rotation *180/M_PI), rotation);

      //  if (pos_idx == -1) printf("NOT OKAY ALRIGHT\n");

            int dd = 0;

            if (next_slot->positions[dd] != NULL)
            {

              ++ dd;
              if (next_slot->positions[dd] != NULL)
              {

                ++ dd;
                if (next_slot->positions[dd] != NULL)
                {
                  free(next_slot->positions[dd]);
                }
              }
            }
            next_slot->positions[dd] = *tp;

          if (pos_idx > -1)
            set_screen_triangle_position (*tp, sx + new_x[pos_idx], sy + new_y[pos_idx]);
          else
          {
            set_screen_triangle_position (*tp, vecx+sx, vecx+sy);
            printf("NURHGFD\n");
            }
            rotate_screen_triangle (*tp, get_screen_triangle_rotation(st));

            //  slider må være tilbake på work triangle
            if (expansions == 0)
            {
              slider_insert_before (n->selector, (void *) *tp);
              inserted_behind ++;
            }
            else
            {
              slider_insert_after (n->selector, (void *) *tp);
            }



            //trenger fiks
          }


        }

        }
        ++  expansions;
      }
      while ( expansions <= num_expansions);


      rotate_screen_triangle ( st, 2);

      //  push pinned to stack
      char err = 0;
      int stack_addition = ((int)cur) - ((int)&n->slots[0]);
      stack_addition /= sizeof(struct slot);
      printf("ADDING %d TO STACK\n", stack_addition);
      stack_push(n->pinned, &err, stack_addition);

      //  remove other screen triangles from current slot

      for (i = 0; i < 3; i ++)
      {
        triangle_t **t_ptr = &cur->positions[i];
        if (*t_ptr != NULL)
        {
          if (st != *t_ptr)
          {
            free_screen_triangle(*t_ptr);
          }
          *t_ptr = NULL;
        }
      }

      //  set slot's triangle to work triangle
      cur->positions[0] = st;

      //  set position of working slot to that of working triangle
      cur->x = sx;
      cur->y = sy;

      //  pin it
      ++ cur->pinned;

      //  pull back, remove pinned from edge, in front
      slider_recede(n->selector);
      slider_remove_next(n->selector);

      if (inserted_behind)
        //  sometimes pull back twice
        slider_recede(n->selector);

      //  switch work triangle along edge
      n->work_triangle = slider_current(n->selector);


//  to trash later






    if(!k)
    {
      // we have a screen triangle
      //  and an anchor, nearest of three, spatially
      //    anchor has no neighbors
      // slot containing screen triangle can be located in memory via anchor




    }
    else if (k == 1)
    {
    //  struct slot *neighbor = NULL;

        //  new ones are C, then B
//        triangle_t *tA = NULL, *tC = NULL, *tB = NULL;
/*
        if (!A->pinned)
        {
          tA = make_screen_triangle (n->radius);
          if (!B->pinned && C->pinned)
          {
            printf("it was C\n");
            neighbor = C;
          }
          if (!C->pinned && B->pinned)
          {
            printf("it was B\n");
            neighbor = B;
          }
        }
        if (!B->pinned)
        {
          tB = make_screen_triangle (n->radius);
          if (!A->pinned  && C->pinned)
          {


            printf ("it was C\n");
            neighbor = C;
          }
          if (!C->pinned  &&  A->pinned)
          {
          printf("it was A\n");
            neighbor = A;
          }
        }
        if (!C->pinned)
        {

          tC = make_screen_triangle(n->radius);


          if (!A->pinned  &&  B->pinned)
          {
          printf("it was B\n");
            neighbor = B;
          }

          if (!B->pinned  &&  A->pinned)
          {
          printf("it was A\n");
            neighbor = A;
          }
        }
*/


        int rot = get_screen_triangle_rotation(st) + 2;
        rot %= 4;

        if (tA != NULL)
        { rotate_screen_triangle (tA, rot); }
        if (tB != NULL)
        { rotate_screen_triangle (tB, rot); }
        if (tC != NULL)
        { rotate_screen_triangle (tC, rot); }

        double a_x, a_y;
        double b_x, b_y;
        double c_x, c_y;

        double thirty = 30, sixty = 60;
        double radc = M_PI/180;
        thirty*= radc;
        sixty*= radc;

        switch (rot)
        {
          case 0:
            a_x = -cos (thirty);
            a_y = sin (thirty);
            b_x = cos (thirty);
            b_y = sin (thirty);
            c_x = 0;
            c_y = -1;
            break;
          case 1:

            a_x = -cos(sixty);
            a_y = sin(sixty);
            b_x = 1;
            b_y = 0;
            c_x = -cos (sixty);
            c_y = -sin (sixty);
            break;
          case 2:
            a_x = -cos (thirty);
            a_y = -sin (thirty);
            b_x = 0;
            b_y = 1;
            c_x = cos (thirty);
            c_y = -sin (thirty);
            break;
          case 3:
            a_x = -1;
            a_y = 0;
            b_x = cos (sixty);
            b_y = sin (sixty);
            c_x = cos (sixty);
            c_y = -sin (sixty);
            break;
          default:
          break;
        }
        double D = 2*cos(sixty)*n->radius;
        a_x *= D; a_y *= D;
        b_x *= D; b_y *= D;
        c_x *= D; c_y *= D;

        int r_x ,r_y;

        get_screen_triangle_position(st, &r_x, &r_y);


        a_x += r_x; b_x += r_x; c_x += r_x;
        a_y += r_y; b_y += r_y; c_y += r_y;
/*
        if (neighbor == A)
        {
        * */


     //   printf("testing A\n");
       /* if (tA != NULL)
          {set_screen_triangle_position (tA, (int)(a_x + 0.5), (int)(a_y + 0.5));}
       */
  /*
        if (tB != NULL)
          {set_screen_triangle_position (tB, (int)(b_x + 0.5), (int)(b_y + 0.5));}
        if (tC != NULL)
          {set_screen_triangle_position (tC, (int)(c_x + 0.5), (int)(c_y + 0.5));}
          }
          if (neighbor == B)
          {
          printf("testing B\n");
        if (tA != NULL)
          {set_screen_triangle_position (tA, (int)(a_x + 0.5), (int)(a_y + 0.5));}
        if (tB != NULL)
          {set_screen_triangle_position (tB, (int)(b_x + 0.5), (int)(b_y + 0.5));}
        if (tC != NULL)
          {set_screen_triangle_position (tC, (int)(c_x + 0.5), (int)(c_y + 0.5));}
          }
          if (neighbor == C)
          {
          printf("testing C\n");
        if (tA != NULL)
          {set_screen_triangle_position (tA, (int)(a_x + 0.5), (int)(a_y + 0.5));}
        if (tB != NULL)
          {set_screen_triangle_position (tB, (int)(b_x + 0.5), (int)(b_y + 0.5));}
        if (tC != NULL)
          {set_screen_triangle_position (tC, (int)(c_x + 0.5), (int)(c_y + 0.5));}
          }
*/

        slider_recede(n->selector);
        slider_remove_next(n->selector);
/*
        if (tA != NULL)
        {
          if (A->positions[0] != NULL)
          {
            if (A->positions[1] != NULL)
              A->positions[2] = tA;
            else
              A->positions[1] = tA;
          }
          else
          {
            A->positions[0] = tA;
          }

          slider_insert_after(n->selector, (void *) tA);
        }


        if (tC != NULL)
        {
          if (C->positions[0] != NULL)
          {
            if (C->positions[1] != NULL)
              C->positions[2] = tC;
            else
              C->positions[1] = tC;
          }
          else
          {
            C->positions[0] = tC;
          }

          slider_insert_after(n->selector, (void *) tC);
        }


        if (tB != NULL)
        {
          if (B->positions[0] != NULL)
          {
            if (B->positions[1] != NULL)
              B->positions[2] = tB;
            else
              B->positions[1] = tB;
          }
          else
          {
            B->positions[0] = tB;
          }

          slider_insert_after(n->selector, (void *) tB);
        }

*/

/*
        if (cur->positions[0] != st)
        {
          if (cur->positions[0] != NULL)
          {
            free (cur->positions[0]);
          }
          cur->positions[0] = st;
        }
*/
        char err = 0;
        stack_push(n->pinned, &err, ((cur - n->slots)/sizeof(struct slot)));
        n->work_triangle = slider_current(n->selector);




      //




      // calculate new position, rotation, etc for two new triangle


      // add new triangles to chain

      // push to stack



      //



      }
      else
      {
//        printf("I have not handled this case\n");
      }



    }
  }

}






void f2 ()
{
  int mX, mY;
  mouse_position (&mX, &mY);
  if (stack_height(d20->pinned) == 0)
  { //  root triangle is not pinned
    set_screen_triangle_position (d20->work_triangle, mX, mY);

    int scroll = mouse_scroll();
    if (rotate_root_triangle())
    {
      if (scroll)
      {
        d20->radius += scroll;
        resize_screen_triangle (d20->work_triangle, scroll);
      }
    }
    else
    {
      if (scroll)
      {
        d20->rotation += scroll;
        rotate_screen_triangle (d20->work_triangle, scroll);
      }
    }
    if (mouse_left() == -1)
    {
      pin_root_triangle(d20);
    }
  }
  else
  {


    int scroll = mouse_scroll();
    if (slide_selector())
    {
      if (scroll)
      {
        if (scroll == -1)
          slider_recede(d20->selector);
        if (scroll == 1)
        {
          slider_procede(d20->selector);
          printf("forward\n");
          }
        d20->work_triangle = slider_current(d20->selector);
      }
    }
    else
    {
      if (mouse_moves ())
      {
        triangle_t *nearest = find_triangle_nearest_to (mX, mY);
        while (nearest != (triangle_t *) slider_current(d20->selector))
        {
          slider_procede(d20->selector);
        }
        d20->work_triangle = nearest;
      }
    }

    if (mouse_right() == -1)
    { //  undoing
      if (stack_height(d20->pinned) == 1)
      {
        unpin_root_triangle(d20);
      }
      else
      {
        printf("undoing\n");

        void undo(net_t *n)
        {
          char err = 0;
          int c = stack_pop(n->pinned,&err);
          printf("popped %d from stack\n", c);
          struct slot *sl = &n->slots[c];
          triangle_t *t = sl->positions[0];
          if (sl->A->pinned + sl->B->pinned + sl->C->pinned == 0)
          {
            sl->positions[0] = NULL;
            free_screen_triangle(t);
            }
          else
            slider_insert_after(n->selector,t);
        //  slider_recede(n->selector);
          n->work_triangle = slider_current(n->selector);
          sl->pinned = 0;

          //  needs expanding upon

          //  ...
        }
        undo(d20);


      }
    }


    if (mouse_left() == -1)
    { //  doing
      printf("click\n");

      printf("we have %d triangles pinned\n", stack_height(d20->pinned));

pin(d20, mX, mY);


    }



  }
}


void f3 (void)
{
  app.rotate_root = 1;
  app.use_slider = 1;
  d20 = make_net_builder(35.0);
}

void f4 (void)
{
  free_net_builder(d20);

}
