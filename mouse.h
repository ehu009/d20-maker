#include "SDL2/SDL.h"


//	not to be touch externally
extern int mouseX, mouseY;
extern unsigned char mouse_state;

//	checks mouse state
int mouse_is_down(void);
int mouse_is_up(void);
int mouse_is_held(void);



/*
 *	controls state
 */
 
//	call on successfully popping event from queue
void mouse_update (SDL_Event *e);

//	call when event queue is empty
void mouse_reset(void);

