#ifndef LEAK_CHECK_H_
#define LEAK_CHECK_H_


void init_leak_check(void);

char *leak_check(void);



void *fake_malloc(int size);

void *fake_calloc(int n, int size);

void fake_free(void *pointer);


#endif
