#include "net_slot.h"


void init_slot (struct slot *ptr)
{
  int j = 0;
  while (j <3)
  {
    ptr->positions[j] = NULL;
    j++;
  }
  ptr->pinned = 0;
  ptr->x = 0.0;
  ptr->y = 0.0;
}

int slot_index(struct slot *arr, struct slot *ptr)
{
  return ((ptr - arr)/sizeof(struct slot));
}

int slot_has_triangle(struct slot *slot, triangle_t *t)
{
  return (slot->positions[0] == t)
      ||  (slot->positions[1] == t)
      ||  (slot->positions[2] == t);
}

int slot_has_neighbor(struct slot *slot, struct slot *ptr)
{
  return (slot->A == ptr || slot->B == ptr || slot->C == ptr);
}

struct slot *neighbor_with_triangle (struct slot *s, triangle_t *t)
{
  if (slot_has_triangle (s->A, t))
    return s->A;
  else if (slot_has_triangle (s->B, t))
    return s->B;
  else if (slot_has_triangle (s->C, t))
    return s->C;
  else return NULL;
}


