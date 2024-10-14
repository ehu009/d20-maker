#ifndef MEMORY_H_
  #define MEMORY_H_

  #ifdef _WIN32
    #include <malloc.h>
  #endif
  #include <stdio.h>
  
  
  void init_memory(void);
  void free_memory(void);
  void memory_report(void);

  void *_malloc(size_t size, const char *file);
  void *_calloc(size_t num, size_t size, const char *file);
  void _free(void *ptr, const char *file, int line);

  #define MALLOC(x) _malloc(x, __FILE__);
  #define CALLOC(y, x) _calloc(y, x, __FILE__);
  #define FREE(x) _free(x, __FILE__, __LINE__);


#endif