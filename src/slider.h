#ifndef     SLIDER_H_
#define     SLIDER_H_


#include  "SDL2/SDL_surface.h"


extern SDL_Surface *canvas;


struct slide_bar;

typedef struct slide_bar  slider_t;


slider_t *slider_create (double start, int width, int height, double *value);

void  slider_free (slider_t *slide);


void slider_setPos (slider_t *slide, int _x, int _y);

void slider_draw (slider_t *slide);

void slider_update (slider_t *slide);



#endif    /*  SLIDER_H_  */
