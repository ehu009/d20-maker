#ifndef   LISTS_H_
#define   LISTS_H_


struct linked_list;
typedef struct linked_list list_t;

list_t *make_list(void);
void free_list(list_t *list);

int list_size(list_t *list);
int list_insert(list_t *list, void *item);
void list_remove(list_t *list, void *item);


struct linked_list_iterator;
typedef struct linked_list_iterator list_i;

list_i *make_list_iterator(list_t *list);
void free_list_iterator(list_i *iterator);

void *list_iterator_next(list_i *iterator);


#endif    /*  LISTS_H_ */
