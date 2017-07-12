#ifndef   STORAGE_H_
  #define   STORAGE_H_

  #include "SDL2/SDL_rect.h"
  #include "SDL2/SDL_surface.h"


  /*
   *  Stores a subsurface to a unique path
   */

   void store_subsurface (SDL_Surface *surface, SDL_Rect *rect);

#endif
