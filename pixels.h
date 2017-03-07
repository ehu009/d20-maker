#ifndef   PIXELS_H_

  #define PIXELS_H_

    #include    "SDL2/SDL.h"

    #define   COLOR  uint32_t


    COLOR getPixel (SDL_Surface *src, int x, int y);

    void setPixel(SDL_Surface *dst, int x, int y, COLOR color);

#endif  /*  PIXELS_H_ */
