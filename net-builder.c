#include "net-builder.h"

#include "chain.h"
#include "screen-triangles.h"
#include    <math.h>

//  distance between two points
double distance (double x1, double y1, double x2, double y2)
{ //  pythagorean root of squared sides summed
  double dx = x2 - x1, dy = y2 - y1;
  dx *= dx;
  dy *= dy;
  return sqrt (dx + dy);
}


int approximates (double test, double source, double accuracy)
{
  return (source + accuracy >= test)
      && (test >= source - accuracy);
}
#define FLOAT_ACC 2


typedef enum {APP_START, APP_MAIN, APP_END, APP_FREE} app_status;

static struct
{
  //  togglers
  char rotate_root;
  char use_slider;
  //  other things
  //  ...
  int mX, mY;
  app_status status;
} application;


int change_root_size (void)
{ //  toggle between rotation and resizing of root triangle
  if((mouse_middle() == -1)
      ||  (mouse_right() == -1))
  {
    application.rotate_root ^= 1;
  }
  return application.rotate_root;
}

int use_scroll_selector(void)
{ //  toggle between using mouse wheel or position for selections
  if(mouse_middle() == -1)
  {
    application.use_slider ^= 1;
  }
  return application.use_slider;
}


/*
 *  model of vertices in an icosahedron
 */
#define NUM_D20_VTX   12

vtx5i_t d20_model[NUM_D20_VTX] =
{
 {.pts = {1,2,3,4,5}},
 {.pts = {0,2,5,6,7}},
 {.pts = {0,1,7,8,3}},
 {.pts = {0,2,4,8,9}},
 {.pts = {0,3,5,9,10}},
 {.pts = {0,1,4,6,10}},
 {.pts = {1,5,7,10,11}},
 {.pts = {1,2,6,8,11}},
 {.pts = {2,3,7,9,11}},
 {.pts = {3,4,8,10,11}},
 {.pts = {4,5,6,10,11}},
 {.pts = {7,8,3,10,11}}
};

#define NUM_VTX_POS   5



struct vertex_slot
{
  char pinned;
  vtx2d_t *pos[NUM_VTX_POS];
  vtx2d_t *invalid;
  struct vertex_slot *links[NUM_VTX_POS];
};

typedef struct vertex_slot slot_t;


struct triangle_face
{
  slot_t *A, *B, *C;
  int pos_A, pos_B, pos_C;
};


typedef struct triangle_face triangle_t;

static struct
{
  int x, y;
  double radius;
  int rotation;

  chain_t *available, *faces;
  chainslider_t *free_selector, *used_selector;
  triangle_t *current_used, *current_free;

  slot_t vertex[NUM_D20_VTX];
} d20;



int rotate_root (int diff)
{ // rotate
  if (diff)
  {
    d20.rotation += diff;
    while (d20.rotation < 0)
      d20.rotation += 4;
    d20.rotation %= 4;
    return 1;;
  }
  return 0;
}

int resize_root (int diff)
{ // increase radius
  if (diff)
  {
    d20.radius += diff*1.75;
    return 1;
  }
  return 0;
}

void app_start (void)
{
  application.rotate_root = 1;
  application.use_slider = 1;
  application.status = APP_START;

  // initialize structures
  int j = 0, k;
  slot_t *s;
  for (; j < NUM_D20_VTX; j ++)
  {
    s = &d20.vertex[j];
    s->pinned = -1;
    for (k = 0; k < NUM_VTX_POS; k++)
    {
      s->pos[k] = NULL;
      s->links[k] = &d20.vertex[d20_model[j].pts[k]];
    }
    s->invalid = NULL;
  }

  d20.radius = 30.0;
  d20.x = 50;
  d20.y = 50;

  triangle_t *t = malloc(sizeof(triangle_t));
  t->A = &d20.vertex[0];
  t->B = &d20.vertex[1];
  t->C = &d20.vertex[2];
  t->pos_A = 0;
  t->pos_B = 0;
  t->pos_C = 0;
  t->A->pos[0] = calloc (1, sizeof(vtx2d_t));
  t->B->pos[0] = calloc (1, sizeof(vtx2d_t));
  t->C->pos[0] = calloc (1, sizeof(vtx2d_t));
  d20.current_free = t;
  d20.current_used = t;

  d20.free_selector = NULL;
  d20.used_selector = NULL;
  d20.faces = NULL;
  d20.available = NULL;
}

void app_free (void)
{
  //  free initialized structures
  int j = 0, k = 0;
  slot_t *s;
  for (; j < NUM_D20_VTX; j ++)
  {
    s = &d20.vertex[j];
    for (; k < NUM_VTX_POS; k++)
    {
      if (s->pos[k] != NULL)
        free (s->pos[k]);
    }
    if (s->invalid != NULL)
      free (s->invalid);
  }
  if (d20.faces == NULL)
  {
    if (d20.current_free != NULL)
      free(d20.current_free);
    return;
  }

  triangle_t *t;
  if (d20.available != NULL)
  {
    if (d20.free_selector == NULL)
      d20.free_selector = make_chainslider (d20.available);
    while (chain_size (d20.available) > 1)
      slider_remove_next (d20.free_selector);
    t = slider_current(d20.free_selector);
    free(t);
    free_chainslider (d20.free_selector);
    free_chain (d20.available);
  }
  if (d20.faces != NULL)
  {
    if (d20.used_selector == NULL)
      d20.used_selector = make_chainslider (d20.faces);
    while (chain_size (d20.faces) > 1)
      slider_remove_next (d20.used_selector);
    t = slider_current(d20.used_selector);
    free(t);
    free_chainslider (d20.used_selector);
    free_chain (d20.faces);
  }
}






void draw_unpinned (triangle_t *t)
{
  fill_triangle (t->A->pos[t->pos_A], t->B->pos[t->pos_B], t->C->pos[t->pos_C], invertPixel, 0);
}

void draw_pinned (triangle_t *t)
{
  draw_triangle (t->A->pos[t->pos_A], t->B->pos[t->pos_B], t->C->pos[t->pos_C], invertPixel, 0);
}

void draw_selected (triangle_t *t, COLOR colour)
{
  fill_triangle (t->A->pos[t->pos_A], t->B->pos[t->pos_B], t->C->pos[t->pos_C], colourPixel, colour);
}






void app_draw (void)
{
  SDL_FillRect (canvas, NULL, 0x0ff);
  SDL_BlitSurface (src_image, NULL, canvas, NULL);

  if (d20.faces == NULL)
  {
    draw_unpinned (d20.current_free);
  }
  else
  {
    //  draw pinned triangles
    triangle_t *start = slider_current (d20.used_selector),
        *cur = start;
    do
    {
      if (cur != d20.current_used)
        draw_pinned (cur);
      else
        draw_selected (cur, 0xff000000);
      slider_procede (d20.used_selector);
      cur = slider_current (d20.used_selector);
    }
    while (cur != start);

    //  draw free tirangles
    start = slider_current (d20.free_selector);
    cur = start;
    do
    {
      if (cur != d20.current_free)
        draw_unpinned (cur);
      else
        draw_selected (cur, 0xffff0000);
      slider_procede (d20.free_selector);
      cur = slider_current (d20.free_selector);
    }
    while (cur != start);

  }
}

void relocate_and_undo (void)
{
  if (mouse_middle() == -1)
  {
    //  relocate
  }
  if (mouse_right() == -1)
  {
    //  undoing
  }
}

void reposition_root_vertices (void)
{
  double radi = d20.radius;
  double half = 0.5;
  double root3 = sqrt(3.0);

  double aX = radi, aY = radi,
      bX = radi, bY = radi,
      cX = radi, cY = radi;

  switch (d20.rotation)
  {
    case 0:
      aX *= root3*half;   aY *= -half;
      bX *= -root3*half;  bY *= -half;
      cX *= 0;   cY *= 1;
      break;
    case 1:
      aX *= 1;   aY *= 0;
      bX *= -half;  bY *= -root3*half;
      cX *= -half;  cY *= root3*half;
      break;
    case 2:
      aX *= root3*half;   aY *= half;
      bX *= 0;   bY *= -1;
      cX *= -root3*half;  cY *= half;
      break;
    case 3:
      aX *= half; aY *= -root3*half;
      bX *= -1;   bY *= 0;
      cX *= half; cY *= root3*half;
      break;
  }

  vtx2d_t *ptr;
  ptr = d20.current_free->A->pos[0];
  ptr->pts[0] = d20.x + aX;
  ptr->pts[1] = d20.y + aY;
  ptr = d20.current_free->B->pos[0];
  ptr->pts[0] = d20.x + bX;
  ptr->pts[1] = d20.y + bY;
  ptr = d20.current_free->C->pos[0];
  ptr->pts[0] = d20.x + cX;
  ptr->pts[1] = d20.y + cY;
}






slot_t *find_slot_opposing (slot_t *anchor1, slot_t *anchor2, slot_t *opposer)
{
  slot_t *r = NULL;
  int link1 = 0, link2;
  for (;  link1 < NUM_VTX_POS; link1 ++)
  {
    if (r != NULL)
      break;
    slot_t *cur1 = anchor1->links[link1];
    if (cur1 == opposer)
      continue;
    for (link2 = 0; link2 < NUM_VTX_POS; link2 ++)
    {
      if (r != NULL)
        break;
      slot_t *cur2 = anchor2->links[link2];
      if (cur2 == opposer)
        continue;
      if (cur1 == cur2)
        r = cur1;
    }
  }
  return r;
}

vtx2d_t *find_vector_opposing (vtx2d_t *anchor1, vtx2d_t *anchor2, vtx2d_t *opposer)
{
  vtx2d_t middle = {.pts = {anchor2->pts[0] - anchor1->pts[0], anchor2->pts[1] - anchor1->pts[1]}};
  middle.pts[0] /= 2;
  middle.pts[1] /= 2;
  middle.pts[0] += anchor1->pts[0];
  middle.pts[1] += anchor1->pts[1];

  vtx2d_t *vector = malloc (sizeof(vtx2d_t));
  *vector = (vtx2d_t) {.pts = {opposer->pts[0] - middle.pts[0], opposer->pts[1] - middle.pts[1]}};
  vector->pts[0] *= -1;
  vector->pts[1] *= -1;
  vector->pts[0] += middle.pts[0];
  vector->pts[1] += middle.pts[1];
  return vector;
}





void app_usage ()
{


  if (d20.faces == NULL)
  { //  root triangle is not pinned
    int change = 0;
    if (mouse_moves())
    { // set root position
      d20.x = application.mX;
      d20.y = application.mY;
      mouse_position (&application.mX, &application.mY);
      ++ change;
    }
    else
    {
      if (mouse_left() == -1)
      {
        // pin root triangle
        d20.faces = make_chain (d20.current_free);
        d20.used_selector = make_chainslider (d20.faces);

        triangle_t *t = NULL, *anchor = d20.current_free;
        slot_t *new = NULL;

        t = malloc (sizeof(triangle_t));
        if (d20.available == NULL)
        {
          d20.available = make_chain (t);
          if (d20.free_selector == NULL)
            d20.free_selector = make_chainslider (d20.available);
        }
        else
        {
          if (d20.free_selector == NULL)
            d20.free_selector = make_chainslider (d20.available);

          slider_insert_after (d20.free_selector, (void *) t);
        }


        {
        //  A, B of anchor
          new = find_slot_opposing (anchor->A, anchor->B, anchor->C);
          new->pos[0] = find_vector_opposing (anchor->A->pos[anchor->pos_A], anchor->B->pos[anchor->pos_B], anchor->C->pos[anchor->pos_C]);


          t->A = anchor->A;
          t->pos_A = anchor->pos_A;
          t->B = anchor->B;
          t->pos_B = anchor->pos_B;
          t->C = new;
          t->pos_C = 0;
        }
        {
        //  B, C of anchor

          new = find_slot_opposing (anchor->B, anchor->C, anchor->A);
          new->pos[0] = find_vector_opposing (anchor->B->pos[anchor->pos_B], anchor->C->pos[anchor->pos_C], anchor->A->pos[anchor->pos_A]);

          t = malloc (sizeof(triangle_t));
          t->A = anchor->B;
          t->pos_A = anchor->pos_B;
          t->B = anchor->C;
          t->pos_B = anchor->pos_C;
          t->C = new;
          t->pos_C = 0;

          if (d20.available == NULL)
          {
            d20.available = make_chain (t);
            if (d20.free_selector == NULL)
              d20.free_selector = make_chainslider (d20.available);
          }
          else
          {
            if (d20.free_selector == NULL)
              d20.free_selector = make_chainslider (d20.available);

            slider_insert_after (d20.free_selector, (void *) t);
          }
        }
        {
        //  C, A of anchor

          new = find_slot_opposing (anchor->C, anchor->A, anchor->B);
          new->pos[0] = find_vector_opposing (anchor->C->pos[anchor->pos_C], anchor->A->pos[anchor->pos_A], anchor->B->pos[anchor->pos_B]);

          t = malloc (sizeof(triangle_t));
          t->A = anchor->C;
          t->pos_A = anchor->pos_C;
          t->B = anchor->A;
          t->pos_B = anchor->pos_A;
          t->C = new;
          t->pos_C = 0;

          if (d20.available == NULL)
          {
            d20.available = make_chain (t);
            if (d20.free_selector == NULL)
              d20.free_selector = make_chainslider (d20.available);
          }
          else
          {
            if (d20.free_selector == NULL)
              d20.free_selector = make_chainslider (d20.available);

            slider_insert_after (d20.free_selector, (void *) t);
          }
        }

        d20.current_free = slider_current (d20.free_selector);
        d20.current_used = slider_current (d20.used_selector);

      }
      int scroll = mouse_scroll();
      if (change_root_size())
      { change += resize_root(scroll);  }
      else
      { change += rotate_root(scroll);  }
    }
    if (change)
      reposition_root_vertices ();
  }
  else
  {
    //  root triangle has been pinned

    relocate_and_undo();

    if (mouse_left() == -1)
    {
      //  pinning
      printf("not yet tho\n");
    }
    int scroll = mouse_scroll();
    if (use_scroll_selector())
    {
      // selecting a triangle with the wheel
      if (scroll)
      {
        if (scroll == -1)
        {
          // backwards

        }
        if (scroll == 1)
        {
          // forwards
        }
      }
    }
    else
    {
      //selecting a triangle by position
      if (mouse_moves ())
      {
        mouse_position (&application.mX, &application.mY);
        vtx2i_t m = {.pts = {application.mX, application.mY}};

        triangle_t *new_cUsed = NULL, *new_cFree = NULL;

        triangle_t *start = slider_current (d20.used_selector),
            *cur = start;
        do
        {
          if (triangle_contains (cur->A->pos[cur->pos_A], cur->B->pos[cur->pos_B], cur->C->pos[cur->pos_C], m))
          {
            new_cUsed = cur;

            break;
          }
          slider_procede (d20.used_selector);
          cur = slider_current (d20.used_selector);
        }
        while (cur != start);

        d20.current_used = new_cUsed;


        start = slider_current (d20.free_selector);
        cur = start;
        do
        {
          if (triangle_contains (cur->A->pos[cur->pos_A], cur->B->pos[cur->pos_B], cur->C->pos[cur->pos_C], m))
          {
            new_cFree=  cur;
            break;
          }
          slider_procede (d20.free_selector);
          cur = slider_current (d20.free_selector);
        }
        while (cur != start);
        if (d20.current_free != new_cFree)
          d20.current_free = new_cFree;

      }
    }
  }
}





