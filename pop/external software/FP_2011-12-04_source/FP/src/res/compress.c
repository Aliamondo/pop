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
compress.c: Princed Resources : Image Compression Library
¯¯¯¯¯¯¯¯¯¯
 Copyright 2003, 2004 Princed Development Team
  Created: 24 Aug 2003

  Author: Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.01 (2003-Oct-23)
  Version: 2.00 (2004-Mar-07)

 Note:
  DO NOT remove this copyright notice
*/

#include <stdio.h>
#include <string.h>
#include "compress.h"
#include "memory.h"
/*#include "pr.h"*/
#include "disk.h" /* array2short */

/***************************************************************\
|                  I M P L E M E N T A T I O N                  |
\***************************************************************/

/***************************************************************\
|                        Image transpose                        |
\***************************************************************/

/* Determines where the transposed byte must be saved */
int transpose(int x,int w,int h) {
	return ((x%h)*(w))+(int)(x/h);
}

void transposeImage(tImage* image,int size) {
	unsigned char* outputaux=getMemory(size);
	int cursor=0;

	while (cursor<size) {outputaux[transpose(cursor,image->widthInBytes,image->height)]=image->pix[cursor];cursor++;}
	free(image->pix);
	image->pix=outputaux;
}

void antiTransposeImage(tImage* image,int size) {
	unsigned char* outputaux=getMemory(size);
	int cursor=0;

	while (cursor<size) {outputaux[cursor]=image->pix[transpose(cursor,image->widthInBytes,image->height)];cursor++;}
	free(image->pix);
	image->pix=outputaux;
}

/***************************************************************\
|                    Uncompression algorithms                   |
\***************************************************************/

/* LZG expansion algorithm sub function */
unsigned char popBit(unsigned char *byte) {
	unsigned char bit=(unsigned char)((*byte)&1);
	(*byte)>>=1;
	return bit;
}

/* Expands LZ Groody algorithm. This is the core of PR */
int expandLzg(const unsigned char* array, int arraySize, tImage* image, int imageSize) {
	char k;
	int location,h,cursor=0,pos=0;
	unsigned char maskbyte,rep;

	if ((image->pix=getMemory(/*imageSize*/MAX_MOD_SIZE_IN_LZG))==NULL) return COMPRESS_RESULT_FATAL; /* reserve memory */
	for(location=0;location<MAX_MOD_SIZE_IN_LZG;image->pix[location]=0,location++); /* clean output garbage */

	/* main loop */
	while (cursor<imageSize) {
		maskbyte=array[pos++];
		for (k=8;k&&(cursor<imageSize);k--) {
			if (popBit(&maskbyte)) {
				image->pix[cursor++]=array[pos++];
			} else {
				location=66+(((rep=array[pos])&3)<<8)+(unsigned char)array[pos+1];pos+=2;
				rep=(unsigned char)((rep>>2)+3);
				while (rep--) { /* Be careful in big images */
					h=cursor/MAX_MXD_SIZE_IN_LZG-((location%MAX_MXD_SIZE_IN_LZG)>(cursor%MAX_MXD_SIZE_IN_LZG));
					image->pix[cursor++]=image->pix[((h<0)?0:h)*MAX_MXD_SIZE_IN_LZG+(location++)%MAX_MXD_SIZE_IN_LZG];
/*
					h=((cursor-(location&0x3FF))&(~0x3FF));
					image->pix[cursor]=image->pix[((h<0)?0:h)+(location&0x3FF)];
					cursor++;location++;
*/
				}
			}
		}
	}
	return ((pos==arraySize)&(cursor==imageSize))-1; /* WARNING or SUCCESS */
}

/* Expands RLE algorithm */
int expandRle(const unsigned char* array, int arraySize, tImage* image, int imageSize) {
	int cursor=0;
	register signed char rep;
	int pos=0;

	if ((image->pix=getMemory(imageSize+128))==NULL) return COMPRESS_RESULT_FATAL; /* reserve memory */

	/* main loop */
	while (cursor<imageSize) {
		rep=(signed char)(array[pos++]);
		if (rep<0) {
			/* Negative */
			while (rep++) image->pix[cursor++]=array[pos];
			pos++;
		} else {
			/* Positive */
			rep=~rep;
			while (rep++) image->pix[cursor++]=array[pos++];
		}
	}
	return ((pos==arraySize)&(cursor==imageSize))-1; /* WARNING or SUCCESS */
}

/***************************************************************\
|                    Compression algorithms                     |
\***************************************************************/

/* Compress using the Run Length Encoding algorithm */
void compressRle(unsigned char* data,tImage* img,int *dataSize) {
	/* Declare pointers */
	unsigned char* cursorData  = data;
	char*          counter;
	unsigned char* cursorPix   = img->pix;
	unsigned char* imgEnd      = img->pix+(*dataSize);

	while (cursorPix<imgEnd) {
		/* Step 1: Create counter */
		*(counter=(char*)(cursorData++))=-1;

		/* Step 2: Look and copy the string until a repeated byte is found */
		while (
			(cursorPix<imgEnd)&&
			(
				(*cursorPix!=*(cursorPix+1))||
				(
					/*(*cursorPix==*(cursorPix+1))&&*/
					((cursorPix+1)<imgEnd)&&
					(*cursorPix!=*(cursorPix+2))
				)
			)&&
			((*counter)!=127)
		) {
			*(cursorData)=*(cursorPix);
			(*counter)++;
			cursorPix++;
			cursorData++;
		}

		/* Step 3: If there was a repeated string, let's ignore it and add the cursor with the repetitions */
		if (*counter==-1) {
			while ((cursorPix+1<imgEnd)&&(*cursorPix==(*(cursorPix+1)))&&((*counter)!=-128)) {
				cursorPix++;
				(*counter)--;
			}

			*(cursorData)=*(cursorPix); /* Print repeated char */
			cursorPix++;
			cursorData++;
		}
	}

	*(cursorData++)=0;
	*(cursorData)=*(cursorPix);
	*dataSize=(int)((long int)cursorData-(long int)data)-1; /* Note: casted to long for portability with 64 bits architectures */
}

/***************************************************************\
|               Main compress and expand graphics               |
\***************************************************************/

/*
		Header info:
		 1 byte  - checksum           char checksum
		 2 bytes - height             short int height
		 2 bytes - width              short int width
		 1 byte  - 00                 (char)0
		 1 byte  - compression type   unsigned char compressionType
*/

/* Expands an array into an image */
int mExpandGraphic(const unsigned char* data,tImage *image, int dataSizeInBytes) {
	/*
		Reads data and extracts tImage
		returns the next image address or -1 in case of error
	*/

	int imageSizeInBytes;
	int result;

	data++;
	image->height=array2short(data);/*((unsigned char)data[0])+((unsigned char)data[1]<<8);data+=2;*/
	data+=2;
	image->width =array2short(data);/*((unsigned char)data[0])+((unsigned char)data[1]<<8);data+=2;*/
	data+=2;
	if (*(data++)) return COMPRESS_RESULT_FATAL; /* Verify format */
	image->type=(unsigned char)(*(data++));
	dataSizeInBytes-=7;
	if (image->type&0xB0) {
		image->widthInBytes=(image->width+1)/2;
	} else {
		image->widthInBytes=(image->width+7)/8;
	}
	imageSizeInBytes=image->widthInBytes*image->height;

	switch (getAlgor(image->type)) {
		case COMPRESS_RAW: /* No Compression Algorithm */
			if ((image->pix=getMemory(imageSizeInBytes))==NULL) return COMPRESS_RESULT_FATAL;
			memcpy(image->pix,data,imageSizeInBytes);
			result=COMPRESS_RESULT_SUCCESS;
			break;
		case COMPRESS_RLE_LR: /* RLE Left to Right Compression Algorithm */
			result=expandRle(data,dataSizeInBytes,image,imageSizeInBytes);
			break;
		case COMPRESS_RLE_UD: /* RLE Up to Down Compression Algorithm */
			result=expandRle(data,dataSizeInBytes,image,imageSizeInBytes);
			if (result==COMPRESS_RESULT_FATAL) return COMPRESS_RESULT_FATAL;
			transposeImage(image,imageSizeInBytes);
			break;
		case COMPRESS_LZG_LR: /* LZ Groody Left to Right Compression Algorithm */
			result=expandLzg(data,dataSizeInBytes,image,imageSizeInBytes);
			break;
		case COMPRESS_LZG_UD: /* LZ Groody Up to Down Compression Algorithm */
			result=expandLzg(data,dataSizeInBytes,image,imageSizeInBytes);
			if (result==COMPRESS_RESULT_FATAL) return COMPRESS_RESULT_FATAL;
			transposeImage(image,imageSizeInBytes);
			break;
		default:
			result=COMPRESS_RESULT_FATAL;
			break;
	}
	return result; /* Ok */
}

/* Compress an image into binary data */
int mCompressGraphic(unsigned char* *data,tImage* image, int* dataSizeInBytes) {
	/* Declare variables */
	unsigned char* compressed     [COMPRESS_WORKING_ALGORITHMS];
	int            compressedSize [COMPRESS_WORKING_ALGORITHMS];
	int            algorithm;
	int            i;
	int            imageSizeInBytes;

	/* Initialize variables */
	imageSizeInBytes=image->widthInBytes*image->height;
	for (i=0;i<COMPRESS_WORKING_ALGORITHMS;i++) compressedSize[i]=imageSizeInBytes;

	/*
		Perform all compressions
	*/

	/* COMPRESS_RAW */
	compressed[COMPRESS_RAW]=getMemory(compressedSize[COMPRESS_RAW]);
	memcpy(compressed[COMPRESS_RAW],image->pix,compressedSize[COMPRESS_RAW]);

	/* COMPRESS_RLE_LR */
	compressed[COMPRESS_RLE_LR]=getMemory((10*imageSizeInBytes+50)); /* This will reserve 10*(image size)+50 bytes, to allocate the compressed file */
	compressRle(compressed[COMPRESS_RLE_LR],image,&(compressedSize[COMPRESS_RLE_LR]));

	/* COMPRESS_RLE_UD */
	compressed[COMPRESS_RLE_UD]=getMemory(10*imageSizeInBytes+50); /* This will reserve 10*(image size)+50 bytes, to allocate the compressed file */
	antiTransposeImage(image,imageSizeInBytes);
	compressRle(compressed[COMPRESS_RLE_UD],image,&(compressedSize[COMPRESS_RLE_UD]));

	/*
		Process results
	*/

	/* Select the best compression (find minimum) */
	*dataSizeInBytes=compressedSize[COMPRESS_RAW];
	algorithm=COMPRESS_RAW;
	for (i=COMPRESS_RLE_LR;i<COMPRESS_WORKING_ALGORITHMS;i++) {
		if ((*dataSizeInBytes)>compressedSize[i]) {
			(*dataSizeInBytes)=compressedSize[i];
			algorithm=i;
		}
	}

	/* Copy the best algorithm in the compressed data */
	*data=getMemory(*dataSizeInBytes+6);
	memcpy(*data+6,compressed[algorithm],*dataSizeInBytes);
	(*dataSizeInBytes)+=6;

	/*
		Write header
	*/

	/* (16 bits)height (Intel short int format) */
	(*data)[0]=image->height;
	(*data)[1]=image->height>>8;
	/* (16 bits)width (Intel short int format) */
	(*data)[2]=image->width;
	(*data)[3]=image->width>>8;
	/* (8 bits)00000000+(4 bits)palette type+(4 bits)algorithm */
	(*data)[4]=0;
	(*data)[5]=image->type|algorithm;

	/* Free all compression attempts */
	for (i=COMPRESS_RAW;i<COMPRESS_WORKING_ALGORITHMS;i++) free(compressed[i]);
	return 1;
}

