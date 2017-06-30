#include "net-builder.h"

#include "chain.h"
#include "screen-triangles.h"
#include    <math.h>


extern SDL_Rect draw_area;


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
  char relocate_mode;
  //  relocation
  char being_relocated;
  int diff_x, diff_y;
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

int relocate_mode (void)
{ //  toggle between using mouse wheel or position for selections
  if((mouse_middle() == -1)
      ||  (mouse_right() == -1))
  {
    application.relocate_mode ^= 1;
  }
  return application.relocate_mode;
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


typedef struct triangle_face tripoint_t;

static struct
{
  int x, y;
  double radius;
  int rotation;

  chain_t *available, *faces;
  chainslider_t *free_selector, *used_selector;
  tripoint_t *current_used, *current_free;

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
  double add = 1.75 * diff;
  if (diff && (d20.radius + add > 0))
  {
    d20.radius += add;
    return 1;
  }
  return 0;
}

void app_start (void)
{
  application.rotate_root = 1;
  application.relocate_mode = 0;
  application.status = APP_START;
  application.being_relocated = 0;
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

  tripoint_t *t = malloc(sizeof(tripoint_t));
  t->A = &d20.vertex[0];
  t->B = &d20.vertex[1];
  t->C = &d20.vertex[2];
  t->pos_A = 0;
  t->pos_B = 0;
  t->pos_C = 0;
  t->A->pos[0] = calloc (1, sizeof(vtx2d_t));
  t->B->pos[0] = calloc (1, sizeof(vtx2d_t));
  t->C->pos[0] = calloc (1, sizeof(vtx2d_t));
  t->A->pinned = 0;
  t->B->pinned = 0;
  t->C->pinned = 0;
  d20.current_free = t;
  d20.current_used = NULL;

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

  tripoint_t *t;
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



void read_triangle_from_tripoint (tripoint_t *src, triangle_t *dst)
{
  dst->pts[0] = src->A->pos[src->pos_A];
  dst->pts[1] = src->B->pos[src->pos_B];
  dst->pts[2] = src->C->pos[src->pos_C];
}



void draw_triangle_transparent (tripoint_t *t)
{
  triangle_t tmp;
  read_triangle_from_tripoint(t, &tmp);
  fill_triangle (&tmp, invertPixel, 0);
}

void draw_triangle_outline (tripoint_t *t)
{
  triangle_t tmp;
  read_triangle_from_tripoint(t, &tmp);
  draw_triangle (&tmp, invertPixel, 0);
}

void draw_triangle_coloured (tripoint_t *t, COLOR colour)
{
  triangle_t tmp;
  read_triangle_from_tripoint(t, &tmp);
  fill_triangle (&tmp, colourPixel, colour);
}







#define   CLR_SELECTED_PINNED   0xff000000
#define   CLR_SELECTED_UNPINNED 0xffff0000


void app_draw (void)
{
  SDL_FillRect (canvas, NULL, 0x0ff);
  SDL_BlitSurface (src_image, NULL, canvas, &draw_area);
  if (d20.faces == NULL)
  {
    draw_triangle_transparent (d20.current_free);
  }
  else
  {
  tripoint_t *start, *cur;
    //  draw pinned triangles
    if ((d20.faces != NULL) && (d20.used_selector != NULL))
    {
    start = slider_current (d20.used_selector);
    cur = start;
    do
    {
      if (cur != d20.current_used)
        draw_triangle_outline (cur);
      else
        draw_triangle_coloured (cur, CLR_SELECTED_PINNED);

      slider_procede (d20.used_selector);
      cur = slider_current (d20.used_selector);
    }
    while (cur != start);
    }
    //  draw free tirangles
    if ((d20.available != NULL) && (d20.free_selector != NULL))
    {
    start = slider_current (d20.free_selector);
    cur = start;
    do
    {
      if (cur != d20.current_free)
        draw_triangle_transparent (cur);
      else
        draw_triangle_coloured (cur, CLR_SELECTED_UNPINNED);

      slider_procede (d20.free_selector);
      cur = slider_current (d20.free_selector);
    }
    while (cur != start);
    }
  }
}

void select_triangle (void)
{
  vtx2i_t m = {.pts = {application.mX, application.mY}};

  tripoint_t *new_cUsed = NULL, *new_cFree = NULL;

  tripoint_t *start = slider_current (d20.used_selector),
      *cur = start;
  triangle_t tmp_triangle;

  do
  {
    read_triangle_from_tripoint(cur, &tmp_triangle);
    if (triangle_contains (&tmp_triangle, m))
    {
      new_cUsed = cur;

      break;
    }
    slider_procede (d20.used_selector);
    cur = slider_current (d20.used_selector);
  }
  while (cur != start);
  if (d20.current_used != new_cUsed)
    d20.current_used = new_cUsed;

  if (d20.free_selector != NULL)
  {
  start = slider_current (d20.free_selector);
  cur = start;
  do
  {
    read_triangle_from_tripoint(cur, &tmp_triangle);
    if (triangle_contains (&tmp_triangle, m))
    {
      new_cFree = cur;
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




void adjust_root_coordinate (double *coordinate, double *error, int reference, int limit)
{
  if (coordinate != error)
    *coordinate = limit + ((reference - *error) + (*coordinate - reference));
}

int adjust_root_singleton (triangle_t *r, SDL_Rect *R)
{
  double *lX = &r->pts[0]->pts[0], *lY = &r->pts[0]->pts[1],
      *hX = &r->pts[0]->pts[0], *hY = &r->pts[0]->pts[1];
  if (*lX > r->pts[1]->pts[0])
    lX = &r->pts[1]->pts[0];
  if (*lY > r->pts[1]->pts[1])
    lY = &r->pts[1]->pts[1];
  if (*lX > r->pts[2]->pts[0])
    lX = &r->pts[2]->pts[0];
  if (*lY > r->pts[2]->pts[1])
    lY = &r->pts[2]->pts[1];
  if (*hX < r->pts[1]->pts[0])
    hX = &r->pts[1]->pts[0];
  if (*hY < r->pts[1]->pts[1])
    hY = &r->pts[1]->pts[1];
  if (*hX < r->pts[2]->pts[0])
    hX = &r->pts[2]->pts[0];
  if (*hY < r->pts[2]->pts[1])
    hY = &r->pts[2]->pts[1];

  if ( R->h < (*hY - *lY))
    return 0;
  if ( R->w < (*hX - *lX))
    return 0;


  //  lower limits
  if (*lX < R->x)
  {
    adjust_root_coordinate (&r->pts[0]->pts[0], lX, d20.x, R->x);
    adjust_root_coordinate (&r->pts[1]->pts[0], lX, d20.x, R->x);
    adjust_root_coordinate (&r->pts[2]->pts[0], lX, d20.x, R->x);

    *lX = R->x;
  }
  if (*lY < R->y)
  {
    adjust_root_coordinate (&r->pts[0]->pts[1], lY, d20.y, R->y);
    adjust_root_coordinate (&r->pts[1]->pts[1], lY, d20.y, R->y);
    adjust_root_coordinate (&r->pts[2]->pts[1], lY, d20.y, R->y);

    *lY = R->y;
  }
  // upper limits
  if (*hX > R->x + R->w)
  {
    adjust_root_coordinate (&r->pts[0]->pts[0], hX, d20.x, R->x + R->w);
    adjust_root_coordinate (&r->pts[1]->pts[0], hX, d20.x, R->x + R->w);
    adjust_root_coordinate (&r->pts[2]->pts[0], hX, d20.x, R->x + R->w);

    *hX = R->x + R->w;
  }
  if (*hY > R->y + R->h)
  {
    adjust_root_coordinate (&r->pts[0]->pts[1], hY, d20.y, R->y + R->h);
    adjust_root_coordinate (&r->pts[1]->pts[1], hY, d20.y, R->y + R->h);
    adjust_root_coordinate (&r->pts[2]->pts[1], hY, d20.y, R->y + R->h);

    *hY = R->y + R->h;
  }
  return 1;
}


void standard_root (triangle_t *dst)
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
  vtx2d_t *ptr = dst->pts[0];
  ptr->pts[0] = aX + d20.x;
  ptr->pts[1] = aY + d20.y;
  ptr = dst->pts[1];
  ptr->pts[0] = bX + d20.x;
  ptr->pts[1] = bY + d20.y;
  ptr = dst->pts[2];
  ptr->pts[0] = cX + d20.x;
  ptr->pts[1] = cY + d20.y;
}

int reposition_root_vertices (void)
{
  vtx2d_t a, b, c;
  triangle_t root = {.pts = {&a, &b, &c}};

  standard_root (&root);
  if (adjust_root_singleton (&root, &draw_area))
  {

  vtx2d_t *ptr;
  ptr = d20.current_free->A->pos[0];
  ptr->pts[0] = a.pts[0];
  ptr->pts[1] = a.pts[1];
  ptr = d20.current_free->B->pos[0];
  ptr->pts[0] = b.pts[0];
  ptr->pts[1] = b.pts[1];
  ptr = d20.current_free->C->pos[0];
  ptr->pts[0] = c.pts[0];
  ptr->pts[1] = c.pts[1];
  return 1;
  }
  else
    return 0;
}


int SDLRect_contains (vtx2d_t *p, SDL_Rect *limit)
{
  int r = 0;
  r |= ((p->pts[0] < limit->x) || (p->pts[0] > limit->x + limit->w));
  r |= ((p->pts[1] < limit->y) || (p->pts[1] > limit->y + limit->h));
  return (r == 0);
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




void pin_root ()
{
  d20.faces = make_chain (d20.current_free);
  d20.used_selector = make_chainslider (d20.faces);
  d20.current_used = d20.current_free;
  d20.current_free = NULL;

  slot_t *new = NULL;
  tripoint_t *t = NULL, *anchor = d20.current_used;
  vtx2d_t *tmp_pos = NULL;

  //  A, B as anchor
  new = find_slot_opposing (anchor->A, anchor->B, anchor->C);
  tmp_pos = find_vector_opposing (anchor->A->pos[anchor->pos_A], anchor->B->pos[anchor->pos_B], anchor->C->pos[anchor->pos_C]);
  if (SDLRect_contains(tmp_pos, &draw_area))
  {
    t = malloc (sizeof(tripoint_t));
    if (d20.available == NULL)
    {
      d20.available = make_chain (t);
      if (d20.free_selector == NULL)
        d20.free_selector = make_chainslider (d20.available);
    }
    else
      slider_insert_after (d20.free_selector, (void *) t);

    bcopy(anchor, t, sizeof(tripoint_t));
    t->C = new;
    t->pos_C = 0;
    new->pos[0] = tmp_pos;
  }

  //  B, C as anchor
  new = find_slot_opposing (anchor->B, anchor->C, anchor->A);
  tmp_pos = find_vector_opposing (anchor->B->pos[anchor->pos_B], anchor->C->pos[anchor->pos_C], anchor->A->pos[anchor->pos_A]);
  if (SDLRect_contains(tmp_pos, &draw_area))
  {
    t = malloc (sizeof(tripoint_t));
    if (d20.available == NULL)
    {
      d20.available = make_chain (t);
      if (d20.free_selector == NULL)
        d20.free_selector = make_chainslider (d20.available);
    }
    else
      slider_insert_after (d20.free_selector, (void *) t);

    bcopy(anchor, t, sizeof(tripoint_t));
    t->A = new;
    t->pos_A = 0;
    new->pos[0] = tmp_pos;
  }

  //  C, A as anchor
  new = find_slot_opposing (anchor->C, anchor->A, anchor->B);
  tmp_pos = find_vector_opposing (anchor->C->pos[anchor->pos_C], anchor->A->pos[anchor->pos_A], anchor->B->pos[anchor->pos_B]);
  if (SDLRect_contains(tmp_pos, &draw_area))
  {
    t = malloc (sizeof(tripoint_t));
    if (d20.available == NULL)
    {
      d20.available = make_chain (t);
      if (d20.free_selector == NULL)
        d20.free_selector = make_chainslider (d20.available);
    }
    else
      slider_insert_after (d20.free_selector, (void *) t);

    bcopy(anchor, t, sizeof(tripoint_t));
    t->B = new;
    t->pos_B = 0;
    new->pos[0] = tmp_pos;
  }
  if (d20.free_selector != NULL)
    d20.current_free = slider_current (d20.free_selector);

}


void app_usage ()
{
  if (mouse_moves ())
  {
    mouse_position (&application.mX, &application.mY);
  }
  if (d20.faces == NULL)
  { //  root triangle is not pinned
    if (mouse_moves())
    {
      d20.x = application.mX;
      d20.y = application.mY;
      reposition_root_vertices();
    }
    else
    {
      int resize = change_root_size();
      int scroll = mouse_scroll();
      if (scroll)
      {
        if (resize)
        {
          if (scroll < 0)
          {
            resize_root(scroll);
            reposition_root_vertices();
          }
          else
          {
            resize_root(scroll);
            if ((scroll > 0) && !reposition_root_vertices())
            {
              resize_root(-scroll);
            }
          }
        }
        else
        {
          rotate_root(scroll);
          if (!reposition_root_vertices())
            rotate_root(-scroll);
        }
      }
    }

    if (mouse_left() == -1)
    {
      // pin root triangle
      pin_root();
    }

  }
  else
  {
    //  root triangle has been pinned
    int relocate = relocate_mode();

    if (mouse_left() == -1)
    {
      if (relocate)
      {
        if (application.being_relocated)
          application.being_relocated --;
        else
        {
          application.being_relocated ++;
          application.diff_x = application.mX - d20.x;
          application.diff_y = application.mY - d20.y;
        }
      }
      else
      {
        if (d20.current_free != NULL)
        { // pin


        }
        else if (d20.current_used != NULL)
        { //  undo


        }
      }
    }

    if (mouse_moves ())
    {
      if (relocate)
      {
        if (application.being_relocated)
        {
          //  reposition vertices

        }
      }
      else
      {
        select_triangle();
      }
    }
  }
}





