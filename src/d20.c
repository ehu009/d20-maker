#include <string.h>

#include "d20.h"
#include "memory.h"

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

void init_d20 (slot_t *root)
{
  slot_t *s;
  int j = 0, k;
  for (; j < NUM_D20_VTX; j ++)
  {
    s = root + j;
    for (k = 0; k < NUM_VTX_POS; k++)
    {
      s->links[k] = root;
      s->links[k] += d20_model[j].pts[k];
    }
  }
}

slot_t *find_slot_opposing (slot_t *a1, slot_t *a2, slot_t *opposer)
{
  slot_t *r = NULL;
  int link1 = 0, link2;
  for (;  link1 < NUM_VTX_POS; link1 ++)
  {
    if (r != NULL)
    {
      break;
    }
    slot_t *cur1 = a1->links[link1];
    if (cur1 == opposer)
    {
      continue;
    }
    for (link2 = 0; link2 < NUM_VTX_POS; link2 ++)
    {
      if (r != NULL)
      {
        break;
      }
      slot_t *cur2 = a2->links[link2];
      if (cur2 == opposer)
      {
        continue;
      }
      if (cur1 == cur2)
      {
        r = cur1;
      }
    }
  }
  return r;
}


face_t *new_face (slot_t *a, slot_t *b, slot_t *c)
{
  face_t *f = MALLOC(sizeof(face_t));
  if (f != NULL)
  {
    f->sA = a;
    f->sB = b;
    f->sC = c;
  }
  return f;
}

void free_face (void*ptr)
{
  face_t *f = ptr;
  FREE(f->item);
  FREE(f);
}

int equal_faces (face_t *A, face_t *B)
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

  return (eq > 2);
}

face_t *copy_face (face_t *ptr)
{
  face_t *f = MALLOC(sizeof(face_t));
  if (f == NULL)
  {
    return f;
  }
  memcpy(f, ptr, sizeof(face_t));
  return f;
}

face_t *face_that_neighbors (face_t *f, char k)
{
  face_t *n = NULL;
  n = copy_face(f);
  if (n == NULL)
  {
    return n;
  }
  switch (k)
  {
    case 'A':
      n->sA = find_slot_opposing(f->sB, f->sC, f->sA);
      break;
    case 'B':
      n->sB = find_slot_opposing(f->sC, f->sA, f->sB);
      break;
    case 'C':
      n->sC = find_slot_opposing(f->sA, f->sB, f->sC);
      break;
  }
  return n;
}
