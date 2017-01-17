#ifndef   PIXELS_H_

  #define PIXELS_H_

    #include    "SDL2/SDL.h"

    uint32_t getPixel (SDL_Surface *src, int x, int y);

    void setPixel(SDL_Surface *dst, int x, int y, unsigned color);

#endif  /*  PIXELS_H_ */
