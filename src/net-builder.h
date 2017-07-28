

#include    "SDL2/SDL.h"

#include <stdio.h>

#include "lines.h"
#include "mouse_app.h"


extern SDL_Surface *canvas, *src_image;

#define FADER_WIDTH 40

#define BORDER_SIZE 7


#define SIDE_BAR_W  FADER_WIDTH + 3*BORDER_SIZE
#define SIDE_BAR_H  0 + 2*BORDER_SIZE

void app_start (void);
void app_free (void);


void app_usage (void);
void app_draw (void);
