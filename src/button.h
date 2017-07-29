#ifndef     BUTTON_H_
#define     BUTTON_H_


#include  "SDL2/SDL_surface.h"

#include  "mouse_app.h"

extern SDL_Surface *canvas;


struct button_switch;

typedef struct button_switch button_t;


button_t *button_create (const char *txt, unsigned *value);

void button_free (button_t *button);

void button_setPosition (button_t *button, unsigned _x, unsigned _y);

void button_update (button_t *button);

void button_draw (button_t *button);

#endif    /*  BUTTON_H_ */
