#ifndef     TOGGLE_H_
#define     TOGGLE_H_


#include  "SDL2/SDL_surface.h"

#include  "mouse_app.h"

extern SDL_Surface *canvas;


struct toggle_switch;

typedef struct toggle_switch toggler_t;


toggler_t *toggler_create (const char *txt1, const char *txt2, unsigned *value);

void toggler_free (toggler_t *toggler);

void toggler_setPosition (toggler_t *button, unsigned _x, unsigned _y);

void toggler_update (toggler_t *button);

void toggler_draw (toggler_t *button);

#endif    /*  TOGGLE_H_ */
