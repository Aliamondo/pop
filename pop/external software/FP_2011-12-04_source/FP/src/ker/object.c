/*  Princed V3 - Prince of Persia Level Editor for PC Version
    Copyright (C) 2003 Princed Development Team

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    The authors of this program may be contacted at http://forum.princed.com.ar
*/

/*
object.h: Free Prince : Objects
¯¯¯¯¯
 Copyright 2004 Princed Development Team
  Created: 19 Jul 2004

  Author: Enrique Calot <ecalot.cod@princed.com.ar>

 Note:
  DO NOT remove this copyright notice
*/

#include "kid.h"
#include "output.h"
#include "resources.h" /* resLoad resFree */
#include "maps.h" /* mapGetRoom getTile */
#include <stdio.h> /* NULL */
#include "states.h"
#include "object.h"
#include "room.h" /* roomGetTile */

void loadGfx(int storeMirrored, tData** gfxCache, unsigned long resId) {
	gfxCache[DIR_LEFT]=resLoad(resId);
	if (storeMirrored)
		gfxCache[DIR_RIGHT]=resLoad(resId|RES_MODS_INVERT);
	else
		gfxCache[DIR_RIGHT]=NULL;
}

void objectFree(tObject* obj) {
	resFree(obj->gfxCache[DIR_LEFT]);
	if (obj->gfxCache[DIR_RIGHT]) resFree(obj->gfxCache[DIR_RIGHT]);
	stateFree(&obj->action);
}

void objectInterrupt(tObject* obj,short action) {
	stateReset(&obj->action,action);
}

/* TODO: make a function in maps.c that calls this one for the kid */
tObject objectCreate(int location, int floor, int direction, int stateId, unsigned long resId, int cacheMirror, tObjectType type) {
	tObject object;

	loadGfx(cacheMirror,object.gfxCache,resId);

	object.location=location;
	object.floor=floor;
	object.type=type;
	object.direction=direction;
	object.action=stateCreate(stateId);

	/* Default lives */
	object.lives=3;
	object.hitPoints=3;
	return object;
}

void objectDraw(tObject* object) {
	/* stateUpdate MUST be called before this function if the object was created/interrupted */
	void* image=object->gfxCache[object->direction ^ stateGetMirror(object)]->pFrames[stateGetImage(object)-1];
	/* TODO: move this -1 to each script frame */
	outputDrawBitmap(
		image, 
		object_getLocation(object,image),
		58-stateGetBottom(object)+object->floor*TILE_H
	);
#ifdef DEBUG_POS
	if (object->type==oKid)
		outputDrawMessage(1,"kidPos: %d,%d",object_getLocation(object,image),stateGetBottom(object));
#endif
}

/* event triggered when an object is moved */

int objectMove(tObject* object,tKey key,tRoom* room) {
	/* advance state and get the flag, then interpret the flag and do the events */
	short flags;
	int refresh;
	int x;
	int y;
	tTile tile;

	flags=stateUpdate(&key,object,room);

	if (room==NULL) return flags; /* exits if it is not associated to a room */

	/* a static variable type in the tObject determinates what object is it about.
	 * This is to simulate polymorphism.
	 * call a function that performs all the actions knowing the room,
	 * the object and the flags. Returns refresh.
	 */

	switch (object->type) {
		case oKid:
			/* Move the kid */
			refresh=kidMove(object,flags,room);
			/* Calculate the new positions */
			x=object->location/TILE_W;
			y=object->floor;

			if (refresh) *room=mapGetRoom(room->level,room->id);
			refresh=0;
			tile=roomGetTile(room,x+1,y+1);

			if (isIn(tile,TILES_BLOCK)) {
#ifdef OBJECT_DEBUG
				printf("INTERRUPTION! Wall at (x,y)=(%d,%d) tile=(%d,%d) loc=%d\n",x,y,tile.code,tile.back,object->location);
#endif
				objectInterrupt(object,STATE_MARKS_CRASH);
				      stateUpdate(NULL,object,room); /* move again the to the interrupted state */
				flags=stateUpdate(NULL,object,room); /* move again to the absoluteOnStart state */
			}

			/* Check if the object must fall down */
			if (flags&STATES_FLAG_P) {
				if (!isIn(tile,TILES_WALKABLE)) { /* INTERRUPTION */
#ifdef OBJECT_DEBUG
					printf("INTERRUPTION! No floor at (x,y)=(%d,%d) tile=(%d,%d)\n",x,y,tile.code,tile.back);
#endif
					objectInterrupt(object,STATE_MARKS_FALL);
					flags=stateUpdate(&key,object,room); /* move again the to the interrupted state */
				}
			}
			if (flags&STATES_FLAG_H) {
				if (!kidTakeHitPoint(object)) /* loose a hit point */
					/* the kid has died! */
					flags=STATE_EXIT_CODE_SPLASH;
				else
					outputBlinkScreen(1,1);
			}
			if (flags&STATES_FLAG_D) {
				if (!kidDrinkPotion(object,tile)) /* drink the potion */
					flags=STATE_EXIT_CODE_SPLASH;
				/* Change the map */
				room->level->fore[(room->id-1)*30+x+y*10]=TILE_FLOOR;
				room->level->back[(room->id-1)*30+x+y*10]=0;
				refresh=1;
			}
			break;
		case oGeneric:
		default:
			refresh=0;
			break;
	}

	if (refresh) /* room map was changed and needs to be refreshed */
		*room=mapGetRoom(room->level,room->id);

	return flags;
}

