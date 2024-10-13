#include "memory.h"


typedef struct {
  size_t size;
  void *ptr;
} alloc_t;

void enter(size_t size, void *ptr)
{
  alloc_t *a = calloc(1, sizeof(alloc_t));
  a->size = size;
  a->ptr = ptr;
  list_insert(allocations, a);
}


void init_memory(void)
{
  allocations = make_list();
}

void free_memory(void)
{
  free_list(allocations);
}

void *_malloc(size_t size, const char *file)
{
  void *ptr = malloc(size);
  enter(size, ptr);
  return ptr;
}

void *_calloc(size_t num, size_t size, const char *file)
{
  void *ptr = calloc(num, size);
  enter(size, ptr);
  return ptr;
}

void _free(void *ptr)
{
  list_i *iter = make_list_iterator(allocations);
  alloc_t *cur = list_iterator_next(iter);
  while (cur != NULL)
  {
    if (cur->ptr == ptr)
    {
      break;
    }
    cur = list_iterator_next(iter);
  }
  enter(-cur->size, ptr);
}

void memory_report(void)
{
  int allocated = 0;

  list_i *iter = make_list_iterator(allocations);
  alloc_t *cur = list_iterator_next(iter);
  while (cur != NULL)
  {
    allocated += cur->size;
    cur = list_iterator_next(iter);
  }
  printf("allocated bytes remaining: %i\r\n", allocated);
  list_empty(allocations);
}