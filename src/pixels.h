#ifndef   PIXELS_H_
  #define PIXELS_H_

    #include    "SDL2/SDL.h"

    #include    "vertex.h"
    #include    "colours.h"


    /*
     *  Change/read pixel values at a point on a surface
     */

    COLOUR getPixel (SDL_Surface *src, vtx2i_t *p);

    void setPixel (SDL_Surface *dst, vtx2i_t *p, COLOUR color);


#endif    /*  PIXELS_H_ */