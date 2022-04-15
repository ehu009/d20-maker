#include <stdlib.h>
#include "leaks.h"

#ifdef _WIN32
#include <malloc.h>
#endif

#include <stdio.h>



static char *output = NULL;
static int head, blocks;

#define OUTPUT_BLOCK_SIZE  512


int len(char *s)
{
  char *p = s;
  while (*p != '\0')
  {
    ++p;
  }
  return p - s;
}

char *extend(char *str, int num_bytes)
{
  int blocks_total = head + num_bytes;
  blocks_total /= OUTPUT_BLOCK_SIZE;
  blocks = blocks_total;
  char *new = malloc(blocks*OUTPUT_BLOCK_SIZE);
  char *src = str, *dst = new;
  int c = 0;
  while (c < head)
  {
    dst[c] = src[c];
    ++c;
  }
  dst[head] = '\0';
  return new;
}




void init_leak_check(void)
{
  output = calloc(1,OUTPUT_BLOCK_SIZE);
  head = 0;
  blocks = 1;
}

char *leak_check(void)
{
  printf("blocks: %d\r\n", blocks);
  return output;
}



void write(char *s)
{
  
  int l = len(s);
  if (l + head > blocks * OUTPUT_BLOCK_SIZE)
  {
    char *tmp = output;
    output = extend(output, l);
    free(tmp);
    ++ blocks;
  }
  int r = l;
  char *src = s, *dst = &output[head];
  while (r>0)
  {
    *dst = *src;
    ++dst;
    ++src;
    --r;
  }
  head += l;
  output[head] = '\0';
}


#define LENGTH_NUM      5   //  assuming allocated chunks has no more than 5 digits worth of bytes
#define LENGTH_POINTER  14
#define LENGTH_MIN      5 + LENGTH_POINTER // includes one letter, a space, new line, and end byte

void *fake_malloc(int size)
{
  void *ptr = malloc(size);
  
  char *str = calloc(1, 1+LENGTH_MIN + LENGTH_NUM);
  sprintf(str, "A %p %d\r\n", ptr, size);
  write(str);
  free(str);
  
  return ptr;
}

void *fake_calloc(int n, int size)
{
  void *ptr = calloc(n,size);

  char *str = calloc(1, 1+LENGTH_MIN + LENGTH_NUM);
  sprintf(str, "A %p %d\r\n", ptr, size);
  write(str);
  free(str);

  return ptr;
}

void fake_free(void *pointer)
{
  char *str = calloc(1, LENGTH_MIN);
  sprintf(str, "F %p\r\n", pointer);
  write(str);
  free(str);
  free(pointer);
}
