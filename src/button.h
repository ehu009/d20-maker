#ifndef   BUTTON_H_
  #define BUTTON_H_

  #include  "SDL2/SDL_surface.h"


  extern SDL_Surface *canvas;


  typedef struct button_switch button_t;


  button_t *make_button (const char *txt, unsigned *value);
  void free_button (button_t *button);

  void button_reposition (button_t *button, unsigned _x, unsigned _y);

  void button_update (button_t *button);
  void button_draw (button_t *button);


#endif    /*  BUTTON_H_ */