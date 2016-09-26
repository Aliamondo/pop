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

    The authors of this program may be contacted at http://forum.princed.org
*/

/*
wave.c: Princed Resources : 
¯¯¯¯¯¯¯¯¯¯
 Copyright 2006 Princed Development Team
  Created: 19 Feb 2006

  Author: Enrique Calot <ecalot.cod@princed.org>
  Version: 1.01 (2006-Feb-09)

 Note:
  DO NOT remove this copyright notice
*/

/***************************************************************\
|                  I M P L E M E N T A T I O N                  |
\***************************************************************/

#include "binary.h" /* tBinary */
#include "common.h"
#include "wav.h" 
#include <stdlib.h>
#include <string.h> /* memcpy */
#include "dat.h" 
#include "reslist.h" 

/***************************************************************\
|                         Binary Object                         |
\***************************************************************/

void* objSoundCreate(tBinary cont, int *error) { /* use get like main.c */
	tBinary* r;
	*error=PR_RESULT_SUCCESS;
	
	r=(tBinary*)malloc(sizeof(tBinary));
	r->data=cont.data+1;
	r->size=cont.size-1;
	return (void*)r;
}

int objSoundWrite(void* o, const char* file, int write(const char* file,tBinary* data,int optionflag, const char* backupExtension), int optionflag, const char* backupExtension) {
	return write(file,(tBinary*)o,optionflag,backupExtension);
}

int objWaveWrite(void* o, const char* file, int optionflag, const char* backupExtension) {
	return writeWav(file,(tBinary*)o,optionflag,backupExtension);
}

/*void* objSoundRead(const char* file, int read(const char* file, tBinary* c, int *pchannels, long *psamplerate, long *pbps), int *result, int *pchannels, long *psamplerate, long *pbps) {
	tBinary* o=(tBinary*)malloc(sizeof(tBinary));
	*result=read(file,o);
	return (void*)o;
}*/

void* objWaveRead(const char* file, int *result) {
	int  channels;
	long samplerate;
 	long bps;
	tBinary* o=(tBinary*)malloc(sizeof(tBinary));
	
	*result=readWav(file,o,&channels,&samplerate,&bps);

	if (*result==PR_RESULT_SUCCESS) {
		if (bps!=8)            *result=PR_RESULT_WAV_UNSUPPORTED_BITRATE;
		if (samplerate!=11025) *result=PR_RESULT_WAV_UNSUPPORTED_SAMPLERATE;
		if (channels!=1)     *result=PR_RESULT_WAV_UNSUPPORTED_STEREO;
	}
	if (*result!=PR_RESULT_SUCCESS) {
		free(o->data);
		return NULL;
	}
	
	return (void*)o;
}

/* TODO: factorize */
int objWaveSet(void* o,tResource* res) {
	tBinary* wave=o;
	res->content.size=wave->size+1;
	res->content.data=malloc(wave->size+1);
	res->content.data[0]=0x01; /* TODO: use WAVE_MAGIC */
	memcpy(res->content.data+1,wave->data,wave->size);
	mWriteFileInDatFile(res);
	return PR_RESULT_SUCCESS;
}

int objMidiSet(void* o,tResource* res) {
	tBinary* midi=o;
	res->content.size=midi->size+1;
	res->content.data=malloc(midi->size+1);
	res->content.data[0]=0x02; /* TODO: use MIDI_MAGIC */
	memcpy(res->content.data+1,midi->data,midi->size);
	mWriteFileInDatFile(res);
	return PR_RESULT_SUCCESS;
}

int objPcspeakerSet(void* o,tResource* res) {
	tBinary* pcspeaker=o;
	res->content.size=pcspeaker->size+1;
	res->content.data=malloc(pcspeaker->size+1);
	res->content.data[0]=0x00; /* TODO: use PCSPEAKER_MAGIC */
	memcpy(res->content.data+1,pcspeaker->data,pcspeaker->size);
	mWriteFileInDatFile(res);
	return PR_RESULT_SUCCESS;
}

