#include "net-builder.h"

#include "chain.h"
#include "screen-triangles.h"
#include    <math.h>
#include <string.h>
#ifdef _WIN32
#include <malloc.h>
#endif

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
 {.pts = {1,2,3,5,6}},
 {.pts = {0,2,3,4,7}},
 {.pts = {0,1,4,5,8}},
 {.pts = {0,1,6,7,9}},
 {.pts = {1,2,7,8,10}},
 {.pts = {0,2,6,8,11}},
 {.pts = {0,3,5,9,11}},
 {.pts = {1,3,4,9,10}},
 {.pts = {2,4,5,10,11}},
 {.pts = {3,6,7,10,11}},
 {.pts = {4,7,8,9,11}},
 {.pts = {5,6,8,9,10}}
};

#define NUM_VTX_POS   5



struct vertex_slot
{

  struct vertex_slot *links[NUM_VTX_POS];
};

typedef struct vertex_slot slot_t;


struct triangle_face
{
  slot_t *sA, *sB, *sC;
  vtx2d_t *pA, *pB, *pC;
};


typedef struct triangle_face tripoint_t;

static struct
{
  int x, y;
  double radius;
  int rotation;

  chain_t *available, *faces, *positions;
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
    for (k = 0; k < NUM_VTX_POS; k++)
    {
      s->links[k] = &d20.vertex[d20_model[j].pts[k]];
    }
  }

  d20.radius = 30.0;
  d20.x = 50;
  d20.y = 50;

  tripoint_t *t = (tripoint_t *) malloc(sizeof(tripoint_t));
  t->sA = &d20.vertex[0];
  t->sB = &d20.vertex[1];
  t->sC = &d20.vertex[2];

  t->pA = (vtx2d_t *) calloc (1, sizeof(vtx2d_t));
  t->pB = (vtx2d_t *) calloc (1, sizeof(vtx2d_t));
  t->pC = (vtx2d_t *) calloc (1, sizeof(vtx2d_t));

  d20.positions = make_chain(t->pA);
  chainslider_t *slider = make_chainslider (d20.positions);
  slider_insert_after(slider, t->pB);
  slider_insert_after(slider, t->pC);
  free_chainslider(slider);

  d20.current_free = t;
  d20.current_used = NULL;

  d20.faces = NULL;
  d20.available = NULL;
  application.status = APP_MAIN;
}

void app_free (void)
{
  //  free initialized structures
  chainslider_t *slider;
  if (d20.available != NULL)
  {
    slider = make_chainslider (d20.available);
    while (chain_size (d20.available) > 1)
    {
      free((void *)slider_current(slider));
      slider_recede(slider);
      slider_remove_next (slider);
    }
    free((void *) slider_current(slider));
    free_chainslider (slider);
    free_chain (d20.available);
  }
  if (d20.faces != NULL)
  {
    slider = make_chainslider (d20.faces);
    while (chain_size (d20.faces) > 1)
    {
      free((void *) slider_current(slider));
      slider_recede(slider);
      slider_remove_next (slider);
    }
    free((void *) slider_current(slider));
    free_chainslider (slider);
    free_chain (d20.faces);
  }
  if (d20.positions != NULL)
  {
    slider = make_chainslider(d20.positions);
    while (chain_size (d20.positions) > 1)
    {
      free((void *) slider_current(slider));
      slider_recede(slider);
      slider_remove_next (slider);
    }
    free((void *) slider_current(slider));
    free_chainslider (slider);
    free_chain (d20.positions);
  }
}



void read_triangle_from_tripoint (tripoint_t *src, triangle_t *dst)
{
  dst->pts[0] = src->pA;
  dst->pts[1] = src->pB;
  dst->pts[2] = src->pC;
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

  printf("drawing...");
  fflush(stdout);

  SDL_FillRect (canvas, NULL, 0x0ff);
  SDL_BlitSurface (src_image, NULL, canvas, &draw_area);

  if (application.status == APP_MAIN)
  {
  if (d20.faces == NULL)
  {
    draw_triangle_transparent (d20.current_free);
  }
  else
  {
    tripoint_t *start, *cur;
    chainslider_t *slider;
    //  draw pinned triangles
    if (d20.faces != NULL)
    {
      slider = make_chainslider(d20.faces);
      start = slider_current (slider);
      cur = start;
      do
      {
        if (cur != d20.current_used)
          draw_triangle_coloured (cur, CLR_SELECTED_PINNED);
        else
          draw_triangle_transparent (cur);
        slider_procede (slider);
        cur = slider_current (slider);
      }
      while (cur != start);

      free_chainslider (slider);
    }
    //  draw free tirangles
    if (d20.available != NULL)
    {
      slider = make_chainslider(d20.available);
      start = slider_current (slider);
      cur = start;
      do
      {
        if (cur != d20.current_free)
          draw_triangle_transparent (cur);
        else
          draw_triangle_coloured (cur, CLR_SELECTED_UNPINNED);

        slider_procede (slider);
        cur = slider_current (slider);
      }
      while (cur != start);
      free_chainslider(slider);
    }
  }
  }
  if (application.status == APP_END)
  {
    tripoint_t *start, *cur;
    chainslider_t *slider;
    //  draw pinned triangles
    if (d20.faces != NULL)
    {
      slider = make_chainslider(d20.faces);
      start = slider_current (slider);
      cur = start;
      do
      {
        draw_triangle_outline (cur);
        slider_procede (slider);
        cur = slider_current (slider);
      }
      while (cur != start);

      free_chainslider (slider);
    }


  }

  printf("\tdone\n");
}

void select_triangle (void)
{
  printf("selecting...");
  fflush(stdout);

  triangle_t tmp_triangle;
  vtx2i_t m = {.pts = {application.mX, application.mY}};

  tripoint_t *new_cUsed = NULL, *new_cFree = NULL;

  chainslider_t *slider = make_chainslider (d20.faces);
  tripoint_t *start = slider_current (slider),
      *cur = start;

  do
  {
    read_triangle_from_tripoint(cur, &tmp_triangle);
    if (triangle_contains (&tmp_triangle, m))
    {
      new_cUsed = cur;
      break;
    }
    slider_procede (slider);
    cur = slider_current (slider);
  }
  while (cur != start);
  free_chainslider(slider);

  if(d20.available != NULL)
  {
    slider = make_chainslider(d20.available);
    start = slider_current (slider);
    cur = start;
    do
    {
      read_triangle_from_tripoint(cur, &tmp_triangle);
      if (triangle_contains (&tmp_triangle, m))
      {
        new_cFree = cur;
        break;
      }
      slider_procede (slider);
      cur = slider_current (slider);
    }
    while (cur != start);

    free_chainslider(slider);
  }

  if (d20.current_used != new_cUsed)
    d20.current_used = new_cUsed;

  if (d20.current_free != new_cFree)
    d20.current_free = new_cFree;
  printf("\tdone\n");
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
  ptr = d20.current_free->pA;
  ptr->pts[0] = a.pts[0];
  ptr->pts[1] = a.pts[1];
  ptr = d20.current_free->pB;
  ptr->pts[0] = b.pts[0];
  ptr->pts[1] = b.pts[1];
  ptr = d20.current_free->pC;
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

  vtx2d_t *vector = (vtx2d_t *) malloc (sizeof(vtx2d_t));
  *vector = (vtx2d_t) {.pts = {opposer->pts[0] - middle.pts[0], opposer->pts[1] - middle.pts[1]}};
  vector->pts[0] *= -1;
  vector->pts[1] *= -1;
  vector->pts[0] += middle.pts[0];
  vector->pts[1] += middle.pts[1];
  return vector;
}




void create_root_neighbor (char k)
{

  void insert (tripoint_t *ptr)
  {

    if (d20.available == NULL)
      d20.available = make_chain (ptr);

    else
    {
      chainslider_t *slider = make_chainslider(d20.available);
      slider_insert_after (slider, (void *) ptr);
      free_chainslider(slider);
    }
  }


  slot_t *new = NULL;
  tripoint_t *t = NULL, *anchor = d20.current_free;
  vtx2d_t *tmp_pos = NULL;

  switch (k)
  {
    case 'A':
      new = find_slot_opposing (anchor->sB, anchor->sC, anchor->sA);
      tmp_pos = find_vector_opposing (anchor->pB, anchor->pC, anchor->pA);
      break;
    case 'B':
      new = find_slot_opposing (anchor->sC, anchor->sA, anchor->sB);
      tmp_pos = find_vector_opposing (anchor->pC, anchor->pA, anchor->pB);
      break;
    case 'C':
      new = find_slot_opposing (anchor->sA, anchor->sB, anchor->sC);
      tmp_pos = find_vector_opposing (anchor->pA, anchor->pB, anchor->pC);
      break;
  }

  if (SDLRect_contains(tmp_pos, &draw_area))
  {
    chainslider_t *slider = make_chainslider(d20.positions);
    slider_insert_after(slider, tmp_pos);
    free_chainslider(slider);

    t = (tripoint_t *) malloc (sizeof(tripoint_t));
    insert(t);
    memcpy(t, anchor, sizeof(tripoint_t));
    switch (k)
    {
      case 'A':
        t->sA = new;
        t->pA = tmp_pos;
        break;
      case 'B':
        t->sB = new;
        t->pB = tmp_pos;
        break;
      case 'C':
        t->sC = new;
        t->pC = tmp_pos;
        break;
    }
  }
  else
  {
    free((void *) tmp_pos);
    free((void *) t);
  }
}

void pin_root (void)
{
  if (d20.faces == NULL)
    d20.faces = make_chain (d20.current_free);

  create_root_neighbor('A');
  create_root_neighbor('B');
  create_root_neighbor('C');

  d20.current_used = NULL;
  d20.current_free = NULL;
}


int equal_faces (tripoint_t *A, tripoint_t *B)
{
  int eq = 0;
  eq += (A->sA == B->sA);
  eq += (A->sA == B->sB);
  eq += (A->sA == B->sC);

  eq += (A->sB == B->sA);
  eq += (A->sB == B->sB);
  eq += (A->sB == B->sC);

  eq += (A->sC == B->sA);
  eq += (A->sC == B->sB);
  eq += (A->sC == B->sC);

  return (eq == 3);
}

int equal_vertices (vtx2d_t *A, vtx2d_t *B, double acc)
{
  return (approximates(A->pts[0], B->pts[0], acc) && approximates(A->pts[1], B->pts[1], acc));
}

vtx2d_t *position_exists (vtx2d_t *p)
{
  vtx2d_t *ptr = NULL;

  chainslider_t *slider = make_chainslider(d20.positions);
  vtx2d_t *start = slider_current(slider),
      *cur = start;
  printf("started looking for preexcisting coordinate..");
  do
  {
    if (equal_vertices(cur, p, 2.5))
    {
      printf("found duplicate coordinate vertex\n");
      ptr = cur;
      break;
    }
    slider_procede (slider);
    cur = slider_current (slider);
  }
  while (cur != start);
  printf("\tdone\n");

  if (ptr == NULL)
    slider_insert_after(slider, p);

  free_chainslider(slider);

  return ptr;
}


int already_pinned (tripoint_t *t)
{
  int eq = 0;
   chainslider_t *slider = make_chainslider(d20.faces);
  tripoint_t *start = slider_current (slider),
      *cur = start;

  do
  {
    if (equal_faces(cur, t))
    {
      eq ++;
      break;
    }
    slider_procede (slider);
    cur = slider_current (slider);
  }
  while (cur != start);
  free_chainslider(slider);
  return eq;
}


void create_neighbor_triangles_for (tripoint_t *p)
{
  slot_t *new = NULL;
  tripoint_t *t = NULL;
  vtx2d_t *tmp_pos = NULL;
  chainslider_t *slider = make_chainslider(d20.available);
  //  A, B as anchor points
  {
    t = (tripoint_t *) malloc (sizeof(tripoint_t));
    new = find_slot_opposing (p->sA, p->sB, p->sC);
    memcpy(t,p, sizeof(tripoint_t));
    t->sC = new;

    if (already_pinned(t))
    {
      free((void *) t);
    }
    else
    {
      tmp_pos = find_vector_opposing (p->pA, p->pB, p->pC);
      if (SDLRect_contains(tmp_pos, &draw_area))
      {
        vtx2d_t *pos = position_exists(tmp_pos);
        if (pos != NULL)
        {
          free ((void *) tmp_pos);
          tmp_pos = pos;
        }
        slider_insert_after (slider, (void *) t);
        t->pC = tmp_pos;
      }
      else
      {
        free ((void *) tmp_pos);
        free ((void *) t);
      }
    }
  }
  //  B, C as anchor points
  {
    t = (tripoint_t *) malloc (sizeof(tripoint_t));
    new = find_slot_opposing (p->sB, p->sC, p->sA);
    memcpy(t, p, sizeof(tripoint_t));
    t->sA = new;

    if (already_pinned(t))
    {
      free ((void *) t);
    }
    else
    {
      tmp_pos = find_vector_opposing (p->pB, p->pC, p->pA);
      if (SDLRect_contains(tmp_pos, &draw_area))
      {
        vtx2d_t *pos = position_exists(tmp_pos);
        if (pos != NULL)
        {
          free ((void *) tmp_pos);
          tmp_pos = pos;
        }
        slider_insert_after (slider, (void *) t);
        t->pA = tmp_pos;
      }
      else
      {
        free ((void *) tmp_pos);
        free ((void *) t);
      }
    }
  }
  //  C, A as anchor
  {
    t = (tripoint_t *) malloc (sizeof(tripoint_t));
    new = find_slot_opposing (p->sC, p->sA, p->sB);
    memcpy(t,p, sizeof(tripoint_t));
    t->sB = new;

    if (already_pinned(t))
    {
      free ((void *) t);
    }
    else
    {
      tmp_pos = find_vector_opposing (p->pC, p->pA, p->pB);
      if (SDLRect_contains(tmp_pos, &draw_area))
      {
        vtx2d_t *pos = position_exists(tmp_pos);
        if (pos != NULL)
        {
          free ((void *) tmp_pos);
          tmp_pos = pos;
        }
        slider_insert_after (slider, (void *) t);

        t->pB = tmp_pos;
      }
      else
      {
        free ((void *) tmp_pos);
        free ((void *) t);
      }
    }
  }
  free_chainslider(slider);
}

void pin (void)
{
  tripoint_t *anchor = d20.current_free;


  d20.current_used = NULL;
  d20.current_free = NULL;

  chainslider_t *slider = make_chainslider(d20.faces);
  slider_insert_after (slider, anchor);
  free_chainslider(slider);

  slider = make_chainslider(d20.available);

  tripoint_t *start, *cur;

  start = slider_current(slider);
  cur = start;
  do
  {
    if (cur == anchor)
    {
      slider_procede(slider);
      slider_remove_prev(slider);
      break;
    }

    slider_procede (slider);
    cur = slider_current (slider);
  }
  while (cur != start);

  create_neighbor_triangles_for (anchor);

  //  remove duplicates + anchor
  cur = slider_current (slider);

  int k = chain_size (d20.available);
  do
  {
    if (chain_size(d20.available) == 1)
      break;
    if (equal_faces(cur, anchor))
    {
      slider_procede(slider);
      slider_remove_prev(slider);
      free((void *) cur);
    }

    slider_procede (slider);
    cur = slider_current (slider);
    --k;
  }
  while (k);


  if (chain_size(d20.available) == 1)
  {
    cur = slider_current (slider);
    if (already_pinned(cur))
    {
    application.status = APP_END;
    free_chain(d20.available);
    d20.available = NULL;
    free((void *)cur);
    }
  }

  free_chainslider(slider);
}


void app_main (void)
{
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
          application.diff_x = d20.x - application.mX;
          application.diff_y = d20.y - application.mY;
        }
      }
      else
      {
        if (d20.current_free != NULL)
        {
            pin();
        }
        else if (d20.current_used != NULL)
        { //  undo
          if (chain_size(d20.faces) == 1)
          {
            app_free();
            app_start();
          }
          else
          {


          }
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



void app_usage ()
{
  if (mouse_moves ())
  {
    mouse_position (&application.mX, &application.mY);
  }
  if (application.status == APP_MAIN)
    app_main();
  else if (application.status == APP_END)
  {




  }

}





