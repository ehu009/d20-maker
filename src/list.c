#include <stdlib.h>
#ifdef _WIN32
  #include <malloc.h>
#endif

#include "list.h"


struct list_node;
typedef struct list_node l_node;

struct list_node
{
  l_node *next;
  void *item;
};

l_node *make_list_node(void *item)
{
  l_node *n = malloc(sizeof(l_node));
  if (n != NULL)
  {
    n->item = item;
  }
  return n;
}



struct linked_list
{
  l_node *head;
  int size;
};

list_t *make_list(void)
{
  list_t *l = malloc(sizeof(list_t));
  if (l != NULL)
  {
    l->head = NULL;
    l->size = 0;
  }
  return l;
}

void free_list(list_t *list)
{
  if (list != NULL)
  {  
    while (list->head != NULL)
    {
      l_node *next = list->head->next;
      free(list->head);
      list->head = next;
    }
  }
}

int list_size(list_t *list)
{
  if (list == NULL)
  {
    return -1;
  }
  return list->size;
} 

int list_insert(list_t *list, void *item)
{
  if (list != NULL)
  {
    l_node *node = make_list_node(item);
    if (node != NULL)
    {
      node->next = list->head;
      list->head = node;
      list->size += 1;
    }
  }
  return 1;
}

void list_remove(list_t *list, void *item)
{
  if (list == NULL)
  {
    return;
  }
  l_node **prev = &list->head;
  l_node *current = *prev;
  while(current != NULL)
  {
    int rm = 0;
    l_node *next = current->next;
    if (current->item == item)
    {
      *prev = next;
      rm = 1;
    }
    prev = &current->next;
    if (rm)
    {
      free(current);
      list->size --;
    }
    current = next;
  }
}


struct linked_list_iterator
{
  list_t *list;
  l_node *current;
};

list_i *make_list_iterator(list_t *list)
{
  list_i *iter = NULL;
  if (list != NULL)
  {
    iter = malloc(sizeof(list_i));
    if (iter != NULL)
    {
      iter->list = list;
      iter->current = list->head;
    }
  }
  return iter;
}

void free_list_iterator(list_i *iterator)
{
  free(iterator);
}

void *list_iterator_next(list_i *iterator)
{
  if (iterator != NULL)
  {
    if (iterator->current != NULL)
    {
      void *item = iterator->current->item;
      iterator->current = iterator->current->next;
      return item;
    }
  }
  return NULL;
}

