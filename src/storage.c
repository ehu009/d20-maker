#include <dirent.h>
#include <sys/ftypes.h>
#include <string.h>

#include "storage.h"

#define OUTPUT_DIR_PATH "src/\0"





int name_exists (char *name)
{
  int found = 0;
  struct dirent *e = NULL;
  DIR *d = opendir(OUTPUT_DIR_PATH);
  do
  {
    e = readdir(d);
    found |= (0 == strcmp(e->d_name, name));
  }
  while (e != NULL);
  return found;
}


void generate_name (char *buf, SDL_Surface *surface)
{


}


void store_subsurface (SDL_Surface *surface, SDL_Rect *rect)
{


}
