/* sdloshw.c: Top-level SDL management functions.
 *
 * Copyright (C) 2001-2006 by Brian Raiter, under the GNU General Public
 * License. No warranty. See COPYING for details.
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<SDL.h>
#include	"sdlgen.h"
#include	"../err.h"

#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include "debugScreen.h"
#define printf psvDebugScreenPrintf

#define PAD_TRIANGLE 0
#define PAD_CIRCLE 1
#define PAD_CROSS 2
#define PAD_SQUARE 3
#define PAD_L 4
#define PAD_R 5
#define PAD_DOWN 6
#define PAD_LEFT 7
#define PAD_UP 8
#define PAD_RIGHT 9
#define PAD_SELECT 10
#define PAD_START 11

int corresponding_keys[12] = {
	'o' ,// PAD_TRIANGLE 0
	'q' ,// PAD_CIRCLE 1
	SDLK_RETURN ,// PAD_CROSS 2
	'r' ,// PAD_SQUARE 3
	'p' ,// PAD_L 4
	'n' ,// PAD_R 5
	SDLK_DOWN ,// PAD_DOWN 6
	SDLK_LEFT ,// PAD_LEFT 7
	SDLK_UP ,// PAD_UP 8
	SDLK_RIGHT ,// PAD_RIGHT 9
	's' ,// PAD_SELECT 10
	SDLK_F1 // PAD_START 11
};




/* Values global to this library.
 */
oshwglobals	sdlg;

/* This is an automatically-generated file, which contains a
 * representation of the program's icon.
 */
#include	"ccicon.c"

/* Dispatch all events sitting in the SDL event queue. 
 */
static void _eventupdate(int wait)
{
    static int	mousevisible = TRUE;
    SDL_Event	event;
    int		x, y;

    if (wait)
	SDL_WaitEvent(NULL);
    SDL_PumpEvents();
    while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_ALLEVENTS)) {
	switch (event.type) {
	  case SDL_JOYBUTTONDOWN:
	  	keyeventcallback(corresponding_keys[event.jbutton.button], TRUE);
		SDL_ShowCursor(SDL_DISABLE);
		mousevisible = FALSE;
    	break;
	  
	  case SDL_JOYBUTTONUP:
	  	keyeventcallback(corresponding_keys[event.jbutton.button], FALSE);
		break;

	  case SDL_KEYDOWN:
	    if (mousevisible) {
			SDL_GetMouseState(&x, &y);
			if (windowmappos(x, y) < 0) {
				SDL_ShowCursor(SDL_DISABLE);
				mousevisible = FALSE;
			}
	    }
	    keyeventcallback(event.key.keysym.sym, TRUE);
	    if (event.key.keysym.unicode
			&& event.key.keysym.unicode != event.key.keysym.sym) {
			keyeventcallback(event.key.keysym.unicode, TRUE);
			keyeventcallback(event.key.keysym.unicode, FALSE);
	    }
	    break;
	  case SDL_KEYUP:
	    if (mousevisible) {
			SDL_GetMouseState(&x, &y);
			if (windowmappos(x, y) < 0) {
				SDL_ShowCursor(SDL_DISABLE);
				mousevisible = FALSE;
			}
	    }
	    keyeventcallback(event.key.keysym.sym, FALSE);
	    break;
	  case SDL_MOUSEBUTTONDOWN:
	  case SDL_MOUSEBUTTONUP:
	    if (!mousevisible) {
			SDL_ShowCursor(SDL_ENABLE);
			mousevisible = TRUE;
	    }
	    mouseeventcallback(event.button.x, event.button.y,
			       event.button.button,
			       event.type == SDL_MOUSEBUTTONDOWN);
	    break;
	  case SDL_MOUSEMOTION:
	    if (!mousevisible) {
			SDL_ShowCursor(SDL_ENABLE);
			mousevisible = TRUE;
	    }
	    break;
	  case SDL_QUIT:
	    exit(EXIT_SUCCESS);
	}
    }
}

/* Alter the window decoration, translating the subtitle to UTF-8 as
 * necessary.
 */
void setsubtitle(char const *subtitle)
{
    char	buf[512] = "Tile World";
    char       *p;

    if (subtitle && *subtitle) {
	p = buf + strlen(buf);
	*p++ = ' ';
	*p++ = '-';
	*p++ = ' ';
	for ( ; *subtitle ; ++subtitle) {
	    if (*subtitle & 0x80) {
		*p++ = 0xC0 | ((*subtitle >> 6) & 0x03);
		*p++ = 0x80 | (*subtitle & 0x3F);
	    } else {
		*p++ = *subtitle;
	    }
	}
	*p = '\0';
    }
    SDL_WM_SetCaption(buf, "Tile World");
}

/* Shut down SDL.
 */
static void shutdown(void)
{
    SDL_Quit();
}

/* Initialize SDL, create the program's icon, and then initialize
 * the other modules of the library.
 */
int oshwinitialize(int silence, int soundbufsize,
		   int showhistogram, int fullscreen)
{
    SDL_Surface	       *icon;

    sdlg.eventupdatefunc = _eventupdate;

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) < 0) {
	errmsg(NULL, "Cannot initialize SDL system: %s\n", SDL_GetError());
	return FALSE;
    }


//=====JOYSTICK DETECTION NOTICE=====//
// 	psvDebugScreenInit();
// 	printf("%i joysticks were found.\n\n", SDL_NumJoysticks() );
//     printf("The names of the joysticks are:\n");
//     for( int i=0; i < SDL_NumJoysticks(); i++ ) 
//     {
//         printf("    %s\n", SDL_JoystickName(i));
//     }
//	sceKernelDelayThread(5*1000000); // Wait for 3 seconds

//===========ENABLE JOYSTICK==============//
	SDL_Joystick *joystick;
    SDL_JoystickEventState(SDL_ENABLE);
    joystick = SDL_JoystickOpen(0);

	atexit(shutdown);    
	setsubtitle(NULL);

    icon = SDL_CreateRGBSurfaceFrom(cciconimage, CXCCICON, CYCCICON,
				    32, 4 * CXCCICON,
				    0x0000FF, 0x00FF00, 0xFF0000, 0);
    if (icon) {
	SDL_WM_SetIcon(icon, cciconmask);
	SDL_FreeSurface(icon);
    } else
	warn("couldn't create icon surface: %s", SDL_GetError());

    return _sdltimerinitialize(showhistogram)
	&& _sdltextinitialize()
	&& _sdltileinitialize()
	&& _sdlinputinitialize()
	&& _sdloutputinitialize(fullscreen)
	&& _sdlsfxinitialize(silence, soundbufsize);
}

/* The real main().
 */
int main(int argc, char *argv[])
{

    return tworld(argc, argv);
}
