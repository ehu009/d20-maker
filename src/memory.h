#ifndef MEMORY_H_
  #define MEMORY_H_

  #ifdef _WIN32
    #include <malloc.h>
  #endif
  #include <stdio.h>

  static int bytes_allocated = 0;

  void *_malloc(size_t size, const char *file);
  void *_calloc(size_t size, const char *file);

  void _free(void *ptr);

  #define MALLOC(x) _malloc(x, __FILE__);
  #define CALLOC(x) _calloc(x, __FILE__);
  #define FREE(x) _free(x);

#endif