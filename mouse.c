#include "mouse.h"

#include <stdio.h>

/*
 *	Some handler prototypes
 */
void mouse_handle_move (SDL_Event *e);
void mouse_handle_down (SDL_Event *e);
void mouse_handle_up (SDL_Event *e);


void mouse_update (SDL_Event *event)
{
	switch (event->type)
	{
		case SDL_MOUSEBUTTONDOWN:
		{
			mouse_handle_down (event);
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			mouse_handle_up (event);
			break;
		}
		case SDL_MOUSEMOTION:
		{
			mouse_handle_move (event);
			break;
		}
	}
}


void mouse_handle_move (SDL_Event *e)
{
	mouseX = e->motion.x;
	mouseY = e->motion.y;
}

void mouse_handle_down (SDL_Event *e)
{
	printf("It presses\n");
}

void mouse_handle_up (SDL_Event *e)
{
	printf("It releases\n");
}

