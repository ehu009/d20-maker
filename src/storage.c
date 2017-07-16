#include <dirent.h>
#include <string.h>
#ifdef _WIN32
  #include <malloc.h>
#endif

#include "storage.h"
#include "sha256.h"

#define OUTPUT_DIR_PATH "output/"

#include <stdlib.h>

int name_exists (char *name)
{
  int found = 0;
  struct dirent *e = NULL;
  printf("line:%d\n",__LINE__);
  fflush(stdout);
  DIR *d = opendir(OUTPUT_DIR_PATH);
  printf("line:%d\n",__LINE__);
  fflush(stdout);
  do
  {
    e = readdir(d);
    if (e == NULL)
      break;
    int s = strcmp(e->d_name, name);
    found |= (s == 0);
    printf("comparing: %s, %s -> %d\n", e->d_name, name, s);
    printf("line:%d\n",__LINE__);
  fflush(stdout);
  }
  while (!found);
  printf("line:%d\n",__LINE__);
  fflush(stdout);
  return found;
}


char hex_from_halfbyte (char c)
{
  char r = c + 0x30;
  if (c >= 0xA)
    r += 0x41;
  return r;
}


short hex_from_byte (char c)
{
  short A, B;
  A = c / 0x010;
  B = c % 0x010;


  /*
  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    A = 0b11110000;
    B = 0b00001111;
  #else
    A = 0b00001111;
    B = 0b11110000;
  #endif

  A &= c;
  B &= c;

  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    A >>= 4;
  #else
    B >>= 4;
  #endif
*/
  A = hex_from_halfbyte(A);
  B = hex_from_halfbyte(B);
/*
  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    A <<= 4;
  #else
    B <<= 4;
  #endif
*/
  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    A <<= 8;
  #else
    B <<= 8;
  #endif
  return A|B;
}


char *generate_name (SDL_Surface *surface)
{
  const char i = sizeof(int);
  int hash;
  char tmp[i];
  SHA256_CTX h;
  sha256_init(&h);
  sha256_update(&h, (unsigned char *) surface->pixels, surface->h * surface->pitch);
  sha256_final(&h, (unsigned char *) &tmp);
  memcpy(&hash, tmp, 4);

  printf("line:%d\n",__LINE__);
  fflush(stdout);
  char *name = calloc(1, 4 + 3 + 3 + 4 + 1);

  sprintf(name, "%X.bmp", hash);

printf("line:%d\n",__LINE__);
  fflush(stdout);
  char c = 0;
  while (name_exists(name))
  {
  /*  if (!c)
    { */

      /*
      buf[ext +0] = ' ';
      buf[ext +1] = '(';
      buf[ext +2] = c;
      buf[ext +3] = ')';
      ext += 4;
      */
/*    }
    else
    {
      ++ c;*/

      printf("line:%d\n",__LINE__);
  fflush(stdout);
      if (c == 999)
      {
        free(name);
        name = NULL;
        break;
        }
      /*
      buf[l + p +2] = c;
      */
  //  }
      ++ c;
      sprintf(name, "%X (%d).bmp", hash, c);
  }
  /*
  printf("line: %d\n",__LINE__);
  buf[ext +0] = '.';
  buf[ext +1] = 'b';
  buf[ext +2] = 'm';
  buf[ext +3] = 'p';

  printf("line:%d\n",__LINE__);
  fflush(stdout);
  */
//  memcpy(buf, name, strlen(name));
  printf("line:%d\n",__LINE__);
  fflush(stdout);
  return name;
}


void store_subsurface (SDL_Surface *surface, SDL_Rect *rect)
{
  SDL_Surface *surf = NULL;
  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    surf = SDL_CreateRGBSurface (0, rect->w, rect->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
  #else
    surf = SDL_CreateRGBSurface (0, rect->w, rect->h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
  #endif

  SDL_BlitSurface(surface, rect, surf, NULL);

printf("line:%d\n",__LINE__);
  fflush(stdout);
  char path[FILENAME_MAX + strlen(OUTPUT_DIR_PATH)];
  memset (path, '\0', FILENAME_MAX + strlen(OUTPUT_DIR_PATH));
  printf("line:%d\n",__LINE__);
  fflush(stdout);


  char *buf = generate_name(surf);

  printf("line:%d\n",__LINE__);
  fflush(stdout);

  strcat (path, OUTPUT_DIR_PATH);
  strcat (path, buf);

  printf("line:%d\n",__LINE__);
  fflush(stdout);

  printf("generated path: %s\n", path);
  SDL_SaveBMP(surf, path);


  SDL_FreeSurface(surf);
  free(buf);
  printf("line: %d\n",__LINE__);
}
