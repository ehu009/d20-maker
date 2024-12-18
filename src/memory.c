#include <stdio.h>

#include "memory.h"
#include "list.h"


static list_t *allocations = NULL;


typedef struct {
  size_t size;
  void *ptr;
} alloc_t;

void enter (size_t size, void *ptr)
{
  alloc_t *a = calloc(1, sizeof(alloc_t));
  a->size = size;
  a->ptr = ptr;
  list_insert(allocations, a);
}


void init_memory (void)
{
  allocations = make_list();
}

void free_memory (void)
{
  free_list(allocations);
}

void *_malloc (size_t size, const char *file, int line)
{
  void *ptr = malloc(size);
  printf("%s: %d, allocating %p\r\n", file, line, ptr);
  enter(size, ptr);
  return ptr;
}

void *_calloc (size_t num, size_t size, const char *file, int line)
{
  void *ptr = calloc(num, size);
  printf("%s: %d, allocating %p\r\n", file, line, ptr);
  enter(size, ptr);
  return ptr;
}

void _free (void *ptr, const char *file, int line)
{
  if (ptr == NULL)
  {
    return;
  }
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
  if (cur != NULL)
  {
    printf("%s: %d, freeing %p\r\n", file, line, ptr);
    free(cur->ptr);
    enter(-cur->size, ptr);
  }
  free(iter);
}

void memory_report (void)
{
  int allocated = 0;
  
  void free_alloc (void *ptr)
  {
    alloc_t *a = ptr;
    allocated += a->size;
  }
  list_erase(allocations, &free_alloc);

  printf("allocated bytes remaining: %i\r\n", allocated);
}