/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2004 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id: SDL_macmouse.c,v 1.6 2004/01/04 16:49:25 slouken Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#if TARGET_API_MAC_CARBON
#include <Carbon.h>
#else
#include <Quickdraw.h>
#endif

/* Routines that are not supported by the Carbon API... */
#if !TARGET_API_MAC_CARBON
#include <CursorDevices.h>
#endif

#include "SDL_error.h"
#include "SDL_mouse.h"
#include "SDL_macmouse_c.h"


/* The implementation dependent data for the window manager cursor */
struct WMcursor {
	Cursor curs;
};


void Mac_FreeWMCursor(_THIS, WMcursor *cursor)
{
	free(cursor);
}

WMcursor *Mac_CreateWMCursor(_THIS,
		Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y)
{
	WMcursor *cursor;
	int row, bytes;
		
	/* Allocate the cursor memory */
	cursor = (WMcursor *)malloc(sizeof(WMcursor));
	if ( cursor == NULL ) {
		SDL_OutOfMemory();
		return(NULL);
	}
	memset(cursor, 0, sizeof(*cursor));
    
    if (w > 16)
        w = 16;
    
    if (h > 16)
        h = 16;
    
	bytes = (w+7)/8;

	for ( row=0; row<h; ++row ) {
		memcpy(&cursor->curs.data[row], data, bytes);
		data += bytes;
	}
	for ( row=0; row<h; ++row ) {
		memcpy(&cursor->curs.mask[row], mask, bytes);
		mask += bytes;
	}
	cursor->curs.hotSpot.h = hot_x;
	cursor->curs.hotSpot.v = hot_y;

	/* That was easy. :) */
	return(cursor);
}

int Mac_cursor_showing = 1;

int Mac_ShowWMCursor(_THIS, WMcursor *cursor)
{
	if ( cursor == NULL ) {
		if ( Mac_cursor_showing ) {
			HideCursor();
			Mac_cursor_showing = 0;
		}
	} else {
		SetCursor(&cursor->curs);
		if ( ! Mac_cursor_showing ) {
			ShowCursor();
			Mac_cursor_showing = 1;
		}
	}
	return(1);
}

void Mac_WarpWMCursor(_THIS, Uint16 x, Uint16 y)
{
#if !TARGET_API_MAC_CARBON
	CursorDevice *cursordevice;

	cursordevice = nil;
	CursorDeviceNextDevice(&cursordevice);
	if ( cursordevice != nil ) {
		WindowPtr saveport;
		Point where;

		GetPort(&saveport);
		SetPort(SDL_Window);
		where.h = x;
		where.v = y;
		LocalToGlobal(&where);
		SetPort(saveport);
		CursorDeviceMoveTo(cursordevice, where.h, where.v);
	}
#endif /* !TARGET_API_MAC_CARBON */
}

