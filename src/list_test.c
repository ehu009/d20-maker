#include <stdlib.h>
#include <stdio.h>
#include "leaks.h"

int main(int argc, char *argv[])
{
  
  init_leak_check();
  
  void *bais = fake_malloc(16);
  
  char *tmp = fake_calloc(1,700), *tmp2 = fake_malloc(23);
  
  fake_free(tmp);
  fake_free(bais);
  fake_free(tmp2);
  
  char *ptr = leak_check();
  
  
  printf("output: \"%s\"\r\n",ptr);
  
  free(ptr);

  return 0;
}
