// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

//static const char rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

#include <errno.h>
#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

#define POINTER_WARP_COUNTDOWN	1

SDL_Window   *S_window   = NULL;
SDL_Renderer *S_renderer = NULL;
SDL_Palette  *S_pal      = NULL;
SDL_Texture  *image		 = NULL;
int		S_width;
int		S_height;


//XShmSegmentInfo	X_shminfo;
//int		X_shmeventtype;

// Fake mouse handling.
// This cannot work properly w/o DGA.
// Needs an invisible mouse cursor at least.
boolean		grabMouse;
int		doPointerWarp = POINTER_WARP_COUNTDOWN;



//
//  Translates the key currently in X_event
//

int xlatekey(SDL_Event *event)
{

    int rc;

    switch(rc = event->key.key) //sym.sym) // SDL3 change
    {
		case SDLK_LEFT:		rc = KEY_LEFTARROW;	 break;
		case SDLK_RIGHT:	rc = KEY_RIGHTARROW; break;
		case SDLK_DOWN:		rc = KEY_DOWNARROW;	 break;
		case SDLK_UP:		rc = KEY_UPARROW;	 break;
		case SDLK_ESCAPE:	rc = KEY_ESCAPE;	 break;
		case SDLK_RETURN:	rc = KEY_ENTER;		 break;
		case SDLK_TAB:		rc = KEY_TAB;		 break;
		case SDLK_F1:		rc = KEY_F1;		 break;
		case SDLK_F2:		rc = KEY_F2;		 break;
		case SDLK_F3:		rc = KEY_F3;		 break;
		case SDLK_F4:		rc = KEY_F4;		 break;
		case SDLK_F5:		rc = KEY_F5;		 break;
		case SDLK_F6:		rc = KEY_F6;		 break;
		case SDLK_F7:		rc = KEY_F7;		 break;
		case SDLK_F8:		rc = KEY_F8;		 break;
		case SDLK_F9:		rc = KEY_F9;		 break;
		case SDLK_F10:		rc = KEY_F10;		 break;
		case SDLK_F11:		rc = KEY_F11;		 break;
		case SDLK_F12:		rc = KEY_F12;		 break;

		case SDLK_BACKSPACE:
		case SDLK_DELETE:	rc = KEY_BACKSPACE;	 break;

		case SDLK_PAUSE:	rc = KEY_PAUSE;		 break;

		case SDLK_KP_EQUALS:
		case SDLK_EQUALS:	rc = KEY_EQUALS;	 break;

		case SDLK_KP_MINUS:
		case SDLK_MINUS:	rc = KEY_MINUS;		 break;

		case SDLK_RSHIFT:
		case SDLK_LSHIFT:	rc = KEY_RSHIFT;	 break;

		case SDLK_LCTRL:
		case SDLK_RCTRL:	rc = KEY_RCTRL;		 break;

		case SDLK_LALT:
		case SDLK_RALT:

		default:
			if (rc >= (int)SDLK_SPACE && rc <= (int)SDLK_DELETE)
				rc = rc - SDLK_SPACE + ' ';
			if (rc >= 'A' && rc <= 'Z')
				rc = rc - 'A' + 'a';
			break;
    }

    return rc;

}

void I_ShutdownGraphics(void)
{
    SDL_DestroyWindow(S_window);
    SDL_DestroyRenderer(S_renderer);
	
	SDL_DestroyPalette(S_pal);
	SDL_DestroyTexture(image);
}



//
// I_StartFrame
//
void I_StartFrame (void)
{
    // er?
}


void I_ProcessEvent(SDL_Event *sdl_event)
{
    event_t 	 event;
	unsigned int mbflags;

	if ((sdl_event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) ||
		(sdl_event->type == SDL_EVENT_MOUSE_BUTTON_UP  ) ||
		(sdl_event->type == SDL_EVENT_MOUSE_MOTION     )
	)
	{
		mbflags = SDL_GetMouseState(NULL, NULL);
	}

    switch (sdl_event->type)
    {
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
			event.type = (sdl_event->type == SDL_EVENT_KEY_DOWN) ? ev_keydown : ev_keyup;
			event.data1 = xlatekey(sdl_event);
			D_PostEvent(&event);
			break;
			
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case   SDL_EVENT_MOUSE_BUTTON_UP:
#ifdef EMSCRIPTEN
			emscripten_request_pointerlock("canvas", true);
#endif
		case      SDL_EVENT_MOUSE_MOTION:
			event.type = ev_mouse;
			event.data1 =
				  (mbflags     	             & SDL_BUTTON_LMASK        )
				| (mbflags                   & SDL_BUTTON_RMASK ? 2 : 0)
				| (mbflags   		         & SDL_BUTTON_MMASK ? 4 : 0);
			event.data2 = event.data3 = 0;
			if(sdl_event->type == SDL_EVENT_MOUSE_MOTION && grabMouse)
			{
				event.data2 =  (int)roundf(sdl_event->motion.xrel) << 2;
				event.data3 = -(int)roundf(sdl_event->motion.yrel) << 2; // Am I a newgen for inverting the Y? lmfao
			}
			D_PostEvent(&event);
			break;

		case SDL_EVENT_QUIT:
			I_Quit(); //byebye
			break;
		
		default:
			//if (doShm && X_event.type == X_shmeventtype) shmFinished = True;
			break;
    }
}


//
// I_StartTic
//
void I_StartTic (void)
{
	SDL_Event    S_event;

	while (SDL_PollEvent(&S_event))
    	I_ProcessEvent(&S_event);
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{
    static int	lasttic;

    // draws little dots on the bottom of the screen
    if (devparm)
    {
 		int		tics;
    	int		i;
		i = I_GetTime();
		tics = i - lasttic;
		lasttic = i;

		if (tics > 20) tics = 20;

		for (i = 0; i < tics * 2; i += 2)
			screens[0][ (S_height-1) * S_width + i] = 0xff;
		for (     ; i < 20   * 2; i += 2)
			screens[0][ (S_height-1) * S_width + i] = 0x0;
		
    }



	void* pixels;
	int pitch;
	SDL_LockTexture(image, NULL, &pixels, &pitch);
	memcpy(pixels, screens[0], S_width * S_height); 
	SDL_UnlockTexture(image);

    SDL_RenderClear(S_renderer);
	SDL_RenderTexture(S_renderer, image, NULL, NULL);
    SDL_RenderPresent(S_renderer);

}


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], S_width * S_height);
}




//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
	register int	i;

	for (i = 0; i < 256; i++)
	{
			S_pal->colors[i].r = gammatable[usegamma][*palette++];
			S_pal->colors[i].g = gammatable[usegamma][*palette++];
			S_pal->colors[i].b = gammatable[usegamma][*palette++];
	}
}



void I_InitGraphics(void)
{   
	static int firsttime = 1;

    if (!firsttime) return;
    firsttime = 0;

    signal(SIGINT, (void (*)(int)) I_Quit);

    if(!SDL_SetAppMetadata("Ready DOOM", "1.1", "uk.zackhe.room"))
	{
		fprintf(stderr, "Couldn't set SDL metadata... not that I care: %s\n", SDL_GetError());
	}

    if (!SDL_Init(SDL_INIT_VIDEO))
	{
        I_Error("Couldn't initialize SDL: %s\n", SDL_GetError());
        return;
    }

    
    S_width  = SCREENWIDTH;
    S_height = SCREENHEIGHT;

    // check if the user wants to grab the mouse (quite unnice)
    grabMouse = True; // !!M_CheckParm("-grabmouse");

    // create the palette
    S_pal = SDL_CreatePalette(256);
	if (S_pal == NULL)
	{
		I_Error("Couldn't create palette: %s\n", SDL_GetError());
		return;
	}
			
	if (!SDL_CreateWindowAndRenderer("Ready DOOM", S_width, S_height,
#ifndef EMSCRIPTEN
		SDL_WINDOW_RESIZABLE,
#else
		SDL_WINDOW_FULLSCREEN,
#endif
		&S_window, &S_renderer
	))
	{
        I_Error("Couldn't create window/renderer: %s\n", SDL_GetError());
        return;
    }

    if (!SDL_SetRenderLogicalPresentation(S_renderer, S_width, S_height, SDL_LOGICAL_PRESENTATION_LETTERBOX))
	{
        I_Error("Couldn't set logical renderer presentation: %s\n", SDL_GetError());
        return;
	}

    if (!SDL_HideCursor())
	{
		I_Error("Couldn't hide cursor: %s\n", SDL_GetError());
        return;
	}

    // grabs the pointer so it is restricted to this window
    if (grabMouse)
		SDL_SetWindowMouseGrab(S_window, True); // nonfatal, no msg


	image = SDL_CreateTexture(S_renderer, SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_STREAMING, S_width, S_height);
	if (image == NULL)
	{
		I_Error("Couldn't create texture: %s\n", SDL_GetError());
		return;
	}

	if (SDL_SetTextureScaleMode(image, SDL_SCALEMODE_NEAREST))
	{
		fprintf(stderr, "Setting scale mode failed! %s\n", SDL_GetError());
	}

	if (!SDL_SetTexturePalette(image, S_pal   ))
	{
		I_Error("Couldn't set texture palette: %s\n", SDL_GetError());
		return;
	}

	screens[0] = (unsigned char *) malloc(S_width * S_height);

}
