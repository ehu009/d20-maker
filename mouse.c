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

/*
enum
{
	m_press,
	m_hold,
	m_release,
	m_checkout
};

#define	MASK (value)	(1 << value)


static int mouseState = 0;
#define GET_STATE (value)		(mouseState & MASK(value))
#define FLIP_STATE (value)	(mouseState ^= MASK(value))
*/



void mouse_handle_down (SDL_Event *e)
{
	if (mouse_up)
	{
		mouse_up = 0;
	}
	if (mouse_down)
	{
		mouse_hold = 1;
		mouse_down = 0;
	}
	else
	{
		if (mouse_hold == 0)
		{
			mouse_down = 1;
		}
	}
}

void mouse_handle_up (SDL_Event *e)
{
	/*if (GET_STATE(m_checkout))
	{
		if (GET_STATE(m_press))
		{
			printf("It releases\n");
		}
		else
		{
			
		}
	}*/
	
	if (mouse_down || mouse_hold)
	{
		mouse_down = 0;
		mouse_hold = 0;
	}
	mouse_up = 1;
}

