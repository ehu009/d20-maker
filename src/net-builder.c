#include <math.h>
#include <string.h>
#ifdef _WIN32
  #include <malloc.h>
#endif

#include "d20.h"
#include "net-builder.h"
#include "chain.h"
#include "screen-triangles.h"


#include "storage.h"
#include "fader.h"
#include "button.h"




extern SDL_Rect draw_area;


int mouse_on_draw_area (void)
{
  int x, y;
  mouse_position(&x, &y);
  return ((x >= draw_area.x) && (x <= draw_area.x + draw_area.w)
      &&  (y >= draw_area.y) && (y <= draw_area.y + draw_area.h));
}



int approximates (double test, double source, double accuracy)
{
  return (source + accuracy >= test)
      && (test >= source - accuracy);
}
#define FLOAT_ACC 2


vtx2d_t *position_exists (vtx2d_t *p, chain_t *positions)
{
  vtx2d_t *ptr = NULL;

  chainslider_t *slider = make_chainslider(positions);
  vtx2d_t *start = slider_current(slider),
      *cur = start;
 // printf("started looking for preexcisting coordinate..");
  do
  {
    if (equal_vertices(cur, p, 2.5))
    {
   //   printf("found duplicate coordinate vertex\n");
      ptr = cur;
      break;
    }
    slider_procede (slider);
    cur = slider_current (slider);
  }
  while (cur != start);
 // printf("\tdone\n");

  if (ptr == NULL)
    slider_insert_after(slider, p);

  free_chainslider(slider);

  return ptr;
}



typedef enum {APP_START, APP_MAIN, APP_END, APP_FREE} app_status;

static struct
{
  //  togglers
  char rotate_root;
  char relocate_mode;
  //  relocation
  char being_relocated;
  int diff_x, diff_y;

  double negate_rate;
  fader_t *colour_select;

  button_t *reset_btn, *save_btn;
  unsigned save, reset;
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




#include "d20.h"


/*
void read_triangle_from_tripoint (face_t *src, triangle_t *dst)
{

 // dst->pts[0] = src->pA;
 // dst->pts[1] = src->pB;
 // dst->pts[2] = src->pC;


  *dst = *src->triangle;


}
*/

void draw_triangle_transparent (face_t *t)
{
  fill_triangle (t->item, invertPixel, 0);
}

void draw_triangle_outline (face_t *t)
{
  draw_triangle (t->item, invertPixel, 0);
}

void draw_triangle_coloured (face_t *t, COLOUR colour)
{
  fill_triangle (t->item, colourPixel, colour);
}

void draw_triangle_on (face_t *t, SDL_Surface *dst, SDL_Rect *at)
{
  transfer_triangle(t->item, dst, at);
}









static struct
{
  int x, y;
  double radius;
  int rotation;

  chain_t *available, *faces, *positions, *lines;
  face_t *current_used, *current_free;

  slot_t net[NUM_D20_VTX];

} d20;






void print_face(face_t *f)
{
  #define IDX(x) (int)(x - d20.net)
  printf("face %p has slots %d, %d, %d\n", f, IDX(f->sA), IDX(f->sB), IDX(f->sC));
}





int face_exists_in (face_t *t, chain_t *list)
{
  int eq = 0;
  chainslider_t *slider = make_chainslider(list);
  face_t *start = slider_current (slider),
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



face_t *find_current_selected (chain_t *list, vtx2i_t *mouse)
{
  chainslider_t *slider = make_chainslider (list);
  face_t *start = slider_current (slider),
      *cur = start,
      *ret = NULL;

  do
  {
    if (triangle_contains (cur->item, *mouse))
    {
      ret = cur;
      break;
    }
    slider_procede (slider);
    cur = slider_current (slider);
  }
  while (cur != start);
  free_chainslider(slider);
  return ret;
}



char line_exists (struct line *ptr, chain_t *lines)
{
  chainslider_t *s = make_chainslider(lines);

  struct line *start, *cur;
  start = slider_current (s);
  cur = start;
  char exists = 0;
  do
  {
    if (equal_lines(cur, ptr))
    {
      exists ++;
      break;
    }
    slider_procede (s);
    cur = slider_current (s);
  }
  while (cur != start);
  free (s);
  return exists;
}


void add_lines_for (chain_t **lines, face_t *t)
{
  struct line *a, *b, *c;
  a = malloc (sizeof (struct line));
  b = malloc (sizeof (struct line));
  c = malloc (sizeof (struct line));
  triangle_t *q = t->item;
  a->A = q->pts[0];
  a->B = q->pts[1];
  b->A = q->pts[1];
  b->B = q->pts[2];
  c->A = q->pts[2];
  c->B = q->pts[0];
  chainslider_t *s;

  if (*lines == NULL)
  {
    *lines = make_chain();
    s = make_chainslider(*lines);
    slider_insert_after(s, a);
    slider_insert_after(s, b);
    slider_insert_after(s, c);
    free(s);
    return;
  }

  s = make_chainslider(*lines);
  void add_or_free_line (struct line *ptr)
  {
    if (line_exists(ptr, *lines))
    {
      free(ptr);
    }
    else
    {
      slider_insert_after(s, ptr);
    }
  }
  add_or_free_line (a);
  add_or_free_line (b);
  add_or_free_line (c);

  free(s);
}




#define TRIANGLE_NUM_ROTATIONS  4


int rotate_root (int diff)
{ // rotate
  if (diff)
  {
    d20.rotation += diff;
    while (d20.rotation < 0)
      d20.rotation += TRIANGLE_NUM_ROTATIONS;
    d20.rotation %= TRIANGLE_NUM_ROTATIONS;
    return 1;;
  }
  return 0;
}

#define TRIANGLE_RESIZE_FACTOR 1.75

int resize_root (int diff)
{ // increase radius
  double add = TRIANGLE_RESIZE_FACTOR * diff;
  if (diff && (d20.radius + add > 0))
  {
    d20.radius += add;
    return 1;
  }
  return 0;
}


int negate_byte_at (unsigned byte, unsigned mask, double rate)
{
  int pt = ((byte ^ 0xffffff) & mask) - (byte & mask);
  pt *= rate;
  pt += (byte & mask);
  while (mask >0xff)
  {
    mask /= 0x100;
    pt /= 0x100;
  }
  return pt;
}

void divertPixel (SDL_Surface *dst, vtx2i_t *p, unsigned color)
{
  vtx2i_t p2 = *p;
  SDL_Rect clip_rect;
  SDL_GetClipRect (canvas, &clip_rect);
  if (clip_rect.x != 0)
    p2.pts[0] += clip_rect.x;
  if (clip_rect.y != 0)
    p2.pts[1] += clip_rect.y;

  unsigned clr = getPixel (canvas, &p2);
  unsigned R, G, B;

  R = negate_byte_at(clr, canvas->format->Rmask, application.negate_rate);
  G = negate_byte_at(clr, canvas->format->Gmask, application.negate_rate);
  B = negate_byte_at(clr, canvas->format->Bmask, application.negate_rate);

  clr = SDL_MapRGBA(dst->format, R,G,B,0xff);
  setPixel (dst, p, clr);
}



void free_list (chain_t *list)
{
  if (list == NULL)
    return;
  chainslider_t *slider = make_chainslider (list);
  while (chain_size (list))
  {
    free((void *) slider_current(slider));
    slider_procede(slider);
    slider_remove_prev (slider);
  }
  free((void *) slider_current(slider));
  free_chainslider (slider);
  free_chain (list);
}



#define NET_START_X 50
#define NET_START_Y 50
#define NET_START_RADIUS 30.0

void init_net_builder (void)
{
// initialize structures
  d20.radius = NET_START_RADIUS;
  d20.x = NET_START_X;
  d20.y = NET_START_Y;


  // need some safety on this one
  //d20.model = new_d20();
  init_d20(d20.net);

  face_t *t = malloc(sizeof(face_t));
  {
    t->sA = &d20.net[0];
    t->sB = &d20.net[1];
    t->sC = &d20.net[2];

    triangle_t *tr = calloc(1, sizeof(triangle_t));

    tr->pts[0] = (vtx2d_t *) calloc (1, sizeof(vtx2d_t));
    tr->pts[1] = (vtx2d_t *) calloc (1, sizeof(vtx2d_t));
    tr->pts[2] = (vtx2d_t *) calloc (1, sizeof(vtx2d_t));
    t->item = tr;

    d20.positions = make_chain(tr->pts[0]);
    chainslider_t *slider = make_chainslider (d20.positions);
    slider_insert_after(slider, tr->pts[1]);
    slider_insert_after(slider, tr->pts[2]);
    free_chainslider(slider);
  }

  d20.current_free = t;
  d20.current_used = NULL;

  d20.faces = NULL;
  d20.available = NULL;
  d20.lines = NULL;



}

void app_start (void)
{
  application.status = APP_START;

  init_net_builder();
  application.rotate_root = 1;
  application.relocate_mode = 0;
  application.being_relocated = 0;

  application.colour_select = NULL;
  application.reset_btn = NULL;
  application.save_btn = NULL;

  application.reset = 0;
  application.save = 0;


  application.status = APP_MAIN;
}

void free_face_list(chain_t *list)
{
  if (list == NULL)
    return;
  chainslider_t *slider = make_chainslider (list);
  face_t *ptr = NULL;

  while (chain_size (list))
  {
    ptr = slider_current(slider);
    free(ptr->item);
    free(ptr);

    slider_procede(slider);
    slider_remove_prev (slider);
  }

//  ptr = slider_current(slider);
  free_chainslider (slider);

<<<<<<< HEAD
//  free(ptr->triangle);
//  free(ptr);
=======
  free(ptr->item);
  free(ptr);
>>>>>>> b371d62... generalized d20 storage model, moved some things strictly related to d20 modelling
  free_chain (list);

}

void app_free (void)
{
  //  free allocated structures
  free_face_list(d20.available);
  free_face_list(d20.faces);
 // free_list(d20.available);
 // free_list(d20.faces);

  free_list(d20.lines);
  free_list(d20.positions);
  d20.positions = NULL;
  d20.lines = NULL;
  d20.faces = NULL;
  d20.available = NULL;
 // free(d20.model);

  button_free(application.reset_btn);
  application.reset_btn = NULL;
  button_free(application.save_btn);
  application.save_btn = NULL;
  fader_free(application.colour_select);
}







void pinned_list_drawing (face_t *t)
{
  if (t != d20.current_used)
    draw_triangle_coloured (t, TRIANGLE_COLOUR_PINNED);
  else
    draw_triangle_transparent (t);
}

void unpinned_list_drawing (face_t *t)
{
  if (t != d20.current_free)
    draw_triangle_transparent (t);
  else
    draw_triangle_coloured (t, TRIANGLE_COLOUR_UNPINNED);
}

void finished_list_drawing (face_t *t)
{
//  draw_triangle_outline (t);

}



typedef void (*tripoint_draw_func) (face_t *);

void draw_list (chain_t *list, tripoint_draw_func draw_func)
{
  chainslider_t *slider;
  slider = make_chainslider(list);
  face_t *start, *cur;
  start = slider_current (slider);
  cur = start;
  do
  {
    draw_func(cur);

    slider_procede (slider);
    cur = slider_current (slider);
  }
  while (cur != start);

  free_chainslider (slider);
}





void copy_triangles (chain_t *triangles, SDL_Surface *dst)
{
  chainslider_t *s = make_chainslider (triangles);
  face_t *start, *cur;
  start = slider_current (s);
  cur = start;
  do
  {
    draw_triangle_on(cur, dst, &draw_area);

    slider_procede (s);
    cur = slider_current (s);
  }
  while (cur != start);
  free_chainslider(s);
}

void draw_lines (chain_t *lines)
{
  chainslider_t *s = make_chainslider(lines);

  struct line *start, *cur;
  start = slider_current (s);
  cur = start;
  do
  {
    vtx2i_t A, B;
    get_vtx2i_from_vtx2d(cur->A, &A);
    get_vtx2i_from_vtx2d(cur->B, &B);

    draw_line2(canvas, &A, &B, divertPixel, 0);

    slider_procede (s);
    cur = slider_current (s);
  }
  while (cur != start);
  free_chainslider(s);
}


void _draw_end (void)
{
  SDL_Surface *surf = SDL_CreateRGBSurface (0, draw_area.w, draw_area.h, 32, canvas->format->Rmask, canvas->format->Gmask, canvas->format->Bmask, alpha_mask);
  if (surf != NULL)
  {
    COLOUR filler = SDL_MapRGBA(surf->format, 0,0,0,0xff);
    SDL_FillRect(surf, NULL, filler);

    copy_triangles(d20.faces, surf);

    SDL_BlitSurface (surf, NULL, canvas, &draw_area);
    SDL_FreeSurface (surf);
  }

}

void _draw_main(void)
{
  if (d20.faces == NULL)
    draw_triangle_transparent (d20.current_free);
  else
  {
    draw_list (d20.faces, pinned_list_drawing);

    if (d20.available != NULL)
      draw_list (d20.available, unpinned_list_drawing);
  }
}

void app_draw (void)
{
  #ifdef DEBUG
  printf("drawing...");
  fflush(stdout);
  #endif

  SDL_FillRect (canvas, NULL, COLOUR_BACKGROUND);
  SDL_BlitSurface (src_image, NULL, canvas, &draw_area);

  if (application.status == APP_MAIN)
  {
    _draw_main();
  }
  if (application.status == APP_END)
  {
    _draw_end();
  }

  if (application.colour_select != NULL)
  {
    fader_draw(application.colour_select);
    draw_lines(d20.lines);
  }
  if ((application.save_btn != NULL) && (chain_size(d20.faces) == 20))
    button_draw(application.save_btn);

  if (application.reset_btn != NULL)
    button_draw(application.reset_btn);

  #ifdef DEBUG
  printf("\tdone\n");
  #endif
}


void select_triangle (void)
{
  #ifdef DEBUG
  printf("selecting...");
  fflush(stdout);
  #endif

  vtx2i_t m = {.pts = {application.mX, application.mY}};

  face_t *new_cUsed = NULL, *new_cFree = NULL;

  new_cUsed = find_current_selected(d20.faces, &m);
  if(d20.available != NULL)
    new_cFree = find_current_selected(d20.available, &m);



  d20.current_free = new_cFree;
  d20.current_used = new_cUsed;

  #ifdef DEBUG
  printf("\tdone\n");
  #endif
}




void adjust_root_coordinate (double *coordinate, double *error, int reference, int limit)
{
  if (coordinate != error)
    *coordinate = limit + ((reference - *error) + (*coordinate - reference));
}

int limit_root_triangle (triangle_t *r, SDL_Rect *R)
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

int reposition_root(void)
{
  vtx2d_t a, b, c;
  triangle_t root = {.pts = {&a, &b, &c}};

  standard_root (&root);
  if (limit_root_triangle (&root, &draw_area))
  {

    vtx2d_t *ptr;
    triangle_t *t = d20.current_free->item;
    ptr = t->pts[0];
    ptr->pts[0] = a.pts[0];
    ptr->pts[1] = a.pts[1];
    ptr = t->pts[1];
    ptr->pts[0] = b.pts[0];
    ptr->pts[1] = b.pts[1];
    ptr = t->pts[2];
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






int equal_triangles (triangle_t *t1, triangle_t *t2, double acc)
{
  int q = 0;
  {
    q += equal_vertices(t1->pts[0],t2->pts[0],acc);
    q += equal_vertices(t1->pts[1],t2->pts[0],acc);
    q += equal_vertices(t1->pts[2],t2->pts[0],acc);
  }
  {
    q += equal_vertices(t1->pts[0],t2->pts[1],acc);
    q += equal_vertices(t1->pts[1],t2->pts[1],acc);
    q += equal_vertices(t1->pts[2],t2->pts[1],acc);
  }
  {
    q += equal_vertices(t1->pts[0],t2->pts[2],acc);
    q += equal_vertices(t1->pts[1],t2->pts[2],acc);
    q += equal_vertices(t1->pts[2],t2->pts[2],acc);
  }
  return (q == 3);
}

int triangle_exists (triangle_t *t, chain_t *list, double acc)
{
  chainslider_t *s = make_chainslider(list);
  face_t *start = slider_current(s), *cur = start;
  int q = 0;
  do
  {
    if (equal_triangles(cur->item, t, acc))
    {
      ++ q;
      break;
    }
    cur = slider_current(s);
    slider_procede(s);
  }
  while (cur != start);
  return q;
}




face_t *create_root_neighbor (char k)
{

  slot_t *new = NULL;
  face_t *t = NULL, *anchor = d20.current_free;
  vtx2d_t *tmp_pos = NULL;
  triangle_t *q = anchor->item;
  switch (k)
  {
    case 'A':
      new = find_slot_opposing (anchor->sB, anchor->sC, anchor->sA);
      tmp_pos = find_vector_opposing (q->pts[1], q->pts[2], q->pts[0]);
      break;
    case 'B':
      new = find_slot_opposing (anchor->sC, anchor->sA, anchor->sB);
      tmp_pos = find_vector_opposing (q->pts[2], q->pts[0], q->pts[1]);
      break;
    case 'C':
      new = find_slot_opposing (anchor->sA, anchor->sB, anchor->sC);
      tmp_pos = find_vector_opposing (q->pts[0], q->pts[1], q->pts[2]);
      break;
  }

  if (SDLRect_contains(tmp_pos, &draw_area))
  {
    chainslider_t *slider = make_chainslider(d20.positions);
    slider_insert_after(slider, tmp_pos);
    free_chainslider(slider);

    t = copy_face(anchor);
   // insert(t);

    triangle_t *p = malloc (sizeof(triangle_t));
    switch (k)
    {
      case 'A':
        t->sA = new;
        p->pts[0] = tmp_pos;
        p->pts[1] = q->pts[1];
        p->pts[2] = q->pts[2];
        break;
      case 'B':
        t->sB = new;
        p->pts[1] = tmp_pos;
        p->pts[2] = q->pts[2];
        p->pts[0] = q->pts[0];
        break;
      case 'C':
        t->sC = new;
        p->pts[2] = tmp_pos;
        p->pts[0] = q->pts[0];
        p->pts[1] = q->pts[1];
        break;
    }
    t->item = p;
    print_face(t);
  }
  else
  {
    free((void *) tmp_pos);
    free((void *) t);
    t = NULL;
  }
  return t;
}




void pin_root (void)
{
  void insert (face_t *ptr)
  {
    if (d20.available == NULL)
      d20.available = make_chain ();

    chainslider_t *slider = make_chainslider(d20.available);
    slider_insert_after (slider, (void *) ptr);
    free_chainslider(slider);
  }

  if (d20.faces == NULL)
  {
    d20.faces = make_chain ();
    chainslider_t *slider = make_chainslider(d20.faces);
    slider_insert_after(slider, d20.current_free);
  }

  face_t *ptr = NULL;
  char c = 'A';
  do
  {
    ptr = create_root_neighbor(c);
    if (ptr != NULL)
      insert(ptr);
    ++c;
  }
  while(c <= 'C');



//  create_root_neighbor('B');
//  create_root_neighbor('C');
  add_lines_for(&d20.lines, d20.current_free);

  d20.current_used = NULL;
  d20.current_free = NULL;
  printf("ROOT PINNED\n");
}




face_t *neighbor_for_slot(char n, face_t *p)
{
  char err = 0;
  face_t *t = copy_face(p);
  err |= (t == NULL);

  if (err)
    return t;

  t->item = NULL;
  switch (n)
  {
    case 'A':
      t->sA = find_slot_opposing (p->sB, p->sC, p->sA);
      break;
    case 'B':
      t->sB = find_slot_opposing (p->sC, p->sA, p->sB);
      break;
    case 'C':
      t->sC = find_slot_opposing (p->sA, p->sB, p->sC);
      break;
  }
  int q = face_exists_in(t, d20.faces);
  err |= (q);

  printf("trying to add neighbor:  ");
  print_face(t);
  if (err)
  {
    printf("already pinned\n");
    //print_face(t);
    free(t);
    return NULL;
  }
  vtx2d_t *tmp_pos = NULL;
  triangle_t *pt = p->item;
  switch (n)
  {
    case 'A':
      tmp_pos = find_vector_opposing (pt->pts[1], pt->pts[2], pt->pts[0]);
      break;
    case 'B':
      tmp_pos = find_vector_opposing (pt->pts[2], pt->pts[0], pt->pts[1]);
      break;
    case 'C':
      tmp_pos = find_vector_opposing (pt->pts[0], pt->pts[1], pt->pts[2]);
      break;
  }
  if (!SDLRect_contains(tmp_pos, &draw_area))
  {
  printf("outside draw area\n");
    free (tmp_pos);
    free (t);
    return NULL;
  }
  vtx2d_t *pos = position_exists(tmp_pos, d20.positions);
  if (pos != NULL)
  {
    free (tmp_pos);
    tmp_pos = pos;
    printf("used existing position %p\n", pos);
  }

  triangle_t *tr = calloc(1, sizeof(triangle_t));
  switch (n)
  {
    case 'A':
      tr->pts[0] = tmp_pos;
      tr->pts[1] = pt->pts[1];
      tr->pts[2] = pt->pts[2];
      break;
    case 'B':
      tr->pts[1] = tmp_pos;
      tr->pts[2] = pt->pts[2];
      tr->pts[0] = pt->pts[0];
      break;
    case 'C':
      tr->pts[2] = tmp_pos;
      tr->pts[0] = pt->pts[0];
      tr->pts[1] = pt->pts[1];
      break;
  }
  t->item = tr;
  return t;
}

void create_neighbor_triangles_for (face_t *p)
{
  char c = 'A';
  face_t *ptr = NULL;
  chainslider_t *s = make_chainslider(d20.available);
  do
  {
    ptr = neighbor_for_slot(c, p);
    if (ptr != NULL)
    {
      if (!triangle_exists(ptr->item, d20.available, FLOAT_ACC))
      {
        slider_insert_after (s, (void *) ptr);
        printf("added neighbor\n");
      }
      else
      {
        printf("triangle seems to exist on screen\n");
        free(ptr->item);
        free(ptr);
      }
    }
    ++ c;
  }
  while (c <= 'C');
  //  A, B as anchor points
//  neighbor_for_slot('A', p);
//  neighbor_for_slot('B', p);
//  neighbor_for_slot('C', p);


  free_chainslider(s);
}

void unlink_unpinned (chain_t *unpinned, face_t *ptr)
{
  if (unpinned == NULL)
    return;

  chainslider_t *s = make_chainslider(unpinned);
  face_t *start = slider_current(s),
    *cur = start;
  do
  {
    if (equal_triangles(cur->item, ptr->item, FLOAT_ACC))
    {
      slider_procede(s);
      slider_remove_prev(s);
      break;
    }

    slider_procede (s);
    cur = slider_current (s);
  }
  while (cur != start);
  free_chainslider(s);
}

void remove_faces_similar_to (chain_t *unpinned, face_t *cmp)
  //  remove duplicates
{
  int k = chain_size (unpinned), q = k;
  if ((unpinned == NULL) || (k == 1))
    return;
  printf("trying to remove duplicates\n");
  chainslider_t *s = make_chainslider(unpinned);
  face_t *cur = slider_current (s);

  do
  {
      slider_procede(s);
      if (equal_faces(cur, cmp))
      {

        slider_remove_prev(s);
        printf("removed a duplicate :  ");
        print_face(cur);
        free(cur->item);
        free(cur);
      }
//    }
//    slider_procede (s);
    cur = slider_current (s);
    --k;
  }
  while (k);
  free(s);
  k = q-chain_size(unpinned);
  if (k)
  printf("removed %d duplicates\n",k);
}



void pin (void)
{
printf("####\n");
  printf("PINNING\nnum pinned : %d\nthe face in question is: ", chain_size(d20.faces));
  face_t *anchor = d20.current_free;

  print_face(anchor);

  //remove_unpinned(d20.available, anchor);

d20.current_used = NULL;
  d20.current_free = NULL;

  unlink_unpinned (d20.available, anchor);

 // if (chain_size(d20.faces) != 4)
//  else printf("NO NEIGHBORS HERE NYAA\n");
  remove_faces_similar_to (d20.available, anchor);

    create_neighbor_triangles_for (anchor);




//  insert into list of placed triangles
  chainslider_t *slider = make_chainslider(d20.faces);
  slider_insert_after (slider, anchor);
  free_chainslider(slider);
  slider = NULL;

  add_lines_for (&d20.lines, anchor);

  //  no more triangles possible
  if (chain_size(d20.available) == 1)
  {
    slider = make_chainslider(d20.available);
  face_t *cur;
    cur = slider_current (slider);
    if (face_exists_in(cur, d20.faces))
    {
      free_chain(d20.available);
      d20.available = NULL;
    }
    free_chainslider(slider);
  }

}




void app_main (void)
{
  if (d20.faces == NULL)
  { //  root triangle is not pinned
    if (mouse_moves())
    {
      d20.x = application.mX;
      d20.y = application.mY;
      reposition_root();
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
            reposition_root();
          }
          else
          {
            resize_root(scroll);
            if ((scroll > 0) && !reposition_root())
            {
              resize_root(-scroll);
            }
          }
        }
        else
        {
          rotate_root(scroll);
          if (!reposition_root())
            rotate_root(-scroll);
        }
      }
    }

    if (mouse_left() == -1)
    {
      // pin root triangle
      if (mouse_on_draw_area())
      {
        pin_root();
        if (application.reset_btn == NULL)
        {
          application.reset_btn = button_create ("reset\0", &application.reset);
          button_setPosition(application.reset_btn, canvas->w-(BORDER_SIZE+BUTTON_WIDTH), BORDER_SIZE);
        }
      }
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
          if (mouse_on_draw_area())
          {
            pin();
            if (chain_size(d20.faces) == 20)
            {
              application.status = APP_END;
              if (application.save_btn == NULL)
              {
                application.save_btn = button_create ("save \0", &application.save);
                button_setPosition(application.save_btn, canvas->w-(BORDER_SIZE+BUTTON_WIDTH), 2*BORDER_SIZE + BUTTON_HEIGHT);
              }
              if (application.colour_select == NULL)
              {
                unsigned fader_yPos = 3*BORDER_SIZE + 2*BUTTON_HEIGHT;
                application.colour_select = fader_create(80.0, FADER_WIDTH, canvas->h - (BORDER_SIZE + fader_yPos), &application.negate_rate);
                fader_setPos(application.colour_select, canvas->w - (BORDER_SIZE + BUTTON_WIDTH), fader_yPos);
              }
            }
          }
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


    mouse_position (&application.mX, &application.mY);
  if (application.status == APP_MAIN)
  {
    app_main();

    if (application.reset_btn != NULL)
      button_update(application.reset_btn);

  }
  else if (application.status == APP_END)
  {
    if (application.colour_select != NULL)
    {
      fader_update(application.colour_select);
      application.negate_rate /= 100;
    }

    if (application.reset_btn != NULL)
      button_update(application.reset_btn);

    if (application.save_btn != NULL)
      button_update(application.save_btn);

    if (application.save)
    {
      d20.current_free = NULL;
      d20.current_used = NULL;
      app_draw();
      store_subsurface(canvas, &draw_area);
    }
  }

  if (application.reset)
  {
    app_free();
    app_start();
  }

}





