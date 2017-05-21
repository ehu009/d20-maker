

#include    "SDL2/SDL.h"

#include <stdio.h>

#include "lines.h"
#include "mouse_app.h"

#include "drawn-triangles.h"


extern SDL_Surface *canvas, *src_image;


void app_start (void);
void app_free (void);


void app_usage (void);
void app_draw (void);
