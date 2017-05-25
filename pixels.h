#ifndef   PIXELS_H_

  #define PIXELS_H_

    #include    "SDL2/SDL.h"

    #include    "vertex.h"

    #define   COLOR  uint32_t

    //  change/read pixel values at a point on a surface

    COLOR getPixel (SDL_Surface *src, vtx2_t *p);

    void setPixel(SDL_Surface *dst, vtx2_t *p, COLOR color);


#endif  /*  PIXELS_H_ */
