#ifndef MEMORY_H_
  #define MEMORY_H_

  #ifdef _WIN32
    #include <malloc.h>
  #endif
  #include <stdio.h>
  
  #include "list.h"

  static list_t *allocations = NULL;
  
  void init_memory(void);
  void free_memory(void);

  void *_malloc(size_t size, const char *file);
  void *_calloc(size_t num, size_t size, const char *file);

  void _free(void *ptr);

  #define MALLOC(x) _malloc(x, __FILE__);
  #define CALLOC(x) _calloc(1, x, __FILE__);
  #define FREE(x) _free(x);

  void memory_report(void);


#endif