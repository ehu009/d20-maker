#ifndef   FADER_H_
  #define FADER_H_

  #include  "SDL2/SDL_surface.h"


  extern SDL_Surface *canvas;


  typedef struct fader_bar  fader_t;


  fader_t *make_fader (double start, int width, int height, double *value);
  void free_fader (fader_t *slide);

  void fader_reposition (fader_t *slide, int _x, int _y);

  void fader_draw (fader_t *slide);
  void fader_update (fader_t *slide);


#endif    /*  FADER_H_  */