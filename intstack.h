#ifndef   INTSTACK_H_
  #define INTSTACK_H_

  struct integer_stack;

  typedef struct integer_stack intstack_t;


  intstack_t *make_integer_stack (int max);
  void free_stack (intstack_t *integer_stack);

  int stack_height (intstack_t *integer_stack);

  int stack_peek (intstack_t *integer_stack, char *err_buf);
  int stack_pop (intstack_t *integer_stack, char *err_buf);
  int stack_push (intstack_t *integer_stack, char *err_buf, int item);

#endif  /*  INTSTACK_H_ */
