#ifndef MEMORY_H_
  #define MEMORY_H_

  #ifdef _WIN32
    #include <malloc.h>
  #else
    #include <stdlib.h>
  #endif
  
  #include "debug.h"

  
  void init_memory(void);
  void free_memory(void);
  void memory_report(void);

  #define MALLOC(x) (debug >= 1) ?\
      _malloc(x, __FILE__, __LINE__) :\
      malloc(x);
  #define CALLOC(y, x) (debug >= 1) ?\
      _calloc(y, x, __FILE__, __LINE__) :\
      calloc(y, x);
  #define FREE(x) (debug >= 1) ?\
      _free(x, __FILE__, __LINE__) :\
      free(x);



  void *_malloc(size_t size, const char *file, int line);
  void *_calloc(size_t num, size_t size, const char *file, int line);
  void _free(void *ptr, const char *file, int line);

#endif