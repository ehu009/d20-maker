#include <stdlib.h>

#include "chain.h"



/*
 *  Ring-linked list link data type
 */

struct chain_link
{
  struct chain_link *next, *prev;
  void *item;
};

struct chain_link *new_link (void *item)
{ //  Create a link that can compose part of a ring-linked list
  struct chain_link *ptr = calloc (1, sizeof (struct chain_link));
  if (ptr != NULL)
  {
    ptr->item = item;
  }
  return ptr;
}


/*
 *  Ring-linked list data type
 */

struct linked_chain
{
  unsigned links;
  struct chain_link *head;
};

typedef struct linked_chain chain_t;


/*
 *  Ring-linked list functionality
 */

chain_t *make_chain (void *start_item)
{ //  Create a ring-linked list having one starting item
  chain_t *chain = calloc (1, sizeof (chain_t));
  if (chain != NULL)
  {
    struct chain_link *link = new_link (start_item);
    if (link == NULL)
    {
      free (chain);
      chain = NULL;
    }
    else
    {
      chain->head = link;
      link->prev = link;
      link->next = link;
      chain->links ++;
    }
  }
  return chain;
}

void free_chain (chain_t *chain)
{ //  Destroy a chain and all of its links
  //    does not destroy added items
  if (chain == NULL)
    return;
  struct chain_link *ptr, *link = chain->head;
  int n = chain->links;
  while (n)
  {
    ptr = link->next;
    free (link);
    link = ptr;
    -- n;
  }
  free (chain);
}

int chain_size (chain_t *chain)
{ //  Get length of ring-linked list
  return chain->links;
}










/*
 *  Ring-linked list manipulator data type
 */

struct chain_iterator
{
  chain_t *linked_chain;
  struct chain_link *current;
};

typedef struct chain_iterator chainslider_t;


/*
 *  Ring-linked list manipulator functionality
 */
chainslider_t *make_chainslider (chain_t *linked_chain)
{ //  Create ring-linked list manipulator
  chainslider_t *slider = calloc (1, sizeof (chainslider_t));
  if (slider != NULL)
  {
    slider->linked_chain = linked_chain;
    slider->current = linked_chain->head;
  }
  return slider;
}

void free_chainslider (chainslider_t *slider)
{ //  Destroy ring-linked list manipulator
  free (slider);
}


/*
 *  Move manipulator along ring-linked list
 */

void slider_recede (chainslider_t *slider)
{
    slider->current = slider->current->prev;
}

void slider_procede (chainslider_t *slider)
{
    slider->current = slider->current->next;
}


/*
 *  Read data from ring-linked list manipulator
 */

void *slider_current (chainslider_t *slider)
{
  return slider->current->item;
}


/*
 * Insertion / removal of items to / from a ring-linked list using manipulator
 */

  #define LOOP struct chain_link
int slider_insert_before (chainslider_t *chain_iterator, void *item)
{

  LOOP *link = new_link (item);
  if (link == NULL)
    return -1;

  LOOP *c = chain_iterator->current, *p = c->prev;

  link->next = c;

  if (p != c)
  {
    c->prev->next = link;
    link->prev = c->prev;
  }
  else
  {
    c->next = link;

    link->prev = c;
  }
  c->prev = link;

  chain_iterator->linked_chain->links ++;
  return 0;
}

int slider_insert_after (chainslider_t *slider, void *item)
{

  LOOP *link = new_link (item);
  if (link == NULL)
    return -1;

  LOOP *c = slider->current, *n = c->next;

  link->prev = c;

  if (n != c)
  {
    c->next->prev = link;
    link->next = c->next;
    c->next = link;
  }
  else
  {
    c->next = link;
    link->next = c;
    c->prev = link;
  }
  slider->linked_chain->links ++;
  return 0;
}

void slider_remove_prev (chainslider_t *slider)
{
  LOOP *c = slider->current, *p = c->prev;

  if (p == c)
    return;
  if (p == slider->linked_chain->head)
    slider->linked_chain->head = p->prev;
  c->prev = p->prev;
  c->prev->next = c;


  free (p);
  slider->linked_chain->links --;
  return;

}

void slider_remove_next (chainslider_t *slider)
{
  LOOP *c = slider->current, *n = c->next;

  if (n == c)
    return;
  if (n == slider->linked_chain->head)
    slider->linked_chain->head = c;
  n->next->prev = c;
  c->next = n->next;
  free (n);
  slider->linked_chain->links --;
  return;

}
