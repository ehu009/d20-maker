#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#ifdef _WIN32
  #include <malloc.h>
#endif

#include "colours.h"
#include "storage.h"
#include "sha256.h"

#define OUTPUT_DIR_PATH "output/"


int name_exists (char *name)
{
  int found = 0;
  struct dirent *e = NULL;
  DIR *d = opendir(OUTPUT_DIR_PATH);
  do
  {
    e = readdir(d);
    if (e == NULL)
    {
      break;
    }
    int s = strcmp(e->d_name, name);
    found |= (s == 0);
  }
  while (!found);
  return found;
}


char hex_from_halfbyte (char c)
{
  char r = c + 0x30;
  if (c >= 0xA)
  {
    r += 0x41;
  }
  return r;
}


short hex_from_byte (char c)
{
  short A, B;
  A = c / 0x010;
  B = c % 0x010;

  A = hex_from_halfbyte(A);
  B = hex_from_halfbyte(B);

  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    A <<= 8;
  #else
    B <<= 8;
  #endif
  return A | B;
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

  char *name = CALLOC(1, 4 + 3 + 3 + 4 + 1);

  sprintf(name, "%X.bmp", hash);

  char c = 0;
  while (name_exists(name))
  {
    if (c == 999)
    {
      FREE(name);
      name = NULL;
      break;
    }
    ++ c;
    sprintf(name, "%X (%d).bmp", hash, c);
  }

  return name;
}


void store_subsurface (SDL_Surface *surface, SDL_Rect *rect)
{

  {
    DIR *d = opendir(OUTPUT_DIR_PATH);
    if (d == NULL)
    {
      system("mkdir output\0");
    }
    closedir(d);
  }

  SDL_Surface *surf = NULL;
  {
    SDL_PixelFormat *f = surface->format;
    surf = SDL_CreateRGBSurface(0, rect->w, rect->h, 32, f->Rmask, f->Gmask, f->Bmask, alpha_mask);
    SDL_BlitSurface(surface, rect, surf, NULL);
  }

  char path[FILENAME_MAX + strlen(OUTPUT_DIR_PATH)];
  memset(path, '\0', FILENAME_MAX + strlen(OUTPUT_DIR_PATH));

  char *buf = generate_name(surf);

  strcat(path, OUTPUT_DIR_PATH);
  strcat(path, buf);

  SDL_SaveBMP(surf, path);

  SDL_FreeSurface(surf);
  FREE(buf);
}
