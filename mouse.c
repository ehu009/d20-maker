#include "mouse.h"

/*
 * 
 */
#define	MASK_DOWN	1
#define	MASK_UP		2
#define	MASK_HELD	4

/*
 * 
 */
#define	IS_DOWN	(mouse_state & MASK_DOWN)
#define IS_UP		(mouse_state & MASK_UP)
#define	IS_HELD	(mouse_state & MASK_HELD)
//
#define	TOGGLE_DOWN	mouse_state ^= MASK_DOWN
#define	TOGGLE_UP		mouse_state ^= MASK_UP
#define	TOGGLE_HELD	mouse_state ^= MASK_HELD

/*
 * 
 */
#define	SET_DOWN	mouse_state |= MASK_DOWN
#define	SET_UP		mouse_state |= MASK_UP
#define	SET_HELD	mouse_state |= MASK_HELD
//
#define	RESET_DOWN	if IS_DOWN	TOGGLE_DOWN
#define	RESET_UP		if IS_UP		TOGGLE_UP
#define	RESET_HELD	if IS_HELD	TOGGLE_HELD

/*
 *	event handles
 */
void mouse_handle_move (SDL_Event *e)
{
	mouseX = e->motion.x;
	mouseY = e->motion.y;
}

void mouse_handle_down ()
{
	SET_DOWN;
}

void mouse_handle_up ()
{
	RESET_HELD;
	SET_UP;
}

/*
 * 	
 */
void mouse_reset (void)
{
	if  IS_DOWN	SET_HELD;
	RESET_DOWN;
	RESET_UP;
}

void mouse_update (SDL_Event *e)
{
	switch (e->type)
	{
		case SDL_MOUSEBUTTONDOWN:
		{	mouse_handle_down ();	break;	}
		
		case SDL_MOUSEBUTTONUP:
		{	mouse_handle_up ();		break;	}
		
		case SDL_MOUSEMOTION:
		{	mouse_handle_move (e);break;	}
	}
}

/*
 * 
 */
int mouse_is_down (void)
{
	return (int) IS_DOWN;
}

int mouse_is_up (void)
{
	return (int)	IS_UP;
}

int mouse_is_held (void)
{
	return (int) IS_HELD;
}
