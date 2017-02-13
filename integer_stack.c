#include <stdlib.h>
#include "intstack.h"

struct integer_stack
{
  int max, pos;
  int *arr;
};




intstack_t *make_integer_stack (int max)
{
  intstack_t *integer_stack = calloc (1, sizeof (intstack_t));

  if (integer_stack != NULL)
  {
    int *array = calloc (max, sizeof (int));
    if (array != NULL)
    {
      integer_stack->arr = array;
      integer_stack->pos = -1;
      integer_stack->max = max;
    }
    else
    {
      free (integer_stack);
      integer_stack = NULL;
    }
  }
  return integer_stack;
}


int stack_height (intstack_t *integer_stack)
{
  return integer_stack->pos + 1;
}

void free_stack (intstack_t *integer_stack)
{
  free (integer_stack->arr);
  free (integer_stack);
}



int stack_peek (intstack_t *integer_stack, char *err_buf)
{
  *err_buf = 1;
  int *p = &integer_stack->pos;
  if (*p < 0)
    return 0;
  *err_buf = 0;
  return integer_stack->arr[*p];
}

int stack_pop (intstack_t *integer_stack, char *err_buf)
{
  *err_buf = 1;
  int p = integer_stack->pos;
  if (p < 0)
    return 0;
  int *slot = &integer_stack->arr[p];
  integer_stack->pos --;
  int item = *slot;
  *slot = 0;
  *err_buf = 0;
  return item;
}

int stack_push (intstack_t *integer_stack, char *err_buf, int item)
{
  *err_buf = 1;
  int *p = &integer_stack->pos;
  if (*p + 1 >= integer_stack->max)
    return 0;
  ++ *p;
  integer_stack->arr[*p] = item;
  *err_buf = 0;
  return 1;
}
