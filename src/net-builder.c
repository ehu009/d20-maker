#include <math.h>
#include <string.h>
#ifdef _WIN32
  #include <malloc.h>
#endif

#include "debug.h"

#include "d20.h"
#include "net-builder.h"
#include "list.h"
#include "screen-triangles.h"


#include "storage.h"
#include "fader.h"
#include "button.h"




extern SDL_Rect draw_area;


int SDLRect_contains (vtx2d_t *p, SDL_Rect *limit)
{
  int r = 0;
  r |= ((p->pts[0] < limit->x) || (p->pts[0] > limit->x + limit->w));
  r |= ((p->pts[1] < limit->y) || (p->pts[1] > limit->y + limit->h));
  return (r == 0);
}

int mouse_on_draw_area (void)
{
  vtx2d_t pt;
  int x, y;
  mouse_position(&x, &y);
  pt.pts[0] = x;
  pt.pts[1] = y;

  return SDLRect_contains(&pt, &draw_area);
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
  int x, y;
  double radius;
  int rotation;

  list_t *positions, *lines;

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





d20_t d20;



vtx2d_t *position_exists (vtx2d_t *p, list_t *positions)
{
  vtx2d_t *ptr = NULL;

  list_i *slider = make_list_iterator(positions);
  vtx2d_t *cur = list_iterator_next(slider);
  while (cur != NULL)
  {
    if (equal_vertices(cur, p, FLOAT_ACC))
    {
      ptr = cur;
      break;
    }
    cur = list_iterator_next(slider);
  }
  

  if (ptr == NULL)
  {
    list_insert(positions, p);
  }

  free_list_iterator(slider);

  return ptr;
}


int face_exists_in (face_t *t, list_t *list)
{
  int eq = 0;
  list_i *slider = make_list_iterator(list);
  face_t *cur = list_iterator_next(slider);

  while (cur != NULL)
  {
    if (equal_faces(cur, t))
    {
      eq ++;
      break;
    }
    cur = list_iterator_next(slider);
  }
  
  
  free_list_iterator(slider);
  return eq;
}


face_t *find_current_selected (list_t *list, vtx2i_t *mouse)
{
  list_i *slider = make_list_iterator (list);
  face_t *cur = list_iterator_next(slider),
      *ret = NULL;

  while (cur != NULL)
  {
    if (triangle_contains (cur->item, *mouse))
    {
      ret = cur;
      break;
    }
    cur = list_iterator_next(slider);
  }
  
  free_list_iterator(slider);
  return ret;
}


char line_exists (struct line *ptr, list_t *lines)
{
  list_i *s = make_list_iterator(lines);

  struct line *cur = list_iterator_next(s);
  char exists = 0;
  while (cur != NULL)
  {
    if (equal_lines(cur, ptr))
    {
      exists ++;
      break;
    }
    cur = list_iterator_next(s);
  }
  
  free_list_iterator(s);
  return exists;
}


void print_face(face_t *f)
{
  #define IDX(x) (int)(x - d20.net)
  printf("face %p has slots %d, %d, %d\n", f, IDX(f->sA), IDX(f->sB), IDX(f->sC));
}


void add_lines_for (list_t **lines, triangle_t *t)
{
  struct line *a, *b, *c;
  a = malloc (sizeof (struct line));
  b = malloc (sizeof (struct line));
  c = malloc (sizeof (struct line));

  a->A = t->pts[0];
  a->B = t->pts[1];
  b->A = t->pts[1];
  b->B = t->pts[2];
  c->A = t->pts[2];
  c->B = t->pts[0];
  
  if (*lines == NULL)
  {
    *lines = make_list();
    list_insert(*lines, a);
    list_insert(*lines, b);
    list_insert(*lines, c);
    return;
  }

  void add_or_free_line (struct line *ptr)
  {
    if (line_exists(ptr, *lines))
    {
      free(ptr);
    }
    else
    {
      list_insert(*lines, ptr);
    }
  }
  add_or_free_line (a);
  add_or_free_line (b);
  add_or_free_line (c);

}




#define TRIANGLE_NUM_ROTATIONS  4


int rotate_root (int diff)
{ // rotate
  if (diff)
  {
    application.rotation += diff;
    while (application.rotation < 0)
      application.rotation += TRIANGLE_NUM_ROTATIONS;
    application.rotation %= TRIANGLE_NUM_ROTATIONS;
    return 1;;
  }
  return 0;
}

#define TRIANGLE_RESIZE_FACTOR 1.75

int resize_root (int diff)
{ // increase radius
  double add = TRIANGLE_RESIZE_FACTOR * diff;
  if (diff && (application.radius + add > 0))
  {
    application.radius += add;
    return 1;
  }
  return 0;
}

void empty_list (list_t *list)
{
  if (list == NULL)
  {
    return;
  }
  list_i *slider = make_list_iterator (list);
  void *item = list_iterator_next(slider);
  
  while (item != NULL)
  {
    free(item);
    item = list_iterator_next(slider);
  }
  free_list_iterator (slider);
  free_list (list);
}



#define NET_START_X 50
#define NET_START_Y 50
#define NET_START_RADIUS 30.0



face_t *root_face (slot_t *root)
{
  return new_face(root, &root[1], &root[2]);
}

void init_net_builder (void)
{
// initialize structures
  application.radius = NET_START_RADIUS;
  application.x = NET_START_X;
  application.y = NET_START_Y;

  colour_negate_rate = &application.negate_rate;

  init_d20(d20.net);

  face_t *t = root_face(d20.net);
  if (t != NULL)
  {
    triangle_t *tr = calloc(1, sizeof(triangle_t));

    tr->pts[0] = (vtx2d_t *) calloc (1, sizeof(vtx2d_t));
    tr->pts[1] = (vtx2d_t *) calloc (1, sizeof(vtx2d_t));
    tr->pts[2] = (vtx2d_t *) calloc (1, sizeof(vtx2d_t));
    t->item = tr;

    application.positions = make_list();
    list_insert(application.positions, tr->pts[0]);
    list_insert(application.positions, tr->pts[1]);
    list_insert(application.positions, tr->pts[2]);
  }

  d20.current_free = t;
  d20.current_used = NULL;

  d20.faces = NULL;
  d20.available = NULL;
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


void app_free (void)
{
  // free structures in the d20
  empty_list(d20.available);
  d20.available = NULL;
  empty_list(d20.faces);
  d20.faces = NULL;

  // free graphical elements
  empty_list(application.lines);
  application.lines = NULL;
  empty_list(application.positions);
  application.positions = NULL;

  // free buttons
  button_free(application.reset_btn);
  application.reset_btn = NULL;
  button_free(application.save_btn);
  application.save_btn = NULL;

  // free slidebar
  fader_free(application.colour_select);
  application.colour_select = NULL;
}







void pinned_list_drawing (face_t *t)
{
  if (t != d20.current_used)
  {
    draw_triangle_coloured (t, TRIANGLE_COLOUR_PINNED);
  }
  else
  {
    draw_triangle_transparent (t);
  }
}

void unpinned_list_drawing (face_t *t)
{
  if (t != d20.current_free)
  {
    draw_triangle_transparent (t);
  }
  else
  {
    draw_triangle_coloured (t, TRIANGLE_COLOUR_UNPINNED);
  }
}



typedef void (*tripoint_draw_func) (face_t *);

void draw_list (list_t *list, tripoint_draw_func draw_func)
{
  list_i *slider;
  slider = make_list_iterator(list);
  if (slider == NULL)
  {
    return;
  }
    
  face_t *cur = list_iterator_next(slider);
  while (cur != NULL)
  {
    draw_func(cur);

    cur = list_iterator_next (slider);
  }

  free_list_iterator (slider);
}





void copy_triangles (list_t *triangles, SDL_Surface *dst)
{
  list_i *s = make_list_iterator (triangles);
  face_t *cur = list_iterator_next (s);
  while (cur != NULL)
  {
    draw_triangle_on(cur, dst, &draw_area);

    cur = list_iterator_next (s);
  }
  
  free_list_iterator(s);
}

void draw_lines (list_t *lines)
{
  list_i *s = make_list_iterator(lines);

  struct line *cur = list_iterator_next (s);
  
  while (cur != NULL)
  {
    vtx2i_t A, B;
    get_vtx2i_from_vtx2d(cur->A, &A);
    get_vtx2i_from_vtx2d(cur->B, &B);

    draw_line(canvas, &A, &B, divertPixel, 0);

    cur = list_iterator_next (s);
  }
  
  free_list_iterator(s);
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
  {
    draw_triangle_transparent (d20.current_free);
  }
  else
  {
    draw_list (d20.faces, pinned_list_drawing);

    draw_lines(application.lines);

    if (d20.available != NULL)
    {
      draw_list (d20.available, unpinned_list_drawing);
    }
  }
}

void app_draw (void)
{
  DEBUG(2, "Started drawing.")
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
    draw_lines(application.lines);
  }
  if ((application.save_btn != NULL) && (list_size(d20.faces) == 20))
  {
    button_draw(application.save_btn);
  }

  if (application.reset_btn != NULL)
  {
    button_draw(application.reset_btn);
  }

  DEBUG(2, "Finished drawing.")
}


void select_triangle (void)
{

  DEBUG(2, "Started selecting.")
  vtx2i_t m = {.pts = {application.mX, application.mY}};

  face_t *new_cUsed = NULL, *new_cFree = NULL;

  new_cUsed = find_current_selected(d20.faces, &m);
  if(d20.available != NULL)
  {
    new_cFree = find_current_selected(d20.available, &m);
  }



  d20.current_free = new_cFree;
  d20.current_used = new_cUsed;

  DEBUG(2, "Finished selecting.")
}




void adjust_root_coordinate (double *coordinate, double *error, int reference, int limit)
{
  if (coordinate != error)
  {
    *coordinate = limit + ((reference - *error) + (*coordinate - reference));
  }
}

int limit_root_triangle (triangle_t *r, SDL_Rect *R)
{
  double *lX = &r->pts[0]->pts[0], *lY = &r->pts[0]->pts[1],
      *hX = &r->pts[0]->pts[0], *hY = &r->pts[0]->pts[1];
  if (*lX > r->pts[1]->pts[0])
    {lX = &r->pts[1]->pts[0];}
  if (*lY > r->pts[1]->pts[1])
    {lY = &r->pts[1]->pts[1];}
  if (*lX > r->pts[2]->pts[0])
    {lX = &r->pts[2]->pts[0];}
  if (*lY > r->pts[2]->pts[1])
    {lY = &r->pts[2]->pts[1];}
  if (*hX < r->pts[1]->pts[0])
    {hX = &r->pts[1]->pts[0];}
  if (*hY < r->pts[1]->pts[1])
    {hY = &r->pts[1]->pts[1];}
  if (*hX < r->pts[2]->pts[0])
    {hX = &r->pts[2]->pts[0];}
  if (*hY < r->pts[2]->pts[1])
    {hY = &r->pts[2]->pts[1];}

  if ( R->h < (*hY - *lY))
    {return 0;}
  if ( R->w < (*hX - *lX))
    {return 0;}


  //  lower limits
  if (*lX < R->x)
  {
    adjust_root_coordinate (&r->pts[0]->pts[0], lX, application.x, R->x);
    adjust_root_coordinate (&r->pts[1]->pts[0], lX, application.x, R->x);
    adjust_root_coordinate (&r->pts[2]->pts[0], lX, application.x, R->x);

    *lX = R->x;
  }
  if (*lY < R->y)
  {
    adjust_root_coordinate (&r->pts[0]->pts[1], lY, application.y, R->y);
    adjust_root_coordinate (&r->pts[1]->pts[1], lY, application.y, R->y);
    adjust_root_coordinate (&r->pts[2]->pts[1], lY, application.y, R->y);

    *lY = R->y;
  }
  // upper limits
  if (*hX > R->x + R->w)
  {
    adjust_root_coordinate (&r->pts[0]->pts[0], hX, application.x, R->x + R->w);
    adjust_root_coordinate (&r->pts[1]->pts[0], hX, application.x, R->x + R->w);
    adjust_root_coordinate (&r->pts[2]->pts[0], hX, application.x, R->x + R->w);

    *hX = R->x + R->w;
  }
  if (*hY > R->y + R->h)
  {
    adjust_root_coordinate (&r->pts[0]->pts[1], hY, application.y, R->y + R->h);
    adjust_root_coordinate (&r->pts[1]->pts[1], hY, application.y, R->y + R->h);
    adjust_root_coordinate (&r->pts[2]->pts[1], hY, application.y, R->y + R->h);

    *hY = R->y + R->h;
  }
  return 1;
}


void standard_root (triangle_t *dst)
{
  double radi = application.radius;
  double half = 0.5;
  double root3 = sqrt(3.0);

  double aX = radi, aY = radi,
      bX = radi, bY = radi,
      cX = radi, cY = radi;

  switch (application.rotation)
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
  ptr->pts[0] = aX + application.x;
  ptr->pts[1] = aY + application.y;
  ptr = dst->pts[1];
  ptr->pts[0] = bX + application.x;
  ptr->pts[1] = bY + application.y;
  ptr = dst->pts[2];
  ptr->pts[0] = cX + application.x;
  ptr->pts[1] = cY + application.y;
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



int equal_triangles (triangle_t *t1, triangle_t *t2, double acc)
{
  int q = 0;
  {
    q += equal_vertices(t1->pts[0],t2->pts[0],acc);
    q += equal_vertices(t1->pts[1],t2->pts[0],acc);
    q += equal_vertices(t1->pts[2],t2->pts[0],acc);
  }
  if (q > 0)
  {
    q += equal_vertices(t1->pts[0],t2->pts[1],acc);
    q += equal_vertices(t1->pts[1],t2->pts[1],acc);
    q += equal_vertices(t1->pts[2],t2->pts[1],acc);
  }
  if (q > 1)
  {
    q += equal_vertices(t1->pts[0],t2->pts[2],acc);
    q += equal_vertices(t1->pts[1],t2->pts[2],acc);
    q += equal_vertices(t1->pts[2],t2->pts[2],acc);
  }
  return (q == 3);
}

int triangle_exists (triangle_t *t, list_t *list, double acc)
{
  list_i *s = make_list_iterator(list);
  face_t *cur = list_iterator_next(s);
  int q = 0;
  while (cur != NULL)
  {
    if (equal_triangles(cur->item, t, acc))
    {
      ++ q;
      break;
    }
    cur = list_iterator_next(s);
  }
  
  return q;
}


vtx2d_t *position_that_neighbors(face_t *f, char k)
{
  vtx2d_t *r = NULL;
  triangle_t *t = f->item;
  switch (k)
  {
    case 'A':
      r = find_vector_opposing (t->pts[1], t->pts[2], t->pts[0]);
      break;
    case 'B':
      r = find_vector_opposing (t->pts[2], t->pts[0], t->pts[1]);
      break;
    case 'C':
      r = find_vector_opposing (t->pts[0], t->pts[1], t->pts[2]);
      break;
  }

  return r;
}

face_t *create_root_neighbor (char k)
{
  face_t *anchor = d20.current_free, *t = face_that_neighbors(anchor, k);
  triangle_t *q = anchor->item;
  vtx2d_t *tmp_pos = position_that_neighbors(anchor, k);

  if (SDLRect_contains(tmp_pos, &draw_area))
  {

    list_insert(application.positions, tmp_pos);
    
    triangle_t *p = malloc (sizeof(triangle_t));
    switch (k)
    {
      case 'A':
        p->pts[0] = tmp_pos;
        p->pts[1] = q->pts[1];
        p->pts[2] = q->pts[2];
        break;
      case 'B':
        p->pts[1] = tmp_pos;
        p->pts[2] = q->pts[2];
        p->pts[0] = q->pts[0];
        break;
      case 'C':
        p->pts[2] = tmp_pos;
        p->pts[0] = q->pts[0];
        p->pts[1] = q->pts[1];
        break;
    }
    t->item = p;
    if (debug >= 1)
      {print_face(t);}
  }
  else
  {
    free(tmp_pos);
    free(t);
    t = NULL;
  }
  return t;
}




void pin_root (void)
{
  void insert (face_t *ptr)
  {
    if (d20.available == NULL)
      {d20.available = make_list ();}

    list_insert(d20.available, (void *) ptr);
  }

  if (d20.faces == NULL)
  {
    d20.faces = make_list ();
    list_insert(d20.faces, d20.current_free);
  }

  face_t *ptr = NULL;
  char c = 'A';
  do
  {
    ptr = create_root_neighbor(c);
    if (ptr != NULL)
      {insert(ptr);}
    ++c;
  }
  while(c <= 'C');

  add_lines_for(&application.lines, d20.current_free->item);

  d20.current_used = NULL;
  d20.current_free = NULL;
  DEBUG(1, "Root pinned.\n");
}



face_t *neighbor_for_slot (face_t *p, char k)
{
  char err = 0;
  face_t *t = face_that_neighbors(p, k);
  err |= (t == NULL);

  if (err)
    {return t;}
  if (debug >= 1)
  {
    printf("trying to add neighbor:  ");
    print_face(t);
  }
  t->item = NULL;

  int q = face_exists_in(t, d20.faces);
  err |= (q);

  if (err)
  {
    if (debug >= 1)
    {printf("already pinned\n");}

    free(t);
    return NULL;
  }
  if (debug >= 1)
    {LINE}

  vtx2d_t *tmp_pos = position_that_neighbors(p, k);
  triangle_t *pt = p->item;
  if (debug >= 1)
    {LINE}

  if (!SDLRect_contains(tmp_pos, &draw_area))
  {
    printf("outside draw area\n");
    free (tmp_pos);
    free (t);
    return NULL;
  }
  if (debug >= 1)
    {LINE}

  vtx2d_t *pos = position_exists(tmp_pos, application.positions);
  if (pos != NULL)
  {
    free (tmp_pos);
    tmp_pos = pos;

    if (debug >= 1)
    {printf("used existing position %p\n", pos);}
  }
  if (debug >= 1)
    {LINE}

  triangle_t *tr = copy_triangle(pt);
  tr->pts[k-'A'] = tmp_pos;
  if (debug >= 1)
    {LINE}

  t->item = tr;
  return t;
}

void create_neighbor_triangles_for (face_t *p)
{
  char c = 'A';
  face_t *ptr = NULL;
  do
  {
    ptr = neighbor_for_slot(p, c);
    if (ptr != NULL)
    {
    if (debug >= 1)
    {
      LINE
      printf("checking if triangle exists on screen already\nslot is %p\ntriangle is %p\nlist is %p\n",ptr, ptr->item, d20.available);
      }
      int ex = triangle_exists(ptr->item, d20.available, FLOAT_ACC);
      if (debug >= 1)
        {LINE}
      if (!ex)
      {
        if (debug >= 1)
          {LINE}

        list_insert(d20.available, (void *) ptr);

        DEBUG(1, "Added neighbor")

      }
      else
      {

        DEBUG(1, "Triangle seems to exist on screen already.");
        //  this case seemingly never occurs
        free(ptr->item);
        free(ptr);
        if (debug >= 1)
          {LINE}
      }
    }
    ++ c;
    if (debug >= 1)
      {LINE;}
    
      
  }
  while (c <= 'C');

}


void remove_faces_similar_to (list_t *unpinned, face_t *cmp)
  //  remove duplicates
{
  int k = list_size (unpinned), q = k;
  if ((unpinned == NULL) || (k == 1))
    {return;}

  DEBUG(1, "Trying to remove duplicates")
  list_i *s = make_list_iterator(unpinned);
  face_t *cur = list_iterator_next (s);
  while (cur != NULL)
  {
      if (equal_faces(cur, cmp))
      {
        if (cur != cmp)
        {
          DEBUG(2, "\t- removed a duplicate");
          
          list_remove(unpinned, cur);
          free(cur->item);
          free(cur);
        }
      }
    cur = list_iterator_next (s);
  }
  

  free(s);
  if (debug >= 1)
  {
    k = q-list_size(unpinned);
    if (k)
      {printf("removed %d duplicates\n",k);}
  }
}



void pin (void)
{
  if (debug >= 1)
  {
    printf("####\n");
    printf("PINNING\nnum pinned : %d\nthe face in question is: ", list_size(d20.faces));
  }
  face_t *anchor = d20.current_free;
  d20.current_free = NULL;

  if (debug >= 1)
    {LINE}

  remove_faces_similar_to (d20.available, anchor);

  list_remove(d20.available, anchor);


  //  insert into list of placed triangles
  list_insert(d20.faces, anchor);
  
  if (debug >= 1)
    {LINE}

  add_lines_for (&application.lines, anchor->item);

  if (debug >= 1)
    {LINE}

  create_neighbor_triangles_for (anchor);
  if (debug >= 1)
    {LINE}
}




void app_main (void)
{
  if (d20.faces == NULL)
  { //  root triangle is not pinned
    if (mouse_moves())
    {
      application.x = application.mX;
      application.y = application.mY;
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
            {rotate_root(-scroll);}
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
          {application.being_relocated --;}
        else
        {
          application.being_relocated ++;
          application.diff_x = application.x - application.mX;
          application.diff_y = application.y - application.mY;
        }
      }
      else
      {
        if (d20.current_free != NULL)
        {
          if (mouse_on_draw_area())
          {
            pin();
            if (list_size(d20.faces) == 20)
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
          if (list_size(d20.faces) == 1)
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
    {mouse_position (&application.mX, &application.mY);}

  if (application.status == APP_MAIN)
  {
    app_main();
  
    if (application.reset_btn != NULL)
      {button_update(application.reset_btn);}

  }
  else if (application.status == APP_END)
  {
    if (application.colour_select != NULL)
    {
      fader_update(application.colour_select);
      application.negate_rate /= 100;
    }

    if (application.reset_btn != NULL)
      {button_update(application.reset_btn);}

    if (application.save_btn != NULL)
      {button_update(application.save_btn);}

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





