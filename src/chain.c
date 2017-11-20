#include <stdlib.h>
#ifdef _WIN32
  #include <malloc.h>
#endif

#include "SDL2/SDL.h"
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
  struct chain_link *ptr = (struct chain_link *) calloc (1, sizeof (struct chain_link));
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
  SDL_mutex *lock;
  unsigned links;
  struct chain_link *head, *tail;
};

typedef struct linked_chain chain_t;


/*
 *  Ring-linked list functionality
 */

void add_head_link (struct chain_link *link, chain_t *chain)
{
  chain->head = link;
  chain->tail = link;
  link->prev = link;
  link->next = link;
}
/*
chain_t *make_chain (void *start_item)
{ //  Create a ring-linked list having one starting item
  chain_t *chain = (chain_t *) calloc (1, sizeof (chain_t));
  if (chain != NULL)
  {
    struct chain_link *link = new_link (start_item);
    if (link == NULL)
    {
      free ((void *) chain);
      chain = NULL;
    }
    else
    {
      chain->lock = SDL_CreateMutex();
      if (!chain->lock)
      {
        free ((void *) link);
        free ((void *) chain);
        chain = NULL;
      }
      add_head_link(link, chain);
      chain->links ++;
    }
  }
  return chain;
}
*/
chain_t *make_chain (void)
{ //  Create a ring-linked list having no starting items
  chain_t *chain = (chain_t *) calloc (1, sizeof(chain_t));
  if (chain != NULL)
  {
    chain->lock = SDL_CreateMutex();
    if (chain->lock == NULL)
    {
      free ((void *) chain);
      printf("init: lock not NULL\n");
      chain = NULL;
    }
    else
    {
      chain->head = NULL;
      chain->tail = NULL;
      chain->links = 0;
    }
  }
  return chain;
}

void free_chain (chain_t *chain)
{ //  Destroy a chain and all of its links
  //    does not destroy added items
  if (chain == NULL)
    return;
  if (!SDL_LockMutex(chain->lock))
  {
    struct chain_link *ptr, *link = chain->head;
    int n = chain->links;
    while (n)
    {
      ptr = link->next;
      free ((void *) link);
      link = ptr;
      -- n;
    }
    SDL_UnlockMutex(chain->lock);
    SDL_DestroyMutex(chain->lock);
  }
  free ((void *) chain);
}

int chain_size (chain_t *chain)
{ //  Get length of ring-linked list
  int r = -1;
  if (!SDL_LockMutex(chain->lock))
  {
    r = chain->links;
    SDL_UnlockMutex(chain->lock);
  }
  return r;
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
  free ((chainslider_t *) slider);
}


/*
 *  Move manipulator along ring-linked list
 */

void slider_recede (chainslider_t *slider)
{
  int s = chain_size(slider->linked_chain);
  if (s <= 1)
    return;
  if (!SDL_LockMutex(slider->linked_chain->lock))
  {
    slider->current = slider->current->prev;
    SDL_UnlockMutex(slider->linked_chain->lock);
  }
}

void slider_procede (chainslider_t *slider)
{
  int s = chain_size(slider->linked_chain);
  if (s <= 1)
    return;
  if (!SDL_LockMutex(slider->linked_chain->lock))
  {
    slider->current = slider->current->next;
    SDL_UnlockMutex(slider->linked_chain->lock);
  }
}


/*
 *  Read data from ring-linked list manipulator
 */

void *slider_current (chainslider_t *slider)
{
  void *ptr = NULL;
  if (slider->current == NULL)
    return ptr;
  if (!chain_size(slider->linked_chain))
    return ptr;
  if (!SDL_LockMutex(slider->linked_chain->lock))
  {

    ptr = slider->current->item;
    SDL_UnlockMutex(slider->linked_chain->lock);
  }
  return ptr;
}


/*
 * Insertion / removal of items to / from a ring-linked list using manipulator
 */


  #define LOOP struct chain_link
int slider_insert_before (chainslider_t *slider, void *item)
{
  chain_t *ch = slider->linked_chain;
  LOOP *link = new_link (item);
  if (link == NULL)
    return -1;
  int s = chain_size(ch);
  if (!SDL_LockMutex(ch->lock))
  {
    if (s)
    {

      LOOP *c = slider->current, *p = c->prev;

      link->next = c;

      if (p != c)
      {
        p->next = link;
        link->prev = p;
      }
      else
      {
        c->next = link;
        link->prev = c;
      }
      c->prev = link;
    }
    else
    {
      add_head_link(link, ch);
      slider->current = link;
    }
    fflush(stdout);
    ++ch->links;
    SDL_UnlockMutex(ch->lock);
  }
  else
  {
  printf("couldn't lock mutex?? %s\n", SDL_GetError());

  }
  return 0;
}

int slider_insert_after (chainslider_t *slider, void *item)
{

  LOOP *link = new_link (item);
  if (link == NULL)
    return -1;
  int s = chain_size(slider->linked_chain);
  if (!SDL_LockMutex(slider->linked_chain->lock))
  {
    if (s)
    {
      LOOP *c = slider->current, *n = c->next;

      link->prev = c;

      if (n != c)
      {
        n->prev = link;
        link->next = n;
      }
      else
      {
        link->next = c;
        c->prev = link;
      }
      c->next = link;
    }
    else
    {
      add_head_link(link, slider->linked_chain);
      slider->current = link;
    }
    ++slider->linked_chain->links;

    SDL_UnlockMutex(slider->linked_chain->lock);
  }
  return 0;
}

void slider_remove_prev (chainslider_t *slider)
{
  int s = chain_size(slider->linked_chain);
  if (!s)
    return;
  if (!SDL_LockMutex(slider->linked_chain->lock))
  {
    if (s == 1)
    {
      free(slider->linked_chain->head);
      slider->linked_chain->head = NULL;
      slider->linked_chain->tail = NULL;
      slider->current = NULL;
    }
    else
    {
      LOOP *c = slider->current, *p = c->prev;

      if (p == slider->linked_chain->head)
        slider->linked_chain->head = c;
      if (p == slider->linked_chain->tail)
        slider->linked_chain->tail = c;
      c->prev = p->prev;
      c->prev->next = c;

      free ((void *) p);
    }
    slider->linked_chain->links --;
    SDL_UnlockMutex(slider->linked_chain->lock);
  }
  return;

}

void slider_remove_next (chainslider_t *slider)
{
  int s = chain_size(slider->linked_chain);
  if (!s)
    return;
  if (!SDL_LockMutex(slider->linked_chain->lock))
  {
    if (s == 1)
    {
      free(slider->linked_chain->head);
      slider->linked_chain->head = NULL;
      slider->linked_chain->tail = NULL;
      slider->current = NULL;
    }
    else
    {
      LOOP *c = slider->current, *n = c->next;
      if (n == slider->linked_chain->head)
        slider->linked_chain->head = c;
      if (n == slider->linked_chain->tail)
        slider->linked_chain->tail = c;
      n->next->prev = c;
      c->next = n->next;
      free ((void *) n);
    }
    slider->linked_chain->links --;
    SDL_UnlockMutex(slider->linked_chain->lock);
  }
  return;

}
