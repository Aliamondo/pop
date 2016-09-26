/* apoplexy v2.0 (December 2013)
 * Copyright (C) 2008-2013 Norbert de Jonge <mail@norbertdejonge.nl>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see [ www.gnu.org/licenses/ ].
 *
 * To properly read this code, set your program's tab stop to: 2.
 */

/*========== Includes ==========*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#if defined WIN32 || _WIN32 || WIN64 || _WIN64
#include <windows.h>
#undef PlaySound
#endif

#include "SDL.h"
#include "SDL_audio.h"
#include "SDL_thread.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
/*========== Includes ==========*/

/*========== Defines ==========*/
#define EXIT_NORMAL 0
#define EXIT_ERROR 1
#define MAX_DATA 720
#define MAX_TEXT 100
#define ROOMS 32 /*** PoP1 only uses 24. ***/
#define USER_DATA 2000
#define SCREEN_WIDTH 642 + 50
#define SCREEN_HEIGHT 380 + 75
#define SCREEN_BPP 16
#define EDITOR_NAME "apoplexy"
#define EDITOR_VERSION "v2.0 (December 2013)"
#define COPYRIGHT "Copyright (C) 2013 Norbert de Jonge"
#define REFRESH 30 /*** That is 33 frames per second, 1000/30. ***/
#define MAX_FILE 100
#define MAX_ADJ 10
#define MAX_GRID 10
#define FONT_SIZE_1 15
#define FONT_SIZE_2 11
#define FONT_SIZE_3 20
#define MAX_TOWRITE 720
#define NUM_SOUNDS 20 /*** Sounds that may play at the same time. ***/
#define MAX_OPTION 100
#define MAX_CUSTOM 10
#define MAX_DATETIME 50
#define MAX_IMG 200

#define OFFSET_X 25
#define OFFSET_Y 50
#define TTP 27
#define HOR0 -64 + OFFSET_X
#define HOR1 0 + OFFSET_X
#define HOR2 64 + OFFSET_X
#define HOR3 128 + OFFSET_X
#define HOR4 192 + OFFSET_X
#define HOR5 256 + OFFSET_X
#define HOR6 320 + OFFSET_X
#define HOR7 384 + OFFSET_X
#define HOR8 448 + OFFSET_X
#define HOR9 512 + OFFSET_X
#define HOR10 576 + OFFSET_X
#define VER1 OFFSET_Y - TTP
#define VER2 OFFSET_Y - TTP + 126
#define VER3 OFFSET_Y - TTP + 252
#define VER4 OFFSET_Y - TTP + 378
#define BROKEN_ROOM_X 438
#define BROKEN_ROOM_Y 76
#define BROKEN_LEFT_X 423
#define BROKEN_LEFT_Y 76
#define BROKEN_RIGHT_X 453
#define BROKEN_RIGHT_Y 76
#define BROKEN_UP_X 438
#define BROKEN_UP_Y 61
#define BROKEN_DOWN_X 438
#define BROKEN_DOWN_Y 91
#define ADJ_BASE_X 422
#define ADJ_BASE_Y 60
#define STATIC 5
#define DYNAMIC 3
#define TILESX1 2;
#define TILESX2 52;
#define TILESX3 102;
#define TILESX4 152
#define TILESX5 202
#define TILESX6 252
#define TILESX7 302
#define TILESX8 352
#define TILESX9 402
#define TILESX10 452
#define TILESX11 502
#define TILESX12 552
#define TILESX13 602
#define TILESY1 2
#define TILESY15 46
#define TILESY2 66
#define TILESY25 120
#define TILESY3 130
#define TILESY4 194
#define TILESY5 258
#define TILESY6 322
#define TILESY7 386
#define TILEWIDTH 48
#define TILEHEIGHT 62
#define BAR_FULL 432
#define MAX_HEX 100

#define G1RIGHT 38
#define G2RIGHT 80
#define G3RIGHT 122
#define G4RIGHT 164
#define G5RIGHT 206
#define G1LEFT 19
#define G2LEFT 61
#define G3LEFT 103
#define G4LEFT 145
#define G5LEFT 187
#define GTYPE1 48
#define GTYPE2 83
#define GTYPE3 130
#define GTYPE4 166
#define GTYPE5 200
#define GTYPE6 227
#define GTYPE7 246
#define GTYPE8 274
#define GTYPE9 300
#define GTYPE10 318
#define GTYPE11 349
#define GTYPE12 377
#define GTYPE13 405
#define ROWW 33
#define ROWH 6

#ifndef O_BINARY
#define O_BINARY 0
#endif

#if defined WIN32 || _WIN32 || WIN64 || _WIN64
#define DEVNULL "NUL"
#define RANDOM rand
#define SRANDOM srand
#define PUTENV putenv
#define PR_EXECUTABLE "pr\\pr.exe"
#define LEVELS_DAT "prince\\LEVELS.DAT"
#define LEVELS_BAK "prince\\LEVELS.BAK"
#define PRINCE_DAT "prince2\\PRINCE.DAT"
#define PRINCE_BAK "prince2\\PRINCE.BAK"
#define PR_RESOURCES "pr\\resources.xml"
#define PR_POP2 "pr\\pop2.xml"
#define IN_DIR "levels\\"
#define OUT_DIR "levels\\"
#define IN_DIR_POP2 "levels2\\PRINCE\\"
#define OUT_DIR_POP2 "levels2\\PRINCE\\"
#define XML_DIR "xml\\"
#define BATCH_FILE "prince\\apoplexy.bat"
#define BATCH_FILE_POP2 "prince2\\apoplexy.bat"
#define PNG_DUNGEON "png\\dungeon\\"
#define PNG_PALACE "png\\palace\\"
#define PNG_OTH "png\\oth\\"
#define PNG_CAVERNS "png\\caverns\\"
#define PNG_CAVERNSD "png\\caverns_details\\"
#define PNG_RUINS "png\\ruins\\"
#define PNG_RUINSD "png\\ruins_details\\"
#define PNG_RUINSB "png\\ruins_back\\"
#define PNG_TEMPLE "png\\temple\\"
#define PNG_TEMPLEB "png\\temple_back\\"
#define PNG_ROOMS "png\\rooms\\"
#define PNG_EXTRAS "png\\extras\\"
#define PNG_VARIOUS "png\\various\\"
#define PNG_LIVING1 "png\\living_pop1\\"
#define PNG_LIVING2 "png\\living_pop2\\"
#define PNG_BUTTONS "png\\buttons\\"
#define PNG_BACK "png\\back\\"
#define PNG_INCOMING "png\\incoming\\"
#define PNG_MUSIC "png\\music\\"
#else
#define DEVNULL "/dev/null"
#define RANDOM random
#define SRANDOM srandom
#define PUTENV SDL_putenv
#define PR_EXECUTABLE "pr/pr"
#define LEVELS_DAT "prince/LEVELS.DAT"
#define LEVELS_BAK "prince/LEVELS.BAK"
#define PRINCE_DAT "prince2/PRINCE.DAT"
#define PRINCE_BAK "prince2/PRINCE.BAK"
#define PR_RESOURCES "pr/resources.xml"
#define PR_POP2 "pr/pop2.xml"
#define IN_DIR "levels/"
#define OUT_DIR "levels/"
#define IN_DIR_POP2 "levels2/PRINCE/"
#define OUT_DIR_POP2 "levels2/PRINCE/"
#define XML_DIR "xml/"
#define BATCH_FILE "prince/apoplexy.bat"
#define BATCH_FILE_POP2 "prince2/apoplexy.bat"
#define PNG_DUNGEON "png/dungeon/"
#define PNG_PALACE "png/palace/"
#define PNG_OTH "png/oth/"
#define PNG_CAVERNS "png/caverns/"
#define PNG_CAVERNSD "png/caverns_details/"
#define PNG_RUINS "png/ruins/"
#define PNG_RUINSD "png/ruins_details/"
#define PNG_RUINSB "png/ruins_back/"
#define PNG_TEMPLE "png/temple/"
#define PNG_TEMPLEB "png/temple_back/"
#define PNG_ROOMS "png/rooms/"
#define PNG_EXTRAS "png/extras/"
#define PNG_VARIOUS "png/various/"
#define PNG_LIVING1 "png/living_pop1/"
#define PNG_LIVING2 "png/living_pop2/"
#define PNG_BUTTONS "png/buttons/"
#define PNG_BACK "png/back/"
#define PNG_INCOMING "png/incoming/"
#define PNG_MUSIC "png/music/"
#endif
/*========== Defines ==========*/

/*** for text ***/
SDL_Color color_bl = {0x00, 0x00, 0x00};
SDL_Color color_wh = {0xff, 0xff, 0xff};
SDL_Color color_red = {0xff, 0x00, 0x00};
SDL_Rect offset;
SDL_Surface *message = NULL;

/* Text. The magic identifier "POP_LVL" text distinguishes a POP
 * level file from other file formats.
 */
unsigned char sMagicID[7 + 2];

/*** UC. 01 = POP1, 02 = POP2 ***/
unsigned char sPOPVersion[1 + 2];

/*** UC. Level version. ***/
unsigned char sPLVVersion[1 + 2];

/*** UC. Level number. ***/
unsigned char sLevelNumber[1 + 2];

/*** UL. Number of fields. ***/
unsigned char sNumberOfFields[4 + 2];

/* UL. Size of the level in bytes. All PoP1 levels are 2305, except
 * the potions level, which is 2304. All PoP2 levels are 12025.
 */
unsigned char sLevelSize[4 + 2];

/*** The checksum. ***/
unsigned char sChecksum[1 + 2];

/*** The first and second door events. ***/
unsigned char sFirstDoorEvents[256 + 2];
unsigned char sSecondDoorEvents[256 + 2];
unsigned char sPoP2DoorEvents[1280 + 2];
int iPoP2DoorLocation[256 + 2];
int iPoP2DoorRoom[256 + 2];
int iPoP2DoorType[256 + 2];
int iPoP2DoorLeft[256 + 2];
int iPoP2DoorRight[256 + 2];

unsigned char sRoomLinks[128 + 2]; /*** PoP1 is only 96. ***/
unsigned char sUnknownI[64 + 2]; /*** PoP2 is only 1. ***/
unsigned char sStartPosition[3 + 2];
unsigned char sUnknownIIandIII[4 + 2];
unsigned char sGuardLocations[ROOMS + 2];
unsigned char sGuardDirections[ROOMS + 2];
unsigned char sUnknownIVaandIVb[48 + 2];
unsigned char sGuardSkills[ROOMS + 2];
unsigned char sUnknownIVc[24 + 2];
unsigned char sGuardColors[ROOMS + 2];
unsigned char sUnknownIVd[16 + 2];
unsigned char sEndCode[2 + 2];
unsigned char sUserDataCount[4 + 2];
unsigned char sUserData[USER_DATA + 2];
unsigned char sLastRoom[1 + 2];
unsigned char sJEFF[4 + 2];
unsigned char sLevelType[1 + 2];
unsigned char sLevelNumberEvents[1 + 2];
unsigned char sExtraImgResources[24 + 2];
unsigned char sUnknownII[4 + 2];
unsigned char sStaticGuards[3712 + 2];
unsigned char sCheckPoints[18 + 2];
unsigned char sDynamicGuards[1088 + 2];
unsigned char sMusic[960 + 2];

int iStaticGuards_Amount[ROOMS + 2];
int iStaticGuards_1_Locations[ROOMS + 2][STATIC + 2];
int iStaticGuards_2_Offsets1[ROOMS + 2][STATIC + 2];
int iStaticGuards_3_Offsets2[ROOMS + 2][STATIC + 2];
int iStaticGuards_4_Directions[ROOMS + 2][STATIC + 2];
int iStaticGuards_5_Skills[ROOMS + 2][STATIC + 2];
int iStaticGuards_6_Mod[ROOMS + 2][STATIC + 2];
int iStaticGuards_7_Unknown[ROOMS + 2][STATIC + 2];
int iStaticGuards_8_Unknown[ROOMS + 2][STATIC + 2];
int iStaticGuards_9_Unknown[ROOMS + 2][STATIC + 2];
int iStaticGuards_10_Colors[ROOMS + 2][STATIC + 2];
int iStaticGuards_11_Number[ROOMS + 2][STATIC + 2];
int iStaticGuards_12_Unknown[ROOMS + 2][STATIC + 2];
int iStaticGuards_13_Unknown[ROOMS + 2][STATIC + 2];
int iStaticGuards_14_Unknown[ROOMS + 2][STATIC + 2];
int iStaticGuards_15_Unknown[ROOMS + 2][STATIC + 2];
int iStaticGuards_16_Type[ROOMS + 2][STATIC + 2];
int iStaticGuards_17_Hitpoints[ROOMS + 2][STATIC + 2];
int iStaticGuards_18_SkelAct1[ROOMS + 2][STATIC + 2];
int iStaticGuards_19_SkelAct2[ROOMS + 2][STATIC + 2];
int iStaticGuards_20_Unknown[ROOMS + 2][STATIC + 2];
int iStaticGuards_21_Unknown[ROOMS + 2][STATIC + 2];
int iStaticGuards_22_Unknown[ROOMS + 2][STATIC + 2];
int iStaticGuards_23_Unknown[ROOMS + 2][STATIC + 2];
int iDynamicGuards_Sets[ROOMS + 2];
int iDynamicGuards_Skill[ROOMS + 2];
int iDynamicGuards_Unknown1[ROOMS + 2];
int iDynamicGuards_Unknown2[ROOMS + 2];
int iDynamicGuards_1_SetNr[ROOMS + 2][DYNAMIC + 2];
int iDynamicGuards_2_MaxTog[ROOMS + 2][DYNAMIC + 2];
int iDynamicGuards_3_Row[ROOMS + 2][DYNAMIC + 2];
int iDynamicGuards_4_Column[ROOMS + 2][DYNAMIC + 2];
int iDynamicGuards_5_LatBefore[ROOMS + 2][DYNAMIC + 2];
int iDynamicGuards_6_LatBetween[ROOMS + 2][DYNAMIC + 2];
int iDynamicGuards_7_CanJump[ROOMS + 2][DYNAMIC + 2];
int iDynamicGuards_8_SpIfAl[ROOMS + 2][DYNAMIC + 2];
int iDynamicGuards_9_Amount[ROOMS + 2][DYNAMIC + 2];
int iDynamicGuards_10_Hitpoints[ROOMS + 2][DYNAMIC + 2];
int iCheckPoints[18 + 2];

unsigned int gamespeed;
Uint32 looptime;

int iDebug;
int iNoAudio;
int iNoChomp;
int iNoAnim;
int iScreen;
int cCurType;
int iCurRoom;
int iDownAt;
int iSelected;
int iOnTile;
int iOKOn;
int iYesOn;
int iNoOn;
int iCloseOn;
int iCloseCustomOn;
int iCloseGuardsOn;
int iCloseMusicOn;
int iCloseBackgroundOn;
int iBackgroundOn;
int iGuardsOn;
int iCustomOn;
int iCustomUseOn;
int iMusicOn;
int iEditLeftOn;
int iEndLeftOn;
int iEditRightOn;
int iEndRightOn;
int iMAllOn;
int iMNoneOn;
int iMLeftOn;
int iMRightOn;
int iMTopOn;
int iMMiddleOn;
int iMBottomOn;
int iMInvertOn;
int iHelpOK;
unsigned long luLevelSize;
unsigned long luLevelNr;
unsigned long luKidRoom;
unsigned long luKidPos;
unsigned long luKidDir;
unsigned long luNumberOfFields;
int iRoomConnections[32 + 2][4 + 2]; /*** PoP1 only uses 24. ***/
int iRoomConnectionsBroken[32 + 2][4 + 2]; /*** PoP1 only uses 24. ***/
int iBrokenRoomLinks;
int iThingA[32 + 2][30 + 2]; /*** PoP1 only uses 24. ***/
int iModifierA[32 + 2][30 + 2][4 + 2]; /*** PoP1 only uses 24 and 1. ***/
char sInformation[20 + 2][2 + 2][100 + 2]; /*** 20 times User Data ***/
int iInformationNr;
TTF_Font *font1 = NULL;
TTF_Font *font2 = NULL;
TTF_Font *font3 = NULL;
SDL_Surface *screen;
int iXPos, iYPos;
int iChanged;
int iChangeEvent;
int iChangeJumps;
int iChangeFrame;
int iChangePercent;
int iChangeWall;
int iChangeStal;
int iChangeBot;
int iChangeSym;
int iChangeCrk;
int iGuardSkill;
char sAuthor[MAX_OPTION];
char sStartLevel[MAX_OPTION];
char sCheat1[MAX_OPTION];
char sCheat2[MAX_OPTION];
int iRoomArray[32 + 1 + 2][32 + 2]; /*** PoP1 only uses 24. ***/
int iMovingRoom;
int iMovingOldX;
int iMovingOldY;
int iMovingNewX;
int iMovingNewY;
int iMovingNewBusy;
int iChangingBrokenRoom;
int iChangingBrokenSide;
int iScale;
int arImage[1] = { 1 };
int arFont[1] = { 0 };
int iChangeForeRand;
int iChangeForeMod;
int iChangeFore;
int iChangeMod[4 + 2]; /*** PoP1 only uses 1. ***/
int iIsCustom;
int iExtras;
int iLastThing;
int iLastModifier[4 + 2];
int iThingACopyPaste[30 + 2];
int iModifierACopyPaste[30 + 2][4 + 2];
int iGuardACopyPaste[4 + 2];
unsigned char sMusicACopyPaste[30 + 2];
int iStaticGuardsACopyPaste[116 + 2];
int iDynamicGuardsACopyPaste[34 + 2];
int iCopied;
int iStartLevel;
int iPoP1;
int iPoP2;
int iPR;
int iOnPoP1;
int iOnPoP2;
int iEditPoP;
int iRooms;
int iRoomLinks;
int iPosShown;
int iPreLoaded;
int iNrToPreLoad;
int iCurrentBarHeight;
int iHourglassFrame;
int iSandFrame;
int iFlameFrameDP;
int iChomperFrameDP;
int iChomped;
int iSwordFrameDP;
int iFlameFrameC;
int iFlameFrameT;
int iGuardTooltip, iGuardTooltipOld;
int iLvl10Warning;
int iMod3Hi, iMod4Hi;

/*** locations; PoP1 ***/
SDL_Surface *imgd0_0[2 + 2];
SDL_Surface *imgp0_0[2 + 2];
SDL_Surface *imgd0_1[2 + 2];
SDL_Surface *imgp0_1[2 + 2];
SDL_Surface *imgd0_2[2 + 2];
SDL_Surface *imgp0_2[2 + 2];
SDL_Surface *imgd0_3[2 + 2];
SDL_Surface *imgp0_3[2 + 2];
SDL_Surface *imgd0_255[2 + 2];
SDL_Surface *imgp0_255[2 + 2];
SDL_Surface *imgd1_0[2 + 2];
SDL_Surface *imgp1_0[2 + 2];
SDL_Surface *imgd1_1[2 + 2];
SDL_Surface *imgp1_1[2 + 2];
SDL_Surface *imgd1_2[2 + 2];
SDL_Surface *imgp1_2[2 + 2];
SDL_Surface *imgd1_3[2 + 2];
SDL_Surface *imgp1_3[2 + 2];
SDL_Surface *imgd1_255[2 + 2];
SDL_Surface *imgp1_255[2 + 2];
SDL_Surface *imgd2_0[2 + 2];
SDL_Surface *imgp2_0[2 + 2];
SDL_Surface *imgd2_1[2 + 2];
SDL_Surface *imgp2_1[2 + 2];
SDL_Surface *imgd2_2[2 + 2];
SDL_Surface *imgp2_2[2 + 2];
SDL_Surface *imgd2_3[2 + 2];
SDL_Surface *imgp2_3[2 + 2];
SDL_Surface *imgd2_4[2 + 2];
SDL_Surface *imgp2_4[2 + 2];
SDL_Surface *imgd2_5[2 + 2];
SDL_Surface *imgp2_5[2 + 2];
SDL_Surface *imgd2_6[2 + 2];
SDL_Surface *imgp2_6[2 + 2];
SDL_Surface *imgd2_7[2 + 2];
SDL_Surface *imgp2_7[2 + 2];
SDL_Surface *imgd2_8[2 + 2];
SDL_Surface *imgp2_8[2 + 2];
SDL_Surface *imgd2_9[2 + 2];
SDL_Surface *imgp2_9[2 + 2];
SDL_Surface *imgd3_0[2 + 2];
SDL_Surface *imgp3_0[2 + 2];
SDL_Surface *imgd4_0[2 + 2];
SDL_Surface *imgp4_0[2 + 2];
SDL_Surface *imgd4_1[2 + 2];
SDL_Surface *imgp4_1[2 + 2];
SDL_Surface *imgd5_0[2 + 2];
SDL_Surface *imgp5_0[2 + 2];
SDL_Surface *imgd6[2 + 2];
SDL_Surface *imgp6[2 + 2];
SDL_Surface *imgd7_0[2 + 2];
SDL_Surface *imgp7_0[2 + 2];
SDL_Surface *imgd7_1[2 + 2];
SDL_Surface *imgp7_1[2 + 2];
SDL_Surface *imgd7_2[2 + 2];
SDL_Surface *imgp7_2[2 + 2];
SDL_Surface *imgd7_3[2 + 2];
SDL_Surface *imgp7_3[2 + 2];
SDL_Surface *imgd8_0[2 + 2];
SDL_Surface *imgp8_0[2 + 2];
SDL_Surface *imgd9_0[2 + 2];
SDL_Surface *imgp9_0[2 + 2];
SDL_Surface *imgd10_0[2 + 2];
SDL_Surface *imgp10_0[2 + 2];
SDL_Surface *imgd10_1[2 + 2];
SDL_Surface *imgp10_1[2 + 2];
SDL_Surface *imgd10_2[2 + 2];
SDL_Surface *imgp10_2[2 + 2];
SDL_Surface *imgd10_3[2 + 2];
SDL_Surface *imgp10_3[2 + 2];
SDL_Surface *imgd10_4[2 + 2];
SDL_Surface *imgp10_4[2 + 2];
SDL_Surface *imgd10_5[2 + 2];
SDL_Surface *imgp10_5[2 + 2];
SDL_Surface *imgd10_6[2 + 2];
SDL_Surface *imgp10_6[2 + 2];
SDL_Surface *imgd11_0[2 + 2];
SDL_Surface *imgp11_0[2 + 2];
SDL_Surface *imgd12_0[2 + 2];
SDL_Surface *imgp12_0[2 + 2];
SDL_Surface *imgd12_1[2 + 2];
SDL_Surface *imgp12_1[2 + 2];
SDL_Surface *imgd12_2[2 + 2];
SDL_Surface *imgp12_2[2 + 2];
SDL_Surface *imgd12_3[2 + 2];
SDL_Surface *imgp12_3[2 + 2];
SDL_Surface *imgd12_4[2 + 2];
SDL_Surface *imgp12_4[2 + 2];
SDL_Surface *imgd12_5[2 + 2];
SDL_Surface *imgp12_5[2 + 2];
SDL_Surface *imgd12_6[2 + 2];
SDL_Surface *imgp12_6[2 + 2];
SDL_Surface *imgd12_7[2 + 2];
SDL_Surface *imgp12_7[2 + 2];
SDL_Surface *imgd13_0[2 + 2];
SDL_Surface *imgp13_0[2 + 2];
SDL_Surface *imgd14_0[2 + 2];
SDL_Surface *imgp14_0[2 + 2];
SDL_Surface *imgd15[2 + 2];
SDL_Surface *imgp15[2 + 2];
SDL_Surface *imgd16_0[2 + 2];
SDL_Surface *imgp16_0[2 + 2];
SDL_Surface *imgd17_0[2 + 2];
SDL_Surface *imgp17_0[2 + 2];
SDL_Surface *imgd18_0[2 + 2];
SDL_Surface *imgp18_0[2 + 2];
SDL_Surface *imgd18_1[2 + 2];
SDL_Surface *imgp18_1[2 + 2];
SDL_Surface *imgd18_2[2 + 2];
SDL_Surface *imgp18_2[2 + 2];
SDL_Surface *imgd18_3[2 + 2];
SDL_Surface *imgp18_3[2 + 2];
SDL_Surface *imgd18_4[2 + 2];
SDL_Surface *imgp18_4[2 + 2];
SDL_Surface *imgd18_5[2 + 2];
SDL_Surface *imgp18_5[2 + 2];
SDL_Surface *imgd19_0[2 + 2];
SDL_Surface *imgp19_0[2 + 2];
SDL_Surface *imgd20_0[2 + 2];
SDL_Surface *imgp20_0[2 + 2];
SDL_Surface *imgd20_1[2 + 2];
SDL_Surface *imgp20_1[2 + 2];
SDL_Surface *imgd21_0[2 + 2];
SDL_Surface *imgp21_0[2 + 2];
SDL_Surface *imgd22_0[2 + 2];
SDL_Surface *imgp22_0[2 + 2];
SDL_Surface *imgd23_0[2 + 2];
SDL_Surface *imgp23_0[2 + 2];
SDL_Surface *imgd24_0[2 + 2];
SDL_Surface *imgp24_0[2 + 2];
SDL_Surface *imgd25_0[2 + 2];
SDL_Surface *imgp25_0[2 + 2];
SDL_Surface *imgd26_0[2 + 2];
SDL_Surface *imgp26_0[2 + 2];
SDL_Surface *imgd27_0[2 + 2];
SDL_Surface *imgp27_0[2 + 2];
SDL_Surface *imgd28_0[2 + 2];
SDL_Surface *imgp28_0[2 + 2];
SDL_Surface *imgd29_0[2 + 2];
SDL_Surface *imgp29_0[2 + 2];
SDL_Surface *imgd30_0[2 + 2];
SDL_Surface *imgp30_0[2 + 2];
SDL_Surface *imgd43_0[2 + 2];
SDL_Surface *imgp43_0[2 + 2];
SDL_Surface *spriteflamed1;
SDL_Surface *spriteflamed2;
SDL_Surface *spriteflamep1;
SDL_Surface *spriteflamep2;
SDL_Surface *spritechomperd;
SDL_Surface *spritechomperdsel;
SDL_Surface *spritechomperp;
SDL_Surface *spritechomperpsel;
SDL_Surface *spriteswordd;
SDL_Surface *spritesworddsel;
SDL_Surface *spriteswordp;
SDL_Surface *spriteswordpsel;

/*** locations; rooftops, desert, final ***/
SDL_Surface *imgo0[2 + 2];
SDL_Surface *imgo1[2 + 2];
SDL_Surface *imgo20[2 + 2];
SDL_Surface *imgo3[2 + 2];
SDL_Surface *imgo37[2 + 2];
SDL_Surface *imgo38[2 + 2];
SDL_Surface *imgo39[2 + 2];
SDL_Surface *imgo4[2 + 2];
SDL_Surface *imgo28[2 + 2];
SDL_Surface *imgo29[2 + 2];
SDL_Surface *imgo30[2 + 2];
SDL_Surface *imgo10_32[2 + 2];
SDL_Surface *imgo31[2 + 2];
SDL_Surface *imgo40[2 + 2];
SDL_Surface *imgo41[2 + 2];
SDL_Surface *imgo2[2 + 2];
SDL_Surface *imgo5[2 + 2];
SDL_Surface *imgo6[2 + 2];
SDL_Surface *imgo7_0[2 + 2];
SDL_Surface *imgo7_1[2 + 2];
SDL_Surface *imgo7_2[2 + 2];
SDL_Surface *imgo7_3[2 + 2];
SDL_Surface *imgo8[2 + 2];
SDL_Surface *imgo9[2 + 2];
SDL_Surface *imgo10_0[2 + 2];
SDL_Surface *imgo10_64[2 + 2];
SDL_Surface *imgo10_96[2 + 2];
SDL_Surface *imgo10_128[2 + 2];
SDL_Surface *imgo10_160[2 + 2];
SDL_Surface *imgo10_192[2 + 2];
SDL_Surface *imgo10_224[2 + 2];
SDL_Surface *imgo11[2 + 2];
SDL_Surface *imgo12[2 + 2];
SDL_Surface *imgo13[2 + 2];
SDL_Surface *imgo14[2 + 2];
SDL_Surface *imgo15[2 + 2];
SDL_Surface *imgo16[2 + 2];
SDL_Surface *imgo17[2 + 2];
SDL_Surface *imgo18[2 + 2];
SDL_Surface *imgo19[2 + 2];
SDL_Surface *imgo21[2 + 2];
SDL_Surface *imgo22_0[2 + 2];
SDL_Surface *imgo22_1[2 + 2];
SDL_Surface *imgo23[2 + 2];
SDL_Surface *imgo24[2 + 2];
SDL_Surface *imgo25[2 + 2];
SDL_Surface *imgo26[2 + 2];
SDL_Surface *imgo27[2 + 2];
SDL_Surface *imgo32[2 + 2];
SDL_Surface *imgo33[2 + 2];
SDL_Surface *imgo34[2 + 2];
SDL_Surface *imgo35[2 + 2];
SDL_Surface *imgo36[2 + 2];
SDL_Surface *imgo43[2 + 2];
SDL_Surface *imgo44[2 + 2];

/*** locations; caverns ***/
SDL_Surface *imgc0_0[2 + 2];
SDL_Surface *imgc0_1[2 + 2];
SDL_Surface *imgc0_2[2 + 2];
SDL_Surface *imgc0_3[2 + 2];
SDL_Surface *imgc0_4[2 + 2];
SDL_Surface *imgc1_0[2 + 2];
SDL_Surface *imgc1_1[2 + 2];
SDL_Surface *imgc1_2[2 + 2];
SDL_Surface *imgc2_0[2 + 2];
SDL_Surface *imgc2_5[2 + 2];
SDL_Surface *imgc3[2 + 2];
SDL_Surface *imgc4_0[2 + 2];
SDL_Surface *imgc4_100[2 + 2];
SDL_Surface *imgc4_200[2 + 2];
SDL_Surface *imgc8_0[2 + 2];
SDL_Surface *imgc8_1[2 + 2];
SDL_Surface *imgc8_2[2 + 2];
SDL_Surface *imgc9[2 + 2];
SDL_Surface *imgc10_0[2 + 2];
SDL_Surface *imgc10_128[2 + 2];
SDL_Surface *imgc10_160[2 + 2];
SDL_Surface *imgc10_192[2 + 2];
SDL_Surface *imgc10_224[2 + 2];
SDL_Surface *imgc10_32[2 + 2];
SDL_Surface *imgc10_64[2 + 2];
SDL_Surface *imgc10_96[2 + 2];
SDL_Surface *imgc11_0[2 + 2];
SDL_Surface *imgc11_128[2 + 2];
SDL_Surface *imgc14_0[2 + 2];
SDL_Surface *imgc14_1[2 + 2];
SDL_Surface *imgc14_2[2 + 2];
SDL_Surface *imgc16[2 + 2];
SDL_Surface *imgc17_0[2 + 2];
SDL_Surface *imgc17_21[2 + 2];
SDL_Surface *imgc17_42[2 + 2];
SDL_Surface *imgc18[2 + 2];
SDL_Surface *imgc19[2 + 2];
SDL_Surface *imgc20_0[2 + 2];
SDL_Surface *imgc20_1[2 + 2];
SDL_Surface *imgc20_2[2 + 2];
SDL_Surface *imgc20_3[2 + 2];
SDL_Surface *imgc20_4[2 + 2];
SDL_Surface *imgc20_5[2 + 2];
SDL_Surface *imgc20_6[2 + 2];
SDL_Surface *imgc20_7[2 + 2];
SDL_Surface *imgc20_8[2 + 2];
SDL_Surface *imgc20_9[2 + 2];
SDL_Surface *imgc20_10[2 + 2];
SDL_Surface *imgc20_128[2 + 2];
SDL_Surface *imgc20_129[2 + 2];
SDL_Surface *imgc20_130[2 + 2];
SDL_Surface *imgc20_131[2 + 2];
SDL_Surface *imgc20_132[2 + 2];
SDL_Surface *imgc20_133[2 + 2];
SDL_Surface *imgc20_134[2 + 2];
SDL_Surface *imgc20_135[2 + 2];
SDL_Surface *imgc20_136[2 + 2];
SDL_Surface *imgc20_137[2 + 2];
SDL_Surface *imgc20_138[2 + 2];
SDL_Surface *imgc23[2 + 2];
SDL_Surface *imgc24[2 + 2];
SDL_Surface *imgc28[2 + 2];
SDL_Surface *imgc32[2 + 2];
SDL_Surface *imgc33[2 + 2];
SDL_Surface *imgc34_E_0[2 + 2];
SDL_Surface *imgc34_E_8[2 + 2];
SDL_Surface *imgc35[2 + 2];
SDL_Surface *imgc36[2 + 2];
SDL_Surface *imgc44[2 + 2];
SDL_Surface *spriteflamec1;
SDL_Surface *spriteflamec2;

/*** locations; ruins ***/
SDL_Surface *imgr0[2 + 2];
SDL_Surface *imgr1_0[2 + 2];
SDL_Surface *imgr1_1[2 + 2];
SDL_Surface *imgr1_2[2 + 2];
SDL_Surface *imgr1_3[2 + 2];
SDL_Surface *imgr2[2 + 2];
SDL_Surface *imgr3_0[2 + 2];
SDL_Surface *imgr3_1[2 + 2];
SDL_Surface *imgr3_2[2 + 2];
SDL_Surface *imgr4_0[2 + 2];
SDL_Surface *imgr4_100[2 + 2];
SDL_Surface *imgr4_200[2 + 2];
SDL_Surface *imgr5_E_0[2 + 2];
SDL_Surface *imgr5_E_8[2 + 2];
SDL_Surface *imgr6_E_0[2 + 2];
SDL_Surface *imgr6_E_8[2 + 2];
SDL_Surface *imgr7_0[2 + 2];
SDL_Surface *imgr7_1[2 + 2];
SDL_Surface *imgr7_2[2 + 2];
SDL_Surface *imgr7_3[2 + 2];
SDL_Surface *imgr8[2 + 2];
SDL_Surface *imgr9[2 + 2];
SDL_Surface *imgr10_0[2 + 2];
SDL_Surface *imgr10_128[2 + 2];
SDL_Surface *imgr10_160[2 + 2];
SDL_Surface *imgr10_192[2 + 2];
SDL_Surface *imgr10_224[2 + 2];
SDL_Surface *imgr10_32[2 + 2];
SDL_Surface *imgr10_64[2 + 2];
SDL_Surface *imgr10_96[2 + 2];
SDL_Surface *imgr11[2 + 2];
SDL_Surface *imgr12_0[2 + 2];
SDL_Surface *imgr12_128[2 + 2];
SDL_Surface *imgr12_160[2 + 2];
SDL_Surface *imgr12_32[2 + 2];
SDL_Surface *imgr13_0[2 + 2];
SDL_Surface *imgr13_32[2 + 2];
SDL_Surface *imgr14_16[2 + 2];
SDL_Surface *imgr14_32[2 + 2];
SDL_Surface *imgr14_64[2 + 2];
SDL_Surface *imgr15_0[2 + 2];
SDL_Surface *imgr15_1[2 + 2];
SDL_Surface *imgr15_2[2 + 2];
SDL_Surface *imgr15_3[2 + 2];
SDL_Surface *imgr16[2 + 2];
SDL_Surface *imgr17_0[2 + 2];
SDL_Surface *imgr17_21[2 + 2];
SDL_Surface *imgr17_42[2 + 2];
SDL_Surface *imgr18[2 + 2];
SDL_Surface *imgr20_0[2 + 2];
SDL_Surface *imgr20_1[2 + 2];
SDL_Surface *imgr20_2[2 + 2];
SDL_Surface *imgr20_4[2 + 2];
SDL_Surface *imgr20_5[2 + 2];
SDL_Surface *imgr20_6[2 + 2];
SDL_Surface *imgr20_8[2 + 2];
SDL_Surface *imgr20_9[2 + 2];
SDL_Surface *imgr20_10[2 + 2];
SDL_Surface *imgr20_12[2 + 2];
SDL_Surface *imgr20_13[2 + 2];
SDL_Surface *imgr20_14[2 + 2];
SDL_Surface *imgr21[2 + 2];
SDL_Surface *imgr22_1[2 + 2];
SDL_Surface *imgr22_2[2 + 2];

/*** locations; temple ***/
SDL_Surface *imgt0[2 + 2];
SDL_Surface *imgt1_0[2 + 2];
SDL_Surface *imgt1_1[2 + 2];
SDL_Surface *imgt2_0[2 + 2];
SDL_Surface *imgt2_5[2 + 2];
SDL_Surface *imgt3_0[2 + 2];
SDL_Surface *imgt3_1[2 + 2];
SDL_Surface *imgt4_0[2 + 2];
SDL_Surface *imgt4_100[2 + 2];
SDL_Surface *imgt4_200[2 + 2];
SDL_Surface *imgt5_E_0[2 + 2];
SDL_Surface *imgt5_E_8[2 + 2];
SDL_Surface *imgt6_E_0[2 + 2];
SDL_Surface *imgt6_E_8[2 + 2];
SDL_Surface *imgt8[2 + 2];
SDL_Surface *imgt9[2 + 2];
SDL_Surface *imgt10_0[2 + 2];
SDL_Surface *imgt10_128[2 + 2];
SDL_Surface *imgt10_160[2 + 2];
SDL_Surface *imgt10_192[2 + 2];
SDL_Surface *imgt10_224[2 + 2];
SDL_Surface *imgt10_32[2 + 2];
SDL_Surface *imgt10_64[2 + 2];
SDL_Surface *imgt10_96[2 + 2];
SDL_Surface *imgt11[2 + 2];
SDL_Surface *imgt12_0[2 + 2];
SDL_Surface *imgt12_128[2 + 2];
SDL_Surface *imgt12_160[2 + 2];
SDL_Surface *imgt12_32[2 + 2];
SDL_Surface *imgt13_0[2 + 2];
SDL_Surface *imgt13_32[2 + 2];
SDL_Surface *imgt14_0[2 + 2];
SDL_Surface *imgt14_1[2 + 2];
SDL_Surface *imgt16[2 + 2];
SDL_Surface *imgt17_0[2 + 2];
SDL_Surface *imgt17_21[2 + 2];
SDL_Surface *imgt17_42[2 + 2];
SDL_Surface *imgt18[2 + 2];
SDL_Surface *imgt19[2 + 2];
SDL_Surface *imgt20_0[2 + 2];
SDL_Surface *imgt20_1[2 + 2];
SDL_Surface *imgt20_2[2 + 2];
SDL_Surface *imgt25_0[2 + 2];
SDL_Surface *imgt25_1[2 + 2];
SDL_Surface *imgt25_2[2 + 2];
SDL_Surface *imgt26[2 + 2];
SDL_Surface *imgt32[2 + 2];
SDL_Surface *spriteflamet1;
SDL_Surface *spriteflamet2;

/*** PoP1 guards ***/
SDL_Surface *imggdb[2 + 2];
SDL_Surface *imgggy[2 + 2];
SDL_Surface *imgglp[2 + 2];
SDL_Surface *imggoy[2 + 2];
SDL_Surface *imggpb[2 + 2];
SDL_Surface *imggrp[2 + 2];
SDL_Surface *imggyo[2 + 2];
SDL_Surface *imgskel[2 + 2];
SDL_Surface *imgfat[2 + 2];
SDL_Surface *imgjaffar[2 + 2];
SDL_Surface *imgshadow[2 + 2];

/*** PoP1 guards, selected ***/
SDL_Surface *imggsel[2 + 2];
SDL_Surface *imgskelsel[2 + 2];
SDL_Surface *imgfatsel[2 + 2];
SDL_Surface *imgjaffarsel[2 + 2];
SDL_Surface *imgshadowsel[2 + 2];

/*** PoP2 guards ***/
SDL_Surface *imgbirdman[2 + 2];
SDL_Surface *imgfakeprince[2 + 2];
SDL_Surface *imgguardblack[2 + 2];
SDL_Surface *imgguardblue[2 + 2];
SDL_Surface *imgguardred[2 + 2];
SDL_Surface *imgheadlong[2 + 2];
SDL_Surface *imgheadshort[2 + 2];
SDL_Surface *imgskelawake[2 + 2];
SDL_Surface *imgskelawakes[2 + 2];
SDL_Surface *imgskelawakessws[2 + 2];
SDL_Surface *imgskelsleeps[2 + 2];
SDL_Surface *imgsnake[2 + 2];
SDL_Surface *imgsword[2 + 2];

/*** various ***/
SDL_Surface *imgunk[2 + 2], *imgup_0, *imgup_1, *imgdown_0, *imgdown_1;
SDL_Surface *imgleft_0, *imgleft_1, *imgright_0, *imgright_1, *imgroomson_0;
SDL_Surface *imgroomson_1, *imgroomsoff, *imgeventson_0, *imgeventson_1;
SDL_Surface *imgeventsoff, *imgsaveoff, *imgsaveon_0, *imgsaveon_1;
SDL_Surface *imgquit_0, *imgquit_1, *imgevents, *imgsele, *imgsell;
SDL_Surface *imgblack, *imghourglass, *imgprevon_0, *imgprevon_1;
SDL_Surface *imghourglasssprite, *imgsandsprite;
SDL_Surface *imgnexton_0, *imgnexton_1, *imgbar;
SDL_Surface *extras_0, *extras_1, *extras_2, *extras_3, *extras_4;
SDL_Surface *extras_5, *extras_6, *extras_7, *extras_8;
SDL_Surface *imgudno, *imglrno, *imgudnonfo, *imgkidr[2 + 2], *imgkidl[2 + 2];
SDL_Surface *imgprevoff, *imgnextoff, *imgrl, *imgrl2;
SDL_Surface *imgsrc, *imgsrs, *imgsrm, *imgsrp, *imgsrb;
SDL_Surface *imgfadedl, *imgfadeds, *imgpopup, *imgok[2 + 2];
SDL_Surface *imgpopup_yn, *imgyes[2 + 2], *imgno[2 + 2], *imghelp;
SDL_Surface *imgdungeon, *imgpalace, *imgclose_0, *imgclose_1;
SDL_Surface *imgback_0, *imgback_1, *imgguards_0, *imgguards_1;
SDL_Surface *imgborderb, *imgborders, *imgborderbl, *imgbordersl;
SDL_Surface *imgdother, *imgdjaffar, *imgdguards, *imgprincew;
SDL_Surface *imgbroomson_0, *imgbroomson_1, *imgbroomsoff, *imgbrl, *imgbrl2;
SDL_Surface *imgcustoma0, *imgcustomi0, *imgcustoma1, *imgcustomi1, *imgcustom;
SDL_Surface *imgcustom_0, *imgcustom_1, *imgclosebig_0, *imgclosebig_1;
SDL_Surface *imgoth, *imgcaverns, *imgruins, *imgruins_back;
SDL_Surface *imgtemple, *imgtemple_back, *imgguards;
SDL_Surface *imgguard_row, *imgguard_entry;
SDL_Surface *imgfadeddg, *imgfadedsgl, *imgfadedsgr, *imgxtooltip, *imgl10warn;
SDL_Surface *imggw_rooftops, *imggw_desert, *imggw_caverns;
SDL_Surface *imggw_ruins, *imggw_temple, *imggw_final, *imggw_dynamic;
SDL_Surface *imgsb, *imghb;
SDL_Surface *imgincomingl[9 + 2], *imgincomingr[9 + 2];
SDL_Surface *imgmusic_0, *imgmusic_1, *imgmusic, *imgmusic_note;
SDL_Surface *imgm_all_0, *imgm_all_1, *imgm_none_0, *imgm_none_1;
SDL_Surface *imgm_left_0, *imgm_left_1, *imgm_right_0, *imgm_right_1;
SDL_Surface *imgm_top_0, *imgm_top_1, *imgm_middle_0, *imgm_middle_1;
SDL_Surface *imgm_bottom_0, *imgm_bottom_1, *imgm_invert_0, *imgm_invert_1;
SDL_Surface *imgee_0, *imgee_1, *imgec_0, *imgec_1, *imgdistort;

/*** the rooms ***/
SDL_Surface *imgroom1, *imgroom2, *imgroom3, *imgroom4, *imgroom5, *imgroom6;
SDL_Surface *imgroom7, *imgroom8, *imgroom9, *imgroom10, *imgroom11;
SDL_Surface *imgroom12, *imgroom13, *imgroom14, *imgroom15, *imgroom16;
SDL_Surface *imgroom17, *imgroom18, *imgroom19, *imgroom20, *imgroom21;
SDL_Surface *imgroom22, *imgroom23, *imgroom24, *imgroom25, *imgroom26;
SDL_Surface *imgroom27, *imgroom28, *imgroom29, *imgroom30, *imgroom31;
SDL_Surface *imgroom32;

/*** PoP select ***/
SDL_Surface *imgpop, *imgpop1dis, *imgpop1off, *imgpop1on;
SDL_Surface *imgpop2dis, *imgpop2off, *imgpop2on, *imgprprob, *imgpopprob;

/*** PoP2 backgrounds ***/
SDL_Surface *backl1r1, *backl1r2, *backl1r3, *backl1r4, *backl1r5, *backl1r10;
SDL_Surface *backl1r11, *backl1r12, *backl1r15, *backl1r16, *backl1r19;
SDL_Surface *backl2r1, *backl2r2, *backl2r3, *backl14r1, *backl14r2;
SDL_Surface *backl14r3, *backl14r4, *backl14r5, *backl14r6, *backl14r7;
SDL_Surface *backl14r8, *backl6r27, *backl9r2, *backl9r11, *backl9r12;
SDL_Surface *backl9r13, *backl9r14, *backl9r15, *backl9r16, *backl13r4;
SDL_Surface *backl8r9;

SDL_Surface *crack1[2 + 2], *crack3[2 + 2], *crack4[2 + 2], *crack5[2 + 2];
SDL_Surface *crack6[2 + 2], *crack7[2 + 2], *crack8[2 + 2], *crack9[2 + 2];
SDL_Surface *crack10[2 + 2], *crack11[2 + 2], *crack12[2 + 2];
SDL_Surface *symbol16[2 + 2], *symbol32[2 + 2], *symbol48[2 + 2];

SDL_Surface *bottom1[2 + 2], *bottom2[2 + 2], *bottom3[2 + 2];
SDL_Surface *bottom4[2 + 2], *bottom5[2 + 2], *bottom6[2 + 2];
SDL_Surface *bottom7[2 + 2], *stalactite8[2 + 2], *stalactite16[2 + 2];
SDL_Surface *stalactite24[2 + 2], *stalactite32[2 + 2], *stalactite40[2 + 2];
SDL_Surface *stalactite48[2 + 2], *stalactite56[2 + 2];

SDL_Surface *templeb[31 + 2];
int iTempleBHeight[31 + 2];
SDL_Surface *ruinsb[31 + 2];
int iRuinsBHeight[31 + 2];
SDL_Surface *imghorse;
SDL_Surface *imgarrow;
SDL_Surface *imginvisible;

int iMinX, iMaxX, iMinY, iMaxY, iStartRoomsX, iStartRoomsY;
int iDone[32 + 2]; /*** PoP1 only uses 24. ***/

struct sample {
	Uint8 *data;
	Uint32 dpos;
	Uint32 dlen;
} sounds[NUM_SOUNDS];

void CheckRequiredFiles (void);
int LoadPLV (char *sFileName);
int EventInfo (int iNr, int iType);
int ReadFromFile (int iFd, char *sWhat, int iSize, unsigned char *sRetString);
void GetAsEightBits (unsigned char cChar, char *sBinary);
int BitsToInt (char *sString);
void GetForegroundAsName (char *sBinaryFore, char *sName);
void GetModifierAsName (int iFore, int iMod, char *sName);
char cShowDirection (int iDirection);
void ShowImage (int iThing, int iModifier[], SDL_Surface *screen,
	int iLocation, int iFromImageX, int iFromImageY,
	int iFromImageWidth, int iFromImageHeight);
int InArea (SDL_Event event, int iUpperLeftX, int iUpperLeftY,
	int iLowerRightX, int iLowerRightY);
void InitScreen (void);
void EventDoor (int iX, int iY);
void EventNext (int iNoNext);
void RunLevel (int iLevel);
void EventRoom (int iRoom);
void RemoveOldRoom (void);
void AddNewRoom (int iX, int iY, int iRoom);
void InitRooms (void);
int StartGame (void *unused);
void Quit (void);
void ShowScreen (int iScreen);
void Prev (int iCurLevel);
void Next (int iCurLevel);
void WhereToStart (void);
void CheckSides (int iRoom, int iX, int iY);
void ShowRooms (int iRoom, int iX, int iY, int iNext);
void InitPopUp (int iAtScreen, int iStart);
void ShowPopUp (void);
void InitPopUpSave (int iAtScreen);
void ShowPopUpSave (void);
void DisplayText (int iStartX, int iStartY, int iFontSize,
	char sText[9 + 2][MAX_TEXT + 2], int iLines);
void ShowPos (SDL_Rect loc, SDL_Rect dest, int iLocation,
	SDL_Surface *img_first[2 + 2], SDL_Surface *img_second[2 + 2], char *sLbl);
void ShowPosPoP2 (SDL_Rect loc, SDL_Rect dest, int iLocation,
	SDL_Surface *img[2 + 2], char *sLbl);
void ShowDetail (SDL_Surface *img[2 + 2], SDL_Rect loc,
	SDL_Surface *screen, SDL_Rect dest, int iLocation);
void ChangePos (int iLocation, SDL_Surface *screen);
int ChangePosCustom (int iLocation, SDL_Surface *screen);
void ChangeGuards (int iLocation, SDL_Surface *screen);
void ChangeMusic (int iLocation, SDL_Surface *screen);
int ChangeBackground (int iLocation, SDL_Surface *screen);
void ChangeEvent (int iAmount, int iChangePos);
void ChangeCustom (int iAmount, int iForeBack, int iArNr);
int IsDisabled (int iTile);
void UseTile (int iTile, int iLocation, int iRoom);
void SetGuard (int iRoom, int iLoc, int iDirection, int iSkill, int iColor);
void ShowChange (int iLocation);
void ShowChangeCustom (int iLocation);
void ShowChangeGuards (int iLocation);
void ShowChangeMusic (int iLocation);
void ShowChangeBackground (int iLocation);
void DisableSome (int iCurLevel);
int OnTile (SDL_Event event);
int Modulo (long lOne, long lTwo, int iType);
void SetLocation (int iRoom, int iLocation, int iThing, int iModifier1,
	int iModifier2, int iModifier3, int iModifier4);
void CallSave (int iJustXML);
int ChecksumOrWrite (int iFd);
void SavePLV (char *sFileName);
int WriteUserData (int iFd, int iType);
void WriteCharByChar (int iFd, unsigned char *sString, int iLength);
void ShowUsage (void);
void PrIfDe (char *sString);
void MixAudio (void *unused, Uint8 *stream, int iLen);
void PlaySound (char *sFile);
int BrokenRoomLinks (int iRoomConnections[ROOMS + 2][4 + 2], int iPrint);
int BrokenRoomChange (int iRoom, int iSide);
int MouseSelectAdj (SDL_Event event);
void CustomBlitSurface (SDL_Surface* src, char *srcn, SDL_Rect* srcrect,
	SDL_Surface* dst, SDL_Rect *dstrect, int arOptions[1]);
void LoadFonts (void);
void CustomTile (int iThing, int iModifier[], char *sRetString);
void ShowCurrentCustom (SDL_Surface *screen);
void CreateBAK (void);
void Help (int iAtScreen);
void ShowHelp (void);
void Zoom (void);
void Sprinkle (void);
void LoadLevel (int iLevel);
void GetOptionValue (char *sArgv, char *sValue);
void FlipRoom (int iRoom, int iAxis);
void CopyPaste (int iRoom, int iAction);
void DateTime (char *sDateTime);
void SaveXML (char *sFileName);
void PoP1OrPoP2 (void);
void ShowPoP1OrPoP2 (void);
void PreLoad (char *sPath, char *sPNG, SDL_Surface **imgImage);
void PreLoadSet (char *sPath, char cType, char *sTile,
	SDL_Surface **imgReg, SDL_Surface **imgSel);
void LoadingBar (SDL_Surface *screen, int iBarHeight);
void CenterNumber (SDL_Surface *screen, int iNumber, int iX, int iY,
	SDL_Color back, SDL_Color fore);
int IntsToHexToSSLittleEndian (int iHexRight, int iHexLeft);
void SSLittleEndianToHexToInts (int iValue, int *iHexRight, int *iHexLeft);
int PlusMinus (SDL_Event event, int *iWhat, int iX, int iY,
	int iMin, int iMax, int iChange, int iAddChanged);
int WhichStaticGuard (int iGuardNr);
void Backgrounds (void);
void ShowBackground (int iModifier[], int iLoc);
void OpenURL (char *sURL);
int ModAsJumps (int iModifier);
int JumpsAsMod (int iJumps);
int ModAsFrame (int iModifier);
int FrameAsMod (int iFrame);
int ModAsPercent (int iModifier, int iThing);
int PercentAsMod (int iPercent, int iThing);
int ModAsWall (int iModifier);
int WallAsMod (int iWall, int iModifier);
void PoP1Basics (void);
void PoP2Basics (void);
int HiSetBackArea (SDL_Event event, int iX, int iY,
	int iLoc, int iMod3, int iMod4, int iType);
int HiSetBack (SDL_Event event, int iLoc, int iType);
int BackKey (SDL_Event event, char cKey, int iLoc);
void ApplyJumpsFrameIfNecessary (int iLoc);
void ApplyPercentIfNecessary (int iLoc);
void ApplyWallIfNecessary (int iLoc);
void ExtractFromMod2 (int iMod2, char cCrackBot, char cSymbolStal);
int ExtractedAsMod2 (char cCrackBot, char cSymbolStal);
void UpdateMod2IfNecessary (int iLoc);

/*****************************************************************************/
int main (int argc, char *argv[])
/*****************************************************************************/
{
	time_t tm;
	int iTemp;
	int iLevel;

	iDebug = 0;
	iNoAudio = 0;
	iNoChomp = 0;
	iNoAnim = 0;
	snprintf (sAuthor, MAX_OPTION, "%s user", EDITOR_NAME);
	snprintf (sCheat1, MAX_OPTION, "%s", "megahit");
	snprintf (sCheat2, MAX_OPTION, "%s", "yippeeyahoo");
	iScale = 1;
	iStartLevel = 1;
	iExtras = 0;
	iLastThing = 0;
	iLastModifier[1] = 0;
	iLastModifier[2] = 0;
	iLastModifier[3] = 0;
	iLastModifier[4] = 0;
	iCopied = 0;
	iEditPoP = 0;

	if (argc > 1)
	{
		for (iTemp = 1; iTemp <= argc - 1; iTemp++)
		{
			if ((strcmp (argv[iTemp], "-h") == 0) ||
				(strcmp (argv[iTemp], "-?") == 0) ||
				(strcmp (argv[iTemp], "--help") == 0))
			{
				ShowUsage();
			}
			else if ((strcmp (argv[iTemp], "-v") == 0) ||
				(strcmp (argv[iTemp], "--version") == 0))
			{
				printf ("%s %s\n", EDITOR_NAME, EDITOR_VERSION);
				exit (EXIT_NORMAL);
			}
			else if ((strcmp (argv[iTemp], "-x") == 0) ||
				(strcmp (argv[iTemp], "--xml") == 0))
			{
				iEditPoP = 1;
				PoP1Basics();
				for (iLevel = 0; iLevel <= 15; iLevel++)
					{ LoadLevel (iLevel); CallSave (1); }
				printf ("Exported all PoP1 levels to the"
					" \"" XML_DIR "\" directory.\n");
				exit (EXIT_NORMAL);
			}
			else if ((strcmp (argv[iTemp], "-d") == 0) ||
				(strcmp (argv[iTemp], "--debug") == 0))
			{
				iDebug = 1;
			}
			else if ((strcmp (argv[iTemp], "-n") == 0) ||
				(strcmp (argv[iTemp], "--noaudio") == 0))
			{
				iNoAudio = 1;
			}
			else if ((strcmp (argv[iTemp], "-q") == 0) ||
				(strcmp (argv[iTemp], "--quiet") == 0))
			{
				iNoChomp = 1;
			}
			else if ((strcmp (argv[iTemp], "-i") == 0) ||
				(strcmp (argv[iTemp], "--improved") == 0))
			{
				snprintf (sCheat1, MAX_OPTION, "%s", "improved");
			}
			else if ((strcmp (argv[iTemp], "-m") == 0) ||
				(strcmp (argv[iTemp], "--makinit") == 0))
			{
				snprintf (sCheat2, MAX_OPTION, "%s", "makinit");
			}
			else if ((strncmp (argv[iTemp], "-a=", 3) == 0) ||
				(strncmp (argv[iTemp], "--author=", 9) == 0))
			{
				GetOptionValue (argv[iTemp], sAuthor);
				if (iDebug == 1)
					printf ("[ INFO ] Using author name: %s\n", sAuthor);
			}
			else if ((strcmp (argv[iTemp], "-z") == 0) ||
				(strcmp (argv[iTemp], "--zoom") == 0))
			{
				iScale = 2;
			}
			else if ((strncmp (argv[iTemp], "-l=", 3) == 0) ||
				(strncmp (argv[iTemp], "--level=", 8) == 0))
			{
				GetOptionValue (argv[iTemp], sStartLevel);
				iStartLevel = atoi (sStartLevel);
			}
			else if ((strncmp (argv[iTemp], "-c=", 3) == 0) ||
				(strncmp (argv[iTemp], "--cheat=", 8) == 0))
			{
				GetOptionValue (argv[iTemp], sCheat1);
				GetOptionValue (argv[iTemp], sCheat2);
				if (iDebug == 1)
					printf ("[ INFO ] Using cheat code: %s\n", sCheat1);
			}
			else if ((strcmp (argv[iTemp], "-1") == 0) ||
				(strcmp (argv[iTemp], "--pop1") == 0))
			{
				iEditPoP = 1;
			}
			else if ((strcmp (argv[iTemp], "-2") == 0) ||
				(strcmp (argv[iTemp], "--pop2") == 0))
			{
				iEditPoP = 2;
			}
			else if ((strcmp (argv[iTemp], "-s") == 0) ||
				(strcmp (argv[iTemp], "--static") == 0))
			{
				iNoAnim = 1;
			}
			else
			{
				ShowUsage();
			}
		}
	}
	SRANDOM ((unsigned)time(&tm));

	CheckRequiredFiles();
	InitScreen();
	Quit();

	return 0;
}
/*****************************************************************************/
void CheckRequiredFiles (void)
/*****************************************************************************/
{
	if (access (LEVELS_DAT, R_OK|W_OK) == -1)
	{
		printf ("[ WARN ] %s: %s!\n", LEVELS_DAT, strerror (errno));
		iPoP1 = 0;
	} else { iPoP1 = 1; }
	if (access (PRINCE_DAT, R_OK|W_OK) == -1)
	{
		printf ("[ WARN ] %s: %s!\n", PRINCE_DAT, strerror (errno));
		iPoP2 = 0;
	} else { iPoP2 = 1; }
	if ((access (PR_EXECUTABLE, R_OK) == -1) ||
		((access (PR_RESOURCES, R_OK) == -1) && (iPoP1 == 1)) ||
		((access (PR_POP2, R_OK) == -1) && (iPoP2 == 1)))
	{
		printf ("[FAILED] %s or %s or %s: %s!\n", PR_EXECUTABLE, PR_RESOURCES,
			PR_POP2, strerror (errno));
		iPR = 0;
	} else { iPR = 1; }
}
/*****************************************************************************/
int LoadPLV (char *sFileName)
/*****************************************************************************/
{
	int iFd;
	int iRoom;
	int iTile;
	int iTemp;
	int iTemp2;
	int iUD;
	int iStatic;
	int iDynamic;
	char sBinaryFore[9 + 2]; /*** 8 chars, plus \0 ***/
	char sBinaryMod[9 + 2]; /*** 8 chars, plus \0 ***/
	char sBinaryModMore[9 + 2]; /*** 8 chars, plus \0 ***/
	char sString[MAX_DATA + 2];
	char sTemp[100 + 2];
	unsigned long luNumber;
	char sReferenceNr[7 + 2];
	int iFieldOrValue;
	int iForeSize, iBackSize;
	int iForePos, iBackPos;
	int iModDone;
	char sName[7 + 2];
	char sModifiers[100 + 2];
	char sModifiersTemp[100 + 2];
	char sForegrounds[100 + 2];
	char sForegroundsTemp[100 + 2];

	/*** For PoP2 luLevelNr. ***/
	char cFirst;
	char cSecond;
	char sFirstSecond[MAX_TEXT + 2];
	int iFirstSecond;

	/*** Everything on the foreground of the level. ***/
	unsigned char sLevelForeground[960 + 2]; /*** PoP1 only uses 720. ***/
	/*** The modifiers of the level. ***/
	unsigned char sLevelModifier[3840 + 2]; /*** PoP1 only uses 720. ***/

	iChanged = 0;

	/*** Open file. ***/
	iFd = open (sFileName, O_RDONLY|O_BINARY);
	if (iFd == -1)
	{
		printf ("[FAILED] Could not open %s: %s!\n", sFileName, strerror (errno));
		printf ("[ INFO ] PR did not (or incorrectly) export this level\n\n");
		exit (EXIT_ERROR);
	}

	/*** Check the Magic ID. ***/
	ReadFromFile (iFd, "Magic ID", 7, sMagicID);
	snprintf (sString, MAX_DATA, "%s", sMagicID);
	if (strcmp (sString, "POP_LVL") != 0)
	{ printf ("[FAILED] This is not a POP level!\n"); exit (EXIT_ERROR); }

	/*** Check the POP Version. ***/
	ReadFromFile (iFd, "POP Version", 1, sPOPVersion);
	if ((iEditPoP == 1) && (sPOPVersion[0] != 0x01))
		{ printf ("[FAILED] This is not a PoP1 level!\n"); exit (EXIT_ERROR); }
	if ((iEditPoP == 2) && (sPOPVersion[0] != 0x02))
		{ printf ("[FAILED] This is not a PoP2 level!\n"); exit (EXIT_ERROR); }

	/*** Check the PLV Version. ***/
	ReadFromFile (iFd, "PLV Version", 1, sPLVVersion);
	if ((sPLVVersion[0] != 0x01) && (iDebug == 1))
	{ printf ("[ INFO ] Strange version detected: %02x\n", sPLVVersion[0]); }

	/*** Load the level number. ***/
	ReadFromFile (iFd, "Level Number", 1, sLevelNumber);
	if (iEditPoP == 1)
	{
		snprintf (sString, MAX_DATA, "%02x", sLevelNumber[0]);
		luLevelNr = strtoul (sString, NULL, 16);
	} else {
		cFirst = sFileName[strlen (sFileName) - 6];
		cSecond = sFileName[strlen (sFileName) - 5];
		snprintf (sFirstSecond, MAX_TEXT, "%c%c", cFirst, cSecond);
		iFirstSecond = atoi (sFirstSecond);
		if ((iFirstSecond >= 0) && (iFirstSecond <= 13))
			{ luLevelNr = iFirstSecond + 1; }
				else { luLevelNr = iFirstSecond - 5; }
	}
	if (iDebug == 1)
	{
		printf ("[ INFO ] The level number is: %lu\n", luLevelNr);
	}

	/*** Load the number of fields. ***/
	ReadFromFile (iFd, "Number Of Fields", 4, sNumberOfFields);
	snprintf (sString, MAX_DATA, "%02x%02x%02x%02x", sNumberOfFields[3],
		sNumberOfFields[2], sNumberOfFields[1], sNumberOfFields[0]);
	luNumber = strtoul (sString, NULL, 16);
	luNumberOfFields = luNumber;
	if (iDebug == 1)
	{
		printf ("[ INFO ] The number of user data fields is: %lu\n", luNumber);
	}

	/*** Load the level size. ***/
	ReadFromFile (iFd, "Level Size", 4, sLevelSize);
	snprintf (sString, MAX_DATA, "%02x%02x%02x%02x", sLevelSize[3],
		sLevelSize[2], sLevelSize[1], sLevelSize[0]);
	luLevelSize = strtoul (sString, NULL, 16);
	if (((iEditPoP == 1) && ((luLevelSize < 2304) || (luLevelSize > 2305))) ||
		((iEditPoP == 2) && (luLevelSize != 12025)))
	{
		printf ("[FAILED] Level has an incorrect size: %lu!\n", luLevelSize);
		exit (EXIT_ERROR);
	}
	if (iDebug == 1)
	{
		printf ("[ INFO ] The level size is: %lu\n", luLevelSize);
	}

	/*** Load the checksum. ***/
	ReadFromFile (iFd, "Checksum", 1, sChecksum);

	/*** Load the level foreground and modifier. ***/
	snprintf (sForegrounds, 100, "%s", "");
	snprintf (sModifiers, 100, "%s", "");
	if (iEditPoP == 1)
		{ iForeSize = 720; iBackSize = 720; }
			else { iForeSize = 960; iBackSize = 3840; }
	ReadFromFile (iFd, "Level Foreground", iForeSize, sLevelForeground);
	ReadFromFile (iFd, "Level Modifier", iBackSize, sLevelModifier);
	for (iRoom = 1; iRoom <= iRooms; iRoom++)
	{
		if (iDebug == 1) { printf ("\nRoom %i:\n\n", iRoom); }
		for (iTile = 0; iTile <= 29; iTile++)
		{
			iForePos = ((iRoom * 30) - 30) + iTile;
			if (iEditPoP == 1)
				{ iBackPos = iForePos; }
					else { iBackPos = ((iRoom * (30 * 4)) - (30 * 4)) + (iTile * 4); }

			/*** foreground ***/
			GetAsEightBits (sLevelForeground[iForePos], sBinaryFore);
			GetForegroundAsName (sBinaryFore, sName);
			iThingA[iRoom][iTile] = (int)sLevelForeground[iForePos];

			/*** Add to sForegrounds. ***/
			if (strcmp (sForegrounds, "") == 0)
			{
				snprintf (sForegrounds, 100, "%s", sName);
			} else {
				snprintf (sForegroundsTemp, 100, "%s", sForegrounds);
				snprintf (sForegrounds, 100, "%s|%s", sForegroundsTemp, sName);
			}

			/*** modifier ***/
			GetAsEightBits (sLevelModifier[iBackPos], sBinaryMod);
			if (iEditPoP == 2)
			{
				GetAsEightBits (sLevelModifier[iBackPos + 1], sBinaryModMore);
				iModifierA[iRoom][iTile][2] = BitsToInt (sBinaryModMore);
				GetAsEightBits (sLevelModifier[iBackPos + 2], sBinaryModMore);
				iModifierA[iRoom][iTile][3] = BitsToInt (sBinaryModMore);
				GetAsEightBits (sLevelModifier[iBackPos + 3], sBinaryModMore);
				iModifierA[iRoom][iTile][4] = BitsToInt (sBinaryModMore);
			}

			iModDone = 0;

			/*** gate or door ***/
			if ((iThingA[iRoom][iTile] == 4) ||
				(iThingA[iRoom][iTile] == 36))
			{
				if (iDebug == 1)
				{
					switch (BitsToInt (sBinaryMod))
					{
						case 1:
							snprintf (sName, 100, "%s", "Open   ");
							break;
						default:
							snprintf (sName, 100, "%s", "Closed ");
							break;
					}
				}
				iModifierA[iRoom][iTile][1] = BitsToInt (sBinaryMod);
				iModDone = 1;
			}

			/*** drop or raise ***/
			if ((iThingA[iRoom][iTile] == 6) ||
				(iThingA[iRoom][iTile] == 15) ||
				(iThingA[iRoom][iTile] == 38) ||
				(iThingA[iRoom][iTile] == 47))
			{
				snprintf (sString, 3 + 6, "%i      ",
					BitsToInt (sBinaryMod) + 1);
				strncpy (sReferenceNr, sString, 7);
				sReferenceNr[7] = '\0';
				snprintf (sName, 100, "%s", sReferenceNr);
				iModifierA[iRoom][iTile][1] = BitsToInt (sBinaryMod);
				iModDone = 1;
			}

			/*** the rest ***/
			if (iModDone == 0)
			{
				GetModifierAsName (BitsToInt (sBinaryFore),
					BitsToInt (sBinaryMod), sName);
				iModifierA[iRoom][iTile][1] = (int)sLevelModifier[iBackPos];
			}

			/*** Add to sModifiers. ***/
			if (strcmp (sModifiers, "") == 0)
			{
				snprintf (sModifiers, 100, "%s", sName);
			} else {
				snprintf (sModifiersTemp, 100, "%s", sModifiers);
				snprintf (sModifiers, 100, "%s|%s", sModifiersTemp, sName);
			}

			/*** Display the room. ***/
			if (iDebug == 1)
			{
				if ((iTile == 9) || (iTile == 19) || (iTile == 29))
				{
					printf ("%s\n", sForegrounds);
					printf ("%s\n", sModifiers);
					if ((iTile == 9) || (iTile == 19))
					{
						for (iTemp = 1; iTemp <= 79; iTemp++) { printf ("-"); }
						printf ("\n");
					}
					snprintf (sForegrounds, 100, "%s", "");
					snprintf (sModifiers, 100, "%s", "");
				}
			}
		}
		PrIfDe ("\n");
	}

	/*** Load first and second door events. ***/
	if (iEditPoP == 1)
	{
		ReadFromFile (iFd, "First Door Events", 256, sFirstDoorEvents);
		ReadFromFile (iFd, "Second Door Events", 256, sSecondDoorEvents);
	} else {
		ReadFromFile (iFd, "PoP2 Door Events", 1280, sPoP2DoorEvents);

		for (iTemp = 0; iTemp < 256; iTemp++)
		{
			iPoP2DoorRoom[iTemp] = sPoP2DoorEvents[(iTemp * 5) + 0];
			iPoP2DoorLocation[iTemp] = sPoP2DoorEvents[(iTemp * 5) + 1];
			iPoP2DoorType[iTemp] = sPoP2DoorEvents[(iTemp * 5) + 2];
			iPoP2DoorLeft[iTemp] = sPoP2DoorEvents[(iTemp * 5) + 3];
			iPoP2DoorRight[iTemp] = sPoP2DoorEvents[(iTemp * 5) + 4];

			/*** Some events in (the original) PoP2 point to non-existing tiles. ***/
			if ((iPoP2DoorRoom[iTemp] < 1) || (iPoP2DoorRoom[iTemp] > iRooms))
				{ iPoP2DoorRoom[iTemp] = 1; }
			if ((iPoP2DoorLocation[iTemp] < 0) || (iPoP2DoorLocation[iTemp] > 29))
				{ iPoP2DoorLocation[iTemp] = 0; }
		}
	}
	if (iDebug == 1)
	{
		if (iEditPoP == 1)
		{
			for (iTemp = 0; iTemp < 256; iTemp++)
			{
				printf ("[ INFO ] Event %i changes the door in room: %i, location:"
					" %i.", iTemp + 1, EventInfo (iTemp, 1), EventInfo (iTemp, 2));
				switch (EventInfo (iTemp, 3))
				{
					case 1: printf (" (next: yes)\n"); break;
					case 0: printf (" (next: no)\n"); break;
				}
			}
		} else {
			for (iTemp = 0; iTemp < 256; iTemp++)
			{
				printf ("[ INFO ] Event %i changes room: %i, loc: %i."
					" (t: %i, l: %i, r: %i)\n", iTemp + 1, iPoP2DoorRoom[iTemp],
					iPoP2DoorLocation[iTemp], iPoP2DoorType[iTemp],
					iPoP2DoorLeft[iTemp], iPoP2DoorRight[iTemp]);
			}
		}
	}

	/*** Load the room links. ***/
	ReadFromFile (iFd, "Room Links", iRoomLinks, sRoomLinks);
	for (iTemp = 0; iTemp < iRoomLinks; iTemp+=4)
	{
		iRoomConnections[(iTemp / 4) + 1][1] =
			sRoomLinks[iTemp]; /*** left ***/
		iRoomConnections[(iTemp / 4) + 1][2] =
			sRoomLinks[iTemp + 1]; /*** right ***/
		iRoomConnections[(iTemp / 4) + 1][3] =
			sRoomLinks[iTemp + 2]; /*** up ***/
		iRoomConnections[(iTemp / 4) + 1][4] =
			sRoomLinks[iTemp + 3]; /*** down ***/
		if (iDebug == 1)
		{
			printf ("[ INFO ] Room %i is connected to room (0 = none): l%i, "
				"r%i, u%i, d%i\n", (iTemp / 4) + 1, sRoomLinks[iTemp],
				sRoomLinks[iTemp + 1], sRoomLinks[iTemp + 2], sRoomLinks[iTemp + 3]);
		}
	}

	if (iEditPoP == 1)
	{
		/*** Load Unknown I. ***/
		ReadFromFile (iFd, "Unknown I", 64, sUnknownI);
		/*** I want remapped modifiers in all 24 rooms. ***/
		sUnknownI[0] = 0x18;
	} else {
		ReadFromFile (iFd, "Last Room", 1, sLastRoom);
		ReadFromFile (iFd, "JEFF", 4, sJEFF);
		ReadFromFile (iFd, "Level Type", 1, sLevelType);
		ReadFromFile (iFd, "Unknown I", 1, sUnknownI);
		ReadFromFile (iFd, "Level Number Events", 1, sLevelNumberEvents);
		ReadFromFile (iFd, "Extra Resources", 24, sExtraImgResources);

		if (iDebug == 1)
		{
			printf ("[ INFO ] Extra resources: ");
			for (iTemp = 0; iTemp < 24; iTemp++)
			{
				printf ("%i", sExtraImgResources[iTemp]);
				if (iTemp != 23) { printf (","); }
			}
			printf ("\n");
		}
	}

	/*** Load Start Position. ***/
	ReadFromFile (iFd, "Start Position", 3, sStartPosition);
	snprintf (sString, MAX_DATA, "%02x", sStartPosition[0]);
	luKidRoom = strtoul (sString, NULL, 16);
	snprintf (sString, MAX_DATA, "%02x", sStartPosition[1]);
	luKidPos = strtoul (sString, NULL, 16) + 1;
	snprintf (sString, MAX_DATA, "%02x", sStartPosition[2]);
	luKidDir = strtoul (sString, NULL, 16);
	/*** 1 of 4 ***/
	if (iEditPoP == 1)
	{
		if (((int)luLevelNr == 1) || ((int)luLevelNr == 13))
		{
			if (luKidDir == 0) { luKidDir = 255; } else { luKidDir = 0; }
		}
	} else {
		if ((luLevelNr == 2) || (luLevelNr == 16) ||
			(((luLevelNr == 1) || (luLevelNr == 15)) && (luKidRoom == 4)) ||
			(((luLevelNr == 6) || (luLevelNr == 20)) && (luKidRoom == 27)) ||
			(((luLevelNr == 10) || (luLevelNr == 24)) && (luKidRoom == 22)) ||
			(((luLevelNr == 14) || (luLevelNr == 28)) && (luKidRoom == 1)))
		{
			if (luKidDir == 0) { luKidDir = 255; } else { luKidDir = 0; }
		}
	}
	if (iDebug == 1)
	{
		printf ("[ INFO ] The kid starts in room: %lu, position: %lu, turned: "
			"%c\n", luKidRoom, luKidPos, cShowDirection ((int)luKidDir));
	}

	PrIfDe ("[  OK  ] Checking for broken room links.\n");
	iBrokenRoomLinks = BrokenRoomLinks (iRoomConnections, 1);

	/*** Load Unknown II-IVd, guard data, checkpoints, music. ***/
	if (iEditPoP == 1)
	{
		switch ((int)luLevelNr)
		{
			case 15: /*** potions level ***/
				ReadFromFile (iFd, "Unknown II and III", 3, sUnknownIIandIII); break;
			default:
				ReadFromFile (iFd, "Unknown II and III", 4, sUnknownIIandIII); break;
		}
		ReadFromFile (iFd, "Guard Locations", iRooms, sGuardLocations);
		ReadFromFile (iFd, "Guard Directions", iRooms, sGuardDirections);
		ReadFromFile (iFd, "Unknown IVa and IVb", 48, sUnknownIVaandIVb);
		ReadFromFile (iFd, "Guard Skills", iRooms, sGuardSkills);
		ReadFromFile (iFd, "Unknown IVc", 24, sUnknownIVc);
		ReadFromFile (iFd, "Guard Colors", iRooms, sGuardColors);
		ReadFromFile (iFd, "Unknown IVd", 16, sUnknownIVd);
	} else {
		ReadFromFile (iFd, "Unknown II", 4, sUnknownII);
		ReadFromFile (iFd, "Static Guards", iRooms * 116, sStaticGuards);
		ReadFromFile (iFd, "Check Points", 18, sCheckPoints);
		ReadFromFile (iFd, "Dynamic Guards", iRooms * 34, sDynamicGuards);
		ReadFromFile (iFd, "Music", iRooms * 30, sMusic);

		for (iTemp = 0; iTemp < iRooms; iTemp++)
		{
			iStaticGuards_Amount[iTemp] = sStaticGuards[(iTemp * 116) + 0];
			for (iStatic = 0; iStatic < STATIC; iStatic++)
			{
				iStaticGuards_1_Locations[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 1];
				iStaticGuards_2_Offsets1[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 2];
				iStaticGuards_3_Offsets2[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 3];
				iStaticGuards_4_Directions[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 4];
				iStaticGuards_5_Skills[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 5];
				iStaticGuards_6_Mod[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 6];
				iStaticGuards_7_Unknown[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 7];
				iStaticGuards_8_Unknown[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 8];
				iStaticGuards_9_Unknown[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 9];
				iStaticGuards_10_Colors[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 10];
				iStaticGuards_11_Number[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 11];
				iStaticGuards_12_Unknown[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 12];
				iStaticGuards_13_Unknown[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 13];
				iStaticGuards_14_Unknown[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 14];
				iStaticGuards_15_Unknown[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 15];
				iStaticGuards_16_Type[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 16];
				iStaticGuards_17_Hitpoints[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 17];
				if (iStaticGuards_17_Hitpoints[iTemp][iStatic] > 128)
				{
					iStaticGuards_17_Hitpoints[iTemp][iStatic]-=128;
				}
				iStaticGuards_18_SkelAct1[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 18];
				iStaticGuards_19_SkelAct2[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 19];
				iStaticGuards_20_Unknown[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 20];
				iStaticGuards_21_Unknown[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 21];
				iStaticGuards_22_Unknown[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 22];
				iStaticGuards_23_Unknown[iTemp][iStatic] =
					sStaticGuards[(iTemp * 116) + (iStatic * 23) + 23];
			}
		}

		for (iTemp = 0; iTemp < iRooms; iTemp++)
		{
			iDynamicGuards_Sets[iTemp] = sDynamicGuards[(iTemp * 34) + 0];
			iDynamicGuards_Skill[iTemp] = sDynamicGuards[(iTemp * 34) + 1];
			iDynamicGuards_Unknown1[iTemp] = sDynamicGuards[(iTemp * 34) + 2];
			iDynamicGuards_Unknown2[iTemp] = sDynamicGuards[(iTemp * 34) + 3];
			for (iDynamic = 0; iDynamic < DYNAMIC; iDynamic++)
			{
				iDynamicGuards_1_SetNr[iTemp][iDynamic] =
					sDynamicGuards[(iTemp * 34) + (iDynamic * 10) + 4];
				iDynamicGuards_2_MaxTog[iTemp][iDynamic] =
					sDynamicGuards[(iTemp * 34) + (iDynamic * 10) + 5];
				iDynamicGuards_3_Row[iTemp][iDynamic] =
					sDynamicGuards[(iTemp * 34) + (iDynamic * 10) + 6];
				iDynamicGuards_4_Column[iTemp][iDynamic] =
					sDynamicGuards[(iTemp * 34) + (iDynamic * 10) + 7];
				iDynamicGuards_5_LatBefore[iTemp][iDynamic] =
					sDynamicGuards[(iTemp * 34) + (iDynamic * 10) + 8];
				iDynamicGuards_6_LatBetween[iTemp][iDynamic] =
					sDynamicGuards[(iTemp * 34) + (iDynamic * 10) + 9];
				iDynamicGuards_7_CanJump[iTemp][iDynamic] =
					sDynamicGuards[(iTemp * 34) + (iDynamic * 10) + 10];
				iDynamicGuards_8_SpIfAl[iTemp][iDynamic] =
					sDynamicGuards[(iTemp * 34) + (iDynamic * 10) + 11];
				iDynamicGuards_9_Amount[iTemp][iDynamic] =
					sDynamicGuards[(iTemp * 34) + (iDynamic * 10) + 12];
				iDynamicGuards_10_Hitpoints[iTemp][iDynamic] =
					sDynamicGuards[(iTemp * 34) + (iDynamic * 10) + 13];
				if (iDynamicGuards_10_Hitpoints[iTemp][iDynamic] > 128)
				{
					iDynamicGuards_10_Hitpoints[iTemp][iDynamic]-=128;
				}
			}
		}

		for (iTemp = 0; iTemp < 18; iTemp++)
		{
			iCheckPoints[iTemp] = (int)sCheckPoints[iTemp];
		}
	}

	if (iEditPoP == 1)
	{
		/*** Load and display the end code. ***/
		ReadFromFile (iFd, "End Code", 2, sEndCode);
		snprintf (sString, MAX_DATA, "%02x%02x", sEndCode[0], sEndCode[1]);
		if (iDebug == 1)
		{
			printf ("[ INFO ] The end code is (usually 0f09): %s\n", sString);
		}
	}

	/*** Display guard information. ***/
	if (iDebug == 1)
	{
		for (iTemp = 0; iTemp < iRooms; iTemp++)
		{
			if (iEditPoP == 1)
			{
				if (sGuardLocations[iTemp] <= 29)
				{
					printf ("[ INFO ] A guard in room: %i, location: %i, turned: %c, "
						"skill: %i, color: %i\n", iTemp + 1, sGuardLocations[iTemp] + 1,
						cShowDirection (sGuardDirections[iTemp]), sGuardSkills[iTemp],
						sGuardColors[iTemp]);
					if (((sUnknownIVaandIVb[iTemp] != 255) && (sUnknownIVaandIVb[iTemp]
						!= 0)) || ((sUnknownIVaandIVb[iTemp + 24] != 255) &&
						(sUnknownIVaandIVb[iTemp + 24] != 0)) ||
						((sUnknownIVc[iTemp] != 255) && (sUnknownIVc[iTemp] != 0)))
					{
						printf ("[ INFO ] Extra data for the above guard: "
							"%i|%i|%i\n", sUnknownIVaandIVb[iTemp],
							sUnknownIVaandIVb[iTemp + 24], sUnknownIVc[iTemp]);
					}
				}
			} else {
				printf ("[ INFO ] Static guards in room %i: %i\n",
					iTemp + 1, iStaticGuards_Amount[iTemp]);
				if (iStaticGuards_Amount[iTemp] > 0)
				{
					for (iStatic = 0; iStatic < iStaticGuards_Amount[iTemp]; iStatic++)
					{
						printf ("[ INFO ] Static guard %i: %i|%i|%i|%i|%i|%i|%i|%i|%i|%i"
							"|%i|%i|%i|%i|%i|%i|%i|%i|%i|%i|%i|%i|%i\n", iStatic + 1,
							iStaticGuards_1_Locations[iTemp][iStatic],
							iStaticGuards_2_Offsets1[iTemp][iStatic],
							iStaticGuards_3_Offsets2[iTemp][iStatic],
							iStaticGuards_4_Directions[iTemp][iStatic],
							iStaticGuards_5_Skills[iTemp][iStatic],
							iStaticGuards_6_Mod[iTemp][iStatic],
							iStaticGuards_7_Unknown[iTemp][iStatic],
							iStaticGuards_8_Unknown[iTemp][iStatic],
							iStaticGuards_9_Unknown[iTemp][iStatic],
							iStaticGuards_10_Colors[iTemp][iStatic],
							iStaticGuards_11_Number[iTemp][iStatic],
							iStaticGuards_12_Unknown[iTemp][iStatic],
							iStaticGuards_13_Unknown[iTemp][iStatic],
							iStaticGuards_14_Unknown[iTemp][iStatic],
							iStaticGuards_15_Unknown[iTemp][iStatic],
							iStaticGuards_16_Type[iTemp][iStatic],
							iStaticGuards_17_Hitpoints[iTemp][iStatic],
							iStaticGuards_18_SkelAct1[iTemp][iStatic],
							iStaticGuards_19_SkelAct2[iTemp][iStatic],
							iStaticGuards_20_Unknown[iTemp][iStatic],
							iStaticGuards_21_Unknown[iTemp][iStatic],
							iStaticGuards_22_Unknown[iTemp][iStatic],
							iStaticGuards_23_Unknown[iTemp][iStatic]);
					}
				}

				printf ("[ INFO ] Dynamic guard sets in room %i: %i\n",
					iTemp + 1, iDynamicGuards_Sets[iTemp]);
				if (iDynamicGuards_Sets[iTemp] > 0)
				{
					for (iDynamic = 0; iDynamic < iDynamicGuards_Sets[iTemp]; iDynamic++)
					{
						printf ("[ INFO ] Dynamic guard %i: "
							"%i|%i|%i|%i|%i|%i|%i|%i|%i|%i\n",
							iDynamic + 1,
							iDynamicGuards_1_SetNr[iTemp][iDynamic],
							iDynamicGuards_2_MaxTog[iTemp][iDynamic],
							iDynamicGuards_3_Row[iTemp][iDynamic],
							iDynamicGuards_4_Column[iTemp][iDynamic],
							iDynamicGuards_5_LatBefore[iTemp][iDynamic],
							iDynamicGuards_6_LatBetween[iTemp][iDynamic],
							iDynamicGuards_7_CanJump[iTemp][iDynamic],
							iDynamicGuards_8_SpIfAl[iTemp][iDynamic],
							iDynamicGuards_9_Amount[iTemp][iDynamic],
							iDynamicGuards_10_Hitpoints[iTemp][iDynamic]);
					}
				}
			}
		}
	}

	/*** Display checkpoint information. ***/
	if ((iDebug == 1) && (iEditPoP == 2))
	{
		for (iTemp = 0; iTemp < 9; iTemp++)
		{
			printf ("[ INFO ] Checkpoint %i: %i, %i\n", iTemp + 1,
				iCheckPoints[(iTemp * 2) + 0], iCheckPoints[(iTemp * 2) + 1]);
		}
	}

	/* Display a warning if the PoP2 guard number differs from the guard's
	 * number. Never for (the original) PoP2, and if another level editor
	 * broke this, apoplexy will fix it when saving levels.
	 */
	if (iEditPoP == 2)
	{
		for (iTemp = 0; iTemp < iRooms; iTemp++)
		{
			if (iStaticGuards_Amount[iTemp] > 0)
			{
				for (iStatic = 0; iStatic < iStaticGuards_Amount[iTemp]; iStatic++)
				{
					if (iStatic != iStaticGuards_11_Number[iTemp][iStatic])
					{
						printf ("[ WARN ] Guard number differs from guard's number:"
							" %i and %i\n", iStatic,
							iStaticGuards_11_Number[iTemp][iStatic]);
					}
				}
			}
		}
	}

	/*** Read the user data. ***/
	ReadFromFile (iFd, "User Data Count", 4, sUserDataCount);
	snprintf (sString, MAX_DATA, "%02x%02x%02x%02x", sUserDataCount[3],
		sUserDataCount[2], sUserDataCount[1], sUserDataCount[0]);
	luNumber = strtoul (sString, NULL, 16);
	if (iDebug == 1)
	{
		printf ("[ INFO ] The user data count is: %lu\n", luNumber);
	}
	ReadFromFile (iFd, "User Data", (int)luNumber, sUserData);
	snprintf (sString, 100, "%s", "");
	iInformationNr = 1;
	iFieldOrValue = 0;
	for (iTemp = 0; iTemp < (int)luNumber; iTemp++)
	{
		if (sUserData[iTemp] == '\0')
		{
			snprintf (sInformation[iInformationNr][iFieldOrValue], 100, "%s",
				sString);
			snprintf (sString, 100, "%s", "");
			switch (iFieldOrValue)
			{
				case 0:
					iFieldOrValue = 1; break;
				case 1:
					iInformationNr++; iFieldOrValue = 0; break;
			}
		} else {
			snprintf (sTemp, 100, "%s", sString);
			snprintf (sString, 100, "%s%c", sTemp, sUserData[iTemp]);
		}
	}
	iUD = 0;
	iTemp2 = 0;
	for (iTemp = 1; iTemp < iInformationNr; iTemp++)
	{
		if (strcmp (sInformation[iTemp][0], "Editor Name") == 0)
		{
			iUD = iUD + 1;
			if (strcmp (sInformation[iTemp][1], "PR") == 0) { iTemp2++; }
		}
		if (strcmp (sInformation[iTemp][0], "Editor Version") == 0)
		{
			iUD = iUD + 2;
			if (strcmp (sInformation[iTemp][1], "v1.3.1") != 0) { iTemp2++; }
		}
		if (strcmp (sInformation[iTemp][0], "Level Author") == 0)
			{ iUD = iUD + 4; }
		if (strcmp (sInformation[iTemp][0], "Level Title") == 0)
			{ iUD = iUD + 8; }
		if (strcmp (sInformation[iTemp][0], "Level Description") == 0)
			{ iUD = iUD + 16; }
		if (strcmp (sInformation[iTemp][0], "Time Created") == 0)
			{ iUD = iUD + 32; }
		if (strcmp (sInformation[iTemp][0], "Time Last Modified") == 0)
			{ iUD = iUD + 64; }
		if (strcmp (sInformation[iTemp][0], "Original Filename") == 0)
			{ iUD = iUD + 128; }
		if (strcmp (sInformation[iTemp][0], "Original Level Number") == 0)
			{ iUD = iUD + 256; }
		if (iDebug == 1)
		{
			printf ("[ INFO ] %s: %s\n", sInformation[iTemp][0],
				sInformation[iTemp][1]);
		}
	}
	if ((iUD != 511) && (iDebug == 1))
	{
		printf ("[ WARN ] Could not find required User Data: %i\n", iUD);
	}
	if (iTemp2 == 2)
	{
		printf ("[ WARN ] Please use PR 1.3.1 with apoplexy.\n");
	}

	iCurRoom = (int)luKidRoom;
	iMod3Hi = 0;
	iMod4Hi = 0;
	close (iFd);

	return (0);
}
/*****************************************************************************/
int EventInfo (int iNr, int iType)
/*****************************************************************************/
{
	char sBinaryFDoors[9 + 2]; /*** 8 chars, plus \0 ***/
	char sBinarySDoors[9 + 2]; /*** 8 chars, plus \0 ***/
	char sTemp[MAX_DATA + 2];

	GetAsEightBits (sFirstDoorEvents[iNr], sBinaryFDoors);
	GetAsEightBits (sSecondDoorEvents[iNr], sBinarySDoors);

	switch (iType)
	{
		case 1: /*** room ***/
			snprintf (sTemp, MAX_DATA, "%c%c%c%c%c", sBinarySDoors[0],
				sBinarySDoors[1], sBinarySDoors[2], sBinaryFDoors[1],
				sBinaryFDoors[2]);
			return (BitsToInt (sTemp)); break;
		case 2: /*** location ***/
			snprintf (sTemp, MAX_DATA, "%c%c%c%c%c", sBinaryFDoors[3],
				sBinaryFDoors[4], sBinaryFDoors[5], sBinaryFDoors[6],
				sBinaryFDoors[7]);
			return (BitsToInt (sTemp) + 1); break;
		case 3: /*** next ***/
			switch (sBinaryFDoors[0])
			{
				case '0': return (1); break;
				case '1': return (0); break;
			}
		default: return (-1); break;
	}
}
/*****************************************************************************/
int ReadFromFile (int iFd, char *sWhat, int iSize, unsigned char *sRetString)
/*****************************************************************************/
{
	int iLength;
	int iRead;
	char sRead[1 + 2];
	int iEOF;

	if (iDebug == 1)
	{
		printf ("[ INFO ] Loading: %s\n", sWhat);
	}
	iLength = 0;
	iEOF = 0;
	do {
		iRead = read (iFd, sRead, 1);
		switch (iRead)
		{
			case -1: printf ("[FAILED] Could not read!\n"); exit (EXIT_ERROR); break;
			case 0: PrIfDe ("[ INFO ] End of level file\n"); iEOF = 1; break;
			default:
				sRetString[iLength] = sRead[0];
				iLength++;
				break;
		}
	} while ((iLength < iSize) && (iEOF == 0));
	sRetString[iLength] = '\0';

	return (iLength);
}
/*****************************************************************************/
void GetAsEightBits (unsigned char cChar, char *sBinary)
/*****************************************************************************/
{
	int i = CHAR_BIT;
	int iTemp;

	iTemp = 0;
	while (i > 0)
	{
		i--;
		if (cChar&(1 << i))
		{
			sBinary[iTemp] = '1';
		} else {
			sBinary[iTemp] = '0';
		}
		iTemp++;
	}
	sBinary[iTemp] = '\0';
}
/*****************************************************************************/
int BitsToInt (char *sString)
/*****************************************************************************/
{
	/*** Converts binary to decimal. ***/
	/*** Example: 11111111 to 255 ***/

	int iTemp = 0;

	for (; *sString; iTemp = (iTemp << 1) | (*sString++ - '0'));
	return (iTemp);
}
/*****************************************************************************/
void GetForegroundAsName (char *sBinaryFore, char *sName)
/*****************************************************************************/
{
	char sPart[6 + 2];

	strncpy (sPart, sBinaryFore + 3, 5);
	sPart[5]='\0';
	if (strcmp (sPart, "00000") == 0) { snprintf (sName, 9, "%s", "Empty  "); }
	if (strcmp (sPart, "00001") == 0) { snprintf (sName, 9, "%s", "Floor  "); }
	if (strcmp (sPart, "00010") == 0) { snprintf (sName, 9, "%s", "Spikes "); }
	if (strcmp (sPart, "00011") == 0) { snprintf (sName, 9, "%s", "Pillar "); }
	if (strcmp (sPart, "00100") == 0) { snprintf (sName, 9, "%s", "Gate   "); }
	if (strcmp (sPart, "00101") == 0) { snprintf (sName, 9, "%s", "StckBtn"); }
	if (strcmp (sPart, "00110") == 0) { snprintf (sName, 9, "%s", "DropBtn"); }
	if (strcmp (sPart, "00111") == 0) { snprintf (sName, 9, "%s", "Tpstry "); }
	if (strcmp (sPart, "01000") == 0) { snprintf (sName, 9, "%s", "BBgpilr"); }
	if (strcmp (sPart, "01001") == 0) { snprintf (sName, 9, "%s", "TBgpilr"); }
	if (strcmp (sPart, "01010") == 0) { snprintf (sName, 9, "%s", "Potion "); }
	if (strcmp (sPart, "01011") == 0)
	{
		if (sBinaryFore[2] == '1')
			{ snprintf (sName, 9, "%s", "LoosStk"); }
				else { snprintf (sName, 9, "%s", "LoosBrd"); }
	}
	if (strcmp (sPart, "01100") == 0) { snprintf (sName, 9, "%s", "TpstryT"); }
	if (strcmp (sPart, "01101") == 0) { snprintf (sName, 9, "%s", "Mirror "); }
	if (strcmp (sPart, "01110") == 0) { snprintf (sName, 9, "%s", "Debris "); }
	if (strcmp (sPart, "01111") == 0) { snprintf (sName, 9, "%s", "RaisBtn"); }
	if (strcmp (sPart, "10000") == 0) { snprintf (sName, 9, "%s", "ExitLft"); }
	if (strcmp (sPart, "10001") == 0) { snprintf (sName, 9, "%s", "ExitRgt"); }
	if (strcmp (sPart, "10010") == 0) { snprintf (sName, 9, "%s", "Chomper"); }
	if (strcmp (sPart, "10011") == 0) { snprintf (sName, 9, "%s", "Torch  "); }
	if (strcmp (sPart, "10100") == 0) { snprintf (sName, 9, "%s", "Wall   "); }
	if (strcmp (sPart, "10101") == 0) { snprintf (sName, 9, "%s", "Skeletn"); }
	if (strcmp (sPart, "10110") == 0) { snprintf (sName, 9, "%s", "Sword  "); }
	if (strcmp (sPart, "10111") == 0) { snprintf (sName, 9, "%s", "BcnyLft"); }
	if (strcmp (sPart, "11000") == 0) { snprintf (sName, 9, "%s", "BcnyRgt"); }
	if (strcmp (sPart, "11001") == 0) { snprintf (sName, 9, "%s", "LcePilr"); }
	if (strcmp (sPart, "11010") == 0) { snprintf (sName, 9, "%s", "LceSprt"); }
	if (strcmp (sPart, "11011") == 0) { snprintf (sName, 9, "%s", "SmalLce"); }
	if (strcmp (sPart, "11100") == 0) { snprintf (sName, 9, "%s", "LceLft "); }
	if (strcmp (sPart, "11101") == 0) { snprintf (sName, 9, "%s", "LceRgt "); }
	if (strcmp (sPart, "11110") == 0) { snprintf (sName, 9, "%s", "TorchDb"); }
	if (strcmp (sPart, "11111") == 0) { snprintf (sName, 9, "%s", "Null   "); }
}
/*****************************************************************************/
void GetModifierAsName (int iFore, int iMod, char *sName)
/*****************************************************************************/
{
	int iForeR;

	iForeR = iFore;
	if (iForeR >= 192) { iForeR-=192; }
	if (iForeR >= 128) { iForeR-=128; }
	if (iForeR >= 64) { iForeR-=64; }
	if (iForeR >= 32) { iForeR-=32; }

	snprintf (sName, 9, "%s", "???????");
	switch (iForeR)
	{
		case 0:
			switch (iMod)
			{
				case 0: snprintf (sName, 9, "%s", "+Not-Bl"); break;
				case 1: snprintf (sName, 9, "%s", "+Spt1-N"); break;
				case 2: snprintf (sName, 9, "%s", "+Spt2-D"); break;
				case 3: snprintf (sName, 9, "%s", "Window "); break;
				case 6: snprintf (sName, 9, "%s", "POTIONS"); break;
				case 78: snprintf (sName, 9, "%s", "POTIONS"); break;
				case 144: snprintf (sName, 9, "%s", "POTIONS"); break;
				case 255: snprintf (sName, 9, "%s", "+Spt3-B"); break;
			}
			break;
		case 1:
			switch (iMod)
			{
				case 0: snprintf (sName, 9, "%s", "+Not-Bl"); break;
				case 1: snprintf (sName, 9, "%s", "+Spt1-N"); break;
				case 2: snprintf (sName, 9, "%s", "+Spt2-D"); break;
				case 3: snprintf (sName, 9, "%s", "Window "); break;
				case 6: snprintf (sName, 9, "%s", "POTIONS"); break;
				case 78: snprintf (sName, 9, "%s", "POTIONS"); break;
				case 144: snprintf (sName, 9, "%s", "POTIONS"); break;
				case 255: snprintf (sName, 9, "%s", "+Spt3-B"); break;
			}
			break;
		case 2:
			switch (iMod)
			{
				case 0: snprintf (sName, 9, "%s", "Normal "); break;
				case 1: snprintf (sName, 9, "%s", "BarelyO"); break;
				case 2: snprintf (sName, 9, "%s", "HalfOut"); break;
				case 3: snprintf (sName, 9, "%s", "FulyOut"); break;
				case 4: snprintf (sName, 9, "%s", "FulyOut"); break;
				case 5: snprintf (sName, 9, "%s", "Out    "); break;
				case 6: snprintf (sName, 9, "%s", "Out    "); break;
				case 7: snprintf (sName, 9, "%s", "HalfOut"); break;
				case 8: snprintf (sName, 9, "%s", "BarelyO"); break;
				case 9: snprintf (sName, 9, "%s", "Disabld"); break;
			}
			break;
		case 3: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 4:
			switch (iMod)
			{
				/*** says the documentation ***/
				case 0: snprintf (sName, 9, "%s", "Closed "); break;
				case 1: snprintf (sName, 9, "%s", "Open   "); break;
				/*** says the game ***/
				case 2: snprintf (sName, 9, "%s", "Closed "); break;
			}
			break;
		case 5: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 6: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 7:
			switch (iMod)
			{
				case 0: snprintf (sName, 9, "%s", "-WithLt"); break;
				case 1: snprintf (sName, 9, "%s", "-AlterD"); break;
				case 2: snprintf (sName, 9, "%s", "-Normal"); break;
				case 3: snprintf (sName, 9, "%s", "-Black "); break;
				case 4: snprintf (sName, 9, "%s", "-Black "); break;
				case 5: snprintf (sName, 9, "%s", "-AltDes"); break;
				case 6: snprintf (sName, 9, "%s", "-WthBtm"); break;
				case 7: snprintf (sName, 9, "%s", "-WthWin"); break;
			}
			break;
		case 8: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 9: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 10:
			switch (iMod)
			{
				case 0: snprintf (sName, 9, "%s", "Empty  "); break;
				case 1: snprintf (sName, 9, "%s", "HealthP"); break;
				case 2: snprintf (sName, 9, "%s", "Life   "); break;
				case 3: snprintf (sName, 9, "%s", "FeatFal"); break;
				case 4: snprintf (sName, 9, "%s", "Invert "); break;
				case 5: snprintf (sName, 9, "%s", "Poison "); break;
				case 6: snprintf (sName, 9, "%s", "Open   "); break;
				case 32: snprintf (sName, 9, "%s", "PRINCES"); break;
				case 64: snprintf (sName, 9, "%s", "PRINCES"); break;
				case 160: snprintf (sName, 9, "%s", "PRINCES"); break;
			}
			break;
		case 11: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 12:
			switch (iMod)
			{
				case 0: snprintf (sName, 9, "%s", "-WithLt"); break;
				case 1: snprintf (sName, 9, "%s", "-AlterD"); break;
				case 2: snprintf (sName, 9, "%s", "-Normal"); break;
				case 3: snprintf (sName, 9, "%s", "-Black "); break;
			}
			break;
		case 13: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 14: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 15: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 16: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 17:
			switch (iMod)
			{
				case 0: snprintf (sName, 9, "%s", "Always "); break;
				case 255: snprintf (sName, 9, "%s", "POTIONS"); break;
			}
			break;
		case 18:
			switch (iMod)
			{
				case 0: snprintf (sName, 9, "%s", "Normal "); break;
				case 1: snprintf (sName, 9, "%s", "HalfOpn"); break;
				case 2: snprintf (sName, 9, "%s", "Closed "); break;
				case 3: snprintf (sName, 9, "%s", "PartOpn"); break;
				case 4: snprintf (sName, 9, "%s", "ExtrOpn"); break;
				case 5: snprintf (sName, 9, "%s", "StckOpn"); break;
			}
			break;
		case 19: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 20:
			switch (iMod)
			{
				case 0: snprintf (sName, 9, "%s", "+Norm-B"); break;
				case 1: snprintf (sName, 9, "%s", "+Norm-N"); break;
			}
			break;
		case 21: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 22: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 23: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 24: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 25: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 26: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 27: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 28: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 29: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 30: if (iMod == 0) { snprintf (sName, 9, "%s", "Always "); } break;
		case 31:
			switch (iMod)
			{
				case 0: snprintf (sName, 9, "%s", "Always "); break;
				case 253: snprintf (sName, 9, "%s", "POTIONS"); break;
				case 255: snprintf (sName, 9, "%s", "POTIONS"); break;
			}
			break;
	}
}
/*****************************************************************************/
char cShowDirection (int iDirection)
/*****************************************************************************/
{
	switch (iDirection)
	{
		case 0: return ('r'); break;
		case 255: return ('l'); break;
		default: return ('?'); break;
	}
}
/*****************************************************************************/
void ShowImage (int iThing, int iModifier[], SDL_Surface *screen,
	int iLocation, int iFromImageX, int iFromImageY,
	int iFromImageWidth, int iFromImageHeight)
/*****************************************************************************/
{
	SDL_Rect dest;
	SDL_Rect loc;
	char sAdj[MAX_ADJ + 2];
	char sAdjR[MAX_ADJ + 2];
	char sAdjL[MAX_ADJ + 2];
	char sGrid[MAX_GRID + 2];
	char sGridX[MAX_GRID + 2];
	char sGridY[MAX_GRID + 2];
	char sText[MAX_TEXT + 2];
	char sCustomTile[MAX_CUSTOM + 2];
	int iTemp;
	int iTemp2;
	int iTile;
	int iMod1;
	int iMod2;
	int iDirection;
	int iBaseY;
	int iUseSprite;

	loc.x = (iFromImageX * iScale);
	loc.y = (iFromImageY * iScale);
	loc.w = (iFromImageWidth * iScale);
	loc.h = (iFromImageHeight * iScale);

	switch (iLocation)
	{
		case -1: dest.x = HOR0; dest.y = VER1; break;
		case -2: dest.x = HOR0; dest.y = VER2; break;
		case -3: dest.x = HOR0; dest.y = VER3; break;
		case -4: dest.x = HOR0; dest.y = VER4; break;
		case -5: dest.x = HOR1; dest.y = VER4; break;
		case -6: dest.x = HOR2; dest.y = VER4; break;
		case -7: dest.x = HOR3; dest.y = VER4; break;
		case -8: dest.x = HOR4; dest.y = VER4; break;
		case -9: dest.x = HOR5; dest.y = VER4; break;
		case -10: dest.x = HOR6; dest.y = VER4; break;
		case -11: dest.x = HOR7; dest.y = VER4; break;
		case -12: dest.x = HOR8; dest.y = VER4; break;
		case -13: dest.x = HOR9; dest.y = VER4; break;
		case -14: dest.x = HOR10; dest.y = VER4; break;
		case 1:
			if (iThing >= 0) /*** A tile. ***/
			{
				if (iRoomConnections[iCurRoom][1] != 0)
				{
					ShowImage (iThingA[iRoomConnections[iCurRoom][1]][9], (int[]){
						iModifierA[iRoomConnections[iCurRoom][1]][9][1],
						iModifierA[iRoomConnections[iCurRoom][1]][9][2],
						iModifierA[iRoomConnections[iCurRoom][1]][9][3],
						iModifierA[iRoomConnections[iCurRoom][1]][9][4]},
						screen, -1, 0, 0, 120, 155);
				} else {
					ShowImage (20, (int[]){0, 0, 0, 0}, screen, -1, 0, 0, 120, 155);
				}
				ShowImage (-11, (int[]){0, 0, 0, 0}, screen, -1, 0, 0, 120, 155);
			}
			dest.x = HOR1; dest.y = VER1;
			break;
		case 2: dest.x = HOR2; dest.y = VER1; break;
		case 3: dest.x = HOR3; dest.y = VER1; break;
		case 4: dest.x = HOR4; dest.y = VER1; break;
		case 5: dest.x = HOR5; dest.y = VER1; break;
		case 6: dest.x = HOR6; dest.y = VER1; break;
		case 7: dest.x = HOR7; dest.y = VER1; break;
		case 8: dest.x = HOR8; dest.y = VER1; break;
		case 9: dest.x = HOR9; dest.y = VER1; break;
		case 10: dest.x = HOR10; dest.y = VER1; break;

		case 11:
			if (iThing >= 0) /*** A tile. ***/
			{
				if (iRoomConnections[iCurRoom][1] != 0)
				{
					ShowImage (iThingA[iRoomConnections[iCurRoom][1]][19], (int[]){
						iModifierA[iRoomConnections[iCurRoom][1]][19][1],
						iModifierA[iRoomConnections[iCurRoom][1]][19][2],
						iModifierA[iRoomConnections[iCurRoom][1]][19][3],
						iModifierA[iRoomConnections[iCurRoom][1]][19][4]},
						screen, -2, 0, 0, 120, 155);
				} else {
					ShowImage (20, (int[]){0, 0, 0, 0}, screen, -2, 0, 0, 120, 155);
				}
				ShowImage (-11, (int[]){0, 0, 0, 0}, screen, -2, 0, 0, 120, 155);
			}
			dest.x = HOR1; dest.y = VER2;
			break;
		case 12: dest.x = HOR2; dest.y = VER2; break;
		case 13: dest.x = HOR3; dest.y = VER2; break;
		case 14: dest.x = HOR4; dest.y = VER2; break;
		case 15: dest.x = HOR5; dest.y = VER2; break;
		case 16: dest.x = HOR6; dest.y = VER2; break;
		case 17: dest.x = HOR7; dest.y = VER2; break;
		case 18: dest.x = HOR8; dest.y = VER2; break;
		case 19: dest.x = HOR9; dest.y = VER2; break;
		case 20: dest.x = HOR10; dest.y = VER2; break;

		case 21:
			if (iThing >= 0) /*** A tile. ***/
			{
				if (iRoomConnections[iRoomConnections[iCurRoom][1]][4] != 0)
				{
					ShowImage (iThingA[iRoomConnections[iRoomConnections
						[iCurRoom][1]][4]][9], (int[]){
						iModifierA[iRoomConnections[iRoomConnections
							[iCurRoom][1]][4]][9][1],
						iModifierA[iRoomConnections[iRoomConnections
							[iCurRoom][1]][4]][9][2],
						iModifierA[iRoomConnections[iRoomConnections
							[iCurRoom][1]][4]][9][3],
						iModifierA[iRoomConnections[iRoomConnections
							[iCurRoom][1]][4]][9][4]},
						screen, -4, 0, 0, 120, 155);
				} else {
					ShowImage (20, (int[]){0, 0, 0, 0}, screen, -4, 0, 0, 120, 155);
				}
				ShowImage (-11, (int[]){0, 0, 0, 0}, screen, -4, 0, 0, 120, 155);
				if (iRoomConnections[iCurRoom][1] != 0)
				{
					ShowImage (iThingA[iRoomConnections[iCurRoom][1]][29], (int[]){
						iModifierA[iRoomConnections[iCurRoom][1]][29][1],
						iModifierA[iRoomConnections[iCurRoom][1]][29][2],
						iModifierA[iRoomConnections[iCurRoom][1]][29][3],
						iModifierA[iRoomConnections[iCurRoom][1]][29][4]},
						screen, -3, 0, 0, 120, 155);
				} else {
					ShowImage (20, (int[]){0, 0, 0, 0}, screen, -3, 0, 0, 120, 155);
				}
				ShowImage (-11, (int[]){0, 0, 0, 0}, screen, -3, 0, 0, 120, 155);
			}
			dest.x = HOR1; dest.y = VER3;
			break;
		case 22: dest.x = HOR2; dest.y = VER3; break;
		case 23: dest.x = HOR3; dest.y = VER3; break;
		case 24: dest.x = HOR4; dest.y = VER3; break;
		case 25: dest.x = HOR5; dest.y = VER3; break;
		case 26: dest.x = HOR6; dest.y = VER3; break;
		case 27: dest.x = HOR7; dest.y = VER3; break;
		case 28: dest.x = HOR8; dest.y = VER3; break;
		case 29: dest.x = HOR9; dest.y = VER3; break;
		case 30: dest.x = HOR10; dest.y = VER3; break;

		/*** black background & previous & faded background & changes & help ***/
		/*** & PoP select background ***/
		case 31: dest.x = 0; dest.y = 0; break;
		case 32: dest.x = 0; dest.y = 50; break; /*** left ***/
		case 33: dest.x = 667; dest.y = 50; break; /*** right ***/
		case 34: dest.x = 25; dest.y = 25; break; /*** up ***/
		case 35: dest.x = 25; dest.y = 430; break; /*** down ***/
		case 36: dest.x = 0; dest.y = 25; break; /*** rooms ***/
		case 37: dest.x = 667; dest.y = 25; break; /*** events ***/
		case 38: dest.x = 0; dest.y = 430; break; /*** save ***/
		case 39: dest.x = 667; dest.y = 430; break; /*** quit ***/
		case 40: dest.x = 667; dest.y = 0; break; /*** next ***/
		case 41: dest.x = 25; dest.y = 0; break; /*** level bar ***/
		/*** room links and events and PoP2 backgrounds ***/
		case 42: dest.x = 25; dest.y = 50; break;
		case 43: dest.x = 590; dest.y = 405; break; /*** OK (help) ***/
		case 44: dest.x = 84; dest.y = 5; break; /*** popup ***/
		case 45: dest.x = 440; dest.y = 317; break; /*** OK ***/
		case 46: dest.x = 300; dest.y = 183; break; /*** hourglass ***/
		case 47: dest.x = 656; dest.y = 0; break; /*** close ***/
		case 48: /*** changing ***/
			if (iOnTile <= (7 * 13)) /*** Regular large tiles. ***/
			{
				/*** Was: (Modulo (iOnTile, 13, 1) - 1) ***/
				dest.x = 2 + (((iOnTile - 1) % 13) * 50);
				/*** Was: (Modulo (iOnTile, 13, 2) - 1) ***/
				dest.y = 2 + (((int)((iOnTile - 1) / 13)) * 64);
				if ((cCurType == 'o') && (iOnTile >= 14) && (iOnTile <= 26))
					{ dest.y-=20; } /*** 1.5 row ***/
				if ((cCurType == 'o') && (iOnTile >= 27) && (iOnTile <= 39))
					{ dest.y-=10; } /*** 2.5 row ***/
			} else { /*** Half sized tiles (humans). ***/
				dest.x = 2 + ((iOnTile - (7 * 13) - 1) * 25);
				dest.y = 386;
			}
			break;
		case 49: /*** sel event, room ***/
			if (iEditPoP == 1)
			{
				dest.x = 282 + (EventInfo (iChangeEvent, 1) * 15);
				dest.y = 115;
			} else {
				if (iPoP2DoorRoom[iChangeEvent] <= 24)
				{
					dest.x = 282 + (iPoP2DoorRoom[iChangeEvent] * 15);
					dest.y = 115;
				} else {
					dest.x = -78 + (iPoP2DoorRoom[iChangeEvent] * 15);
					dest.y = 150;
				}
			}
			break;
		case 50: /*** sel event, location ***/
			if (iEditPoP == 1)
			{
				iTemp = EventInfo (iChangeEvent, 2);
			} else {
				iTemp = iPoP2DoorLocation[iChangeEvent] + 1;
			}
			if ((iTemp >= 1) && (iTemp <= 10))
			{
				dest.x = 447 + (iTemp * 15);
				dest.y = 155;
			}
			if ((iTemp >= 11) && (iTemp <= 20))
			{
				dest.x = 447 + ((iTemp - 10) * 15);
				dest.y = 170;
			}
			if ((iTemp >= 21) && (iTemp <= 30))
			{
				dest.x = 447 + ((iTemp - 20) * 15);
				dest.y = 185;
			}
			break;
		case 51: /*** sel event, next ***/
			if (iEditPoP == 1)
			{
				dest.x = 582 + (EventInfo (iChangeEvent, 3) * 15);
				dest.y = 225;
			} else {
				printf ("[ WARN ] PoP2 has left/right chains, no next.\n");
			}
			break;
		case 52: dest.x = 552; dest.y = 322; break;
		case 53: dest.x = 452; dest.y = 322; break;
		case 54: dest.x = 402; dest.y = 386; break;
		case 55: dest.x = 352; dest.y = 322; break;
		case 56: dest.x = 2; dest.y = 386; break;
		case 57: dest.x = 352; dest.y = 386; break; /*** kid, turned right ***/
		case 58: dest.x = 377; dest.y = 386; break; /*** kid, turned left ***/

		/*** guards ***/
		case 59: dest.x = 2; dest.y = 386; break;
		case 60: dest.x = 27; dest.y = 386; break;
		case 61: dest.x = 52; dest.y = 386; break;
		case 62: dest.x = 77; dest.y = 386; break;
		case 63: dest.x = 102; dest.y = 386; break;
		case 64: dest.x = 127; dest.y = 386; break;
		case 65: dest.x = 152; dest.y = 386; break;
		case 66: dest.x = 177; dest.y = 386; break;
		case 67: dest.x = 202; dest.y = 386; break;
		case 68: dest.x = 227; dest.y = 386; break;
		case 69: dest.x = 252; dest.y = 386; break;
		case 70: dest.x = 277; dest.y = 386; break;
		case 71: dest.x = 302; dest.y = 386; break;
		case 72: dest.x = 327; dest.y = 386; break;

		/*** skeleton, fat, jaffar, shadow ***/
		case 73: dest.x = 552; dest.y = 322; break;
		case 74: dest.x = 602; dest.y = 322; break;
		case 75: dest.x = 452; dest.y = 322; break;
		case 76: dest.x = 502; dest.y = 322; break;
		case 77: dest.x = 402; dest.y = 386; break;
		case 78: dest.x = 427; dest.y = 386; break;
		case 79: dest.x = 352; dest.y = 322; break;
		case 80: dest.x = 402; dest.y = 322; break;

		case 81:
			switch (iGuardSkill)
			{
				case 0: dest.x = 455; dest.y = 420; break;
				case 1: dest.x = 470; dest.y = 420; break;
				case 2: dest.x = 485; dest.y = 420; break;
				case 3: dest.x = 500; dest.y = 420; break;
				case 4: dest.x = 515; dest.y = 420; break;
				case 5: dest.x = 455; dest.y = 435; break;
				case 6: dest.x = 470; dest.y = 435; break;
				case 7: dest.x = 485; dest.y = 435; break;
				case 8: dest.x = 500; dest.y = 435; break;
				case 9: dest.x = 515; dest.y = 435; break;
				case 10: dest.x = 500; dest.y = 405; break;
				case 11: dest.x = 515; dest.y = 405; break;
			}
			break;
		case 82:
			iIsCustom = 0;
			iTile = iThingA[iCurRoom][iSelected - 1];
			iMod1 = iModifierA[iCurRoom][iSelected - 1][1];
			iMod2 = iModifierA[iCurRoom][iSelected - 1][2];

			if (iEditPoP == 1)
			{
				/*** first row ***/
				if (((iTile == 0) || (iTile == 32)) && (iMod1 == 0))
					{ dest.x = TILESX1; dest.y = TILESY1; }
				else if (((iTile == 0) || (iTile == 32)) && (iMod1 == 1))
					{ dest.x = TILESX2; dest.y = TILESY1; }
				else if (((iTile == 0) || (iTile == 32)) && (iMod1 == 2))
					{ dest.x = TILESX3; dest.y = TILESY1; }
				else if (((iTile == 0) || (iTile == 32)) && (iMod1 == 3))
					{ dest.x = TILESX4; dest.y = TILESY1; }
				else if (((iTile == 0) || (iTile == 32)) && (iMod1 == 255))
					{ dest.x = TILESX5; dest.y = TILESY1; }
				else if (((iTile == 1) || (iTile == 33)) && (iMod1 == 0))
					{ dest.x = TILESX6; dest.y = TILESY1; }
				else if (((iTile == 1) || (iTile == 33)) && (iMod1 == 1))
					{ dest.x = TILESX7; dest.y = TILESY1; }
				else if (((iTile == 1) || (iTile == 33)) && (iMod1 == 2))
					{ dest.x = TILESX8; dest.y = TILESY1; }
				else if (((iTile == 1) || (iTile == 33)) && (iMod1 == 3))
					{ dest.x = TILESX9; dest.y = TILESY1; }
				else if (((iTile == 1) || (iTile == 33)) && (iMod1 == 255))
					{ dest.x = TILESX10; dest.y = TILESY1; }
				else if (((iTile == 2) || (iTile == 34)) && (iMod1 == 0))
					{ dest.x = TILESX11; dest.y = TILESY1; }
				else if (((iTile == 2) || (iTile == 34)) && (iMod1 == 1))
					{ dest.x = TILESX12; dest.y = TILESY1; }
				else if (((iTile == 2) || (iTile == 34)) && (iMod1 == 2))
					{ dest.x = TILESX13; dest.y = TILESY1; }

				/*** second row ***/
				else if (((iTile == 2) || (iTile == 34)) && (iMod1 == 3))
					{ dest.x = TILESX1; dest.y = TILESY2; }
				else if (((iTile == 2) || (iTile == 34)) && (iMod1 == 4))
					{ dest.x = TILESX2; dest.y = TILESY2; }
				else if (((iTile == 2) || (iTile == 34)) && (iMod1 == 5))
					{ dest.x = TILESX3; dest.y = TILESY2; }
				else if (((iTile == 2) || (iTile == 34)) && (iMod1 == 6))
					{ dest.x = TILESX4; dest.y = TILESY2; }
				else if (((iTile == 2) || (iTile == 34)) && (iMod1 == 7))
					{ dest.x = TILESX5; dest.y = TILESY2; }
				else if (((iTile == 2) || (iTile == 34)) && (iMod1 == 8))
					{ dest.x = TILESX6; dest.y = TILESY2; }
				else if (((iTile == 2) || (iTile == 34)) && (iMod1 == 9))
					{ dest.x = TILESX7; dest.y = TILESY2; }
				else if (((iTile == 3) || (iTile == 35)) && (iMod1 == 0))
					{ dest.x = TILESX8; dest.y = TILESY2; }
				else if (((iTile == 4) || (iTile == 36)) &&
					((iMod1 == 0) || (iMod1 == 2)))
					{ dest.x = TILESX9; dest.y = TILESY2; }
				else if (((iTile == 4) || (iTile == 36)) && (iMod1 == 1))
					{ dest.x = TILESX10; dest.y = TILESY2; }
				else if (((iTile == 5) || (iTile == 37)) && (iMod1 == 0))
					{ dest.x = TILESX11; dest.y = TILESY2; }
				else if ((iTile == 6) || (iTile == 38))
					{ dest.x = TILESX12; dest.y = TILESY2; iChangeEvent = iMod1; }
				else if (((iTile == 7) || (iTile == 39)) && (iMod1 == 0))
					{ dest.x = TILESX13; dest.y = TILESY2; }

				/*** third row ***/
				else if (((iTile == 7) || (iTile == 39)) && (iMod1 == 1))
					{ dest.x = TILESX1; dest.y = TILESY3; }
				else if (((iTile == 7) || (iTile == 39)) && (iMod1 == 2))
					{ dest.x = TILESX2; dest.y = TILESY3; }
				else if (((iTile == 7) || (iTile == 39)) && (iMod1 == 3))
					{ dest.x = TILESX3; dest.y = TILESY3; }
				else if (((iTile == 8) || (iTile == 40)) && (iMod1 == 0))
					{ dest.x = TILESX4; dest.y = TILESY3; }
				else if (((iTile == 9) || (iTile == 41)) && (iMod1 == 0))
					{ dest.x = TILESX5; dest.y = TILESY3; }
				else if (((iTile == 10) || (iTile == 42)) && (iMod1 == 0))
					{ dest.x = TILESX6; dest.y = TILESY3; }
				else if (((iTile == 10) || (iTile == 42)) && (iMod1 == 1))
					{ dest.x = TILESX7; dest.y = TILESY3; }
				else if (((iTile == 10) || (iTile == 42)) && (iMod1 == 2))
					{ dest.x = TILESX8; dest.y = TILESY3; }
				else if (((iTile == 10) || (iTile == 42)) && (iMod1 == 3))
					{ dest.x = TILESX9; dest.y = TILESY3; }
				else if (((iTile == 10) || (iTile == 42)) && (iMod1 == 4))
					{ dest.x = TILESX10; dest.y = TILESY3; }
				else if (((iTile == 10) || (iTile == 42)) && (iMod1 == 5))
					{ dest.x = TILESX11; dest.y = TILESY3; }
				else if (((iTile == 10) || (iTile == 42)) && (iMod1 == 6))
					{ dest.x = TILESX12; dest.y = TILESY3; }
				else if ((iTile == 11) && (iMod1 == 0))
					{ dest.x = TILESX13; dest.y = TILESY3; }

				/*** fourth row ***/
				else if (((iTile == 12) || (iTile == 44)) && (iMod1 == 0))
					{ dest.x = TILESX1; dest.y = TILESY4; }
				else if (((iTile == 12) || (iTile == 44)) && (iMod1 == 1))
					{ dest.x = TILESX2; dest.y = TILESY4; }
				else if (((iTile == 12) || (iTile == 44)) && (iMod1 == 2))
					{ dest.x = TILESX3; dest.y = TILESY4; }
				else if (((iTile == 12) || (iTile == 44)) && (iMod1 == 3))
					{ dest.x = TILESX4; dest.y = TILESY4; }
				else if (((iTile == 12) || (iTile == 44)) && (iMod1 == 4))
					{ dest.x = TILESX5; dest.y = TILESY4; }
				else if (((iTile == 12) || (iTile == 44)) && (iMod1 == 5))
					{ dest.x = TILESX6; dest.y = TILESY4; }
				else if (((iTile == 12) || (iTile == 44)) && (iMod1 == 6))
					{ dest.x = TILESX7; dest.y = TILESY4; }
				else if (((iTile == 12) || (iTile == 44)) && (iMod1 == 7))
					{ dest.x = TILESX8; dest.y = TILESY4; }
				else if (((iTile == 13) || (iTile == 45)) && (iMod1 == 0))
					{ dest.x = TILESX9; dest.y = TILESY4; }
				else if (((iTile == 14) || (iTile == 46)) && (iMod1 == 0))
					{ dest.x = TILESX10; dest.y = TILESY4; }
				else if ((iTile == 15) || (iTile == 47))
					{ dest.x = TILESX11; dest.y = TILESY4; iChangeEvent = iMod1; }
				else if (((iTile == 16) || (iTile == 48)) && (iMod1 == 0))
					{ dest.x = TILESX12; dest.y = TILESY4; }
				else if (((iTile == 17) || (iTile == 49)) && (iMod1 == 0))
					{ dest.x = TILESX13; dest.y = TILESY4; }

				/*** fifth row ***/
				else if (((iTile == 18) || (iTile == 50)) && (iMod1 == 0))
					{ dest.x = TILESX1; dest.y = TILESY5; }
				else if (((iTile == 18) || (iTile == 50)) && (iMod1 == 1))
					{ dest.x = TILESX2; dest.y = TILESY5; }
				else if (((iTile == 18) || (iTile == 50)) && (iMod1 == 2))
					{ dest.x = TILESX3; dest.y = TILESY5; }
				else if (((iTile == 18) || (iTile == 50)) && (iMod1 == 3))
					{ dest.x = TILESX4; dest.y = TILESY5; }
				else if (((iTile == 18) || (iTile == 50)) && (iMod1 == 4))
					{ dest.x = TILESX5; dest.y = TILESY5; }
				else if (((iTile == 18) || (iTile == 50)) && (iMod1 == 5))
					{ dest.x = TILESX6; dest.y = TILESY5; }
				else if (((iTile == 19) || (iTile == 51)) && (iMod1 == 0))
					{ dest.x = TILESX7; dest.y = TILESY5; }
				else if (((iTile == 20) || (iTile == 52)) && (iMod1 == 0))
					{ dest.x = TILESX8; dest.y = TILESY5; }
				else if (((iTile == 20) || (iTile == 52)) && (iMod1 == 1))
					{ dest.x = TILESX9; dest.y = TILESY5; }
				else if (((iTile == 21) || (iTile == 53)) && (iMod1 == 0))
					{ dest.x = TILESX10; dest.y = TILESY5; }
				else if (((iTile == 22) || (iTile == 54)) && (iMod1 == 0))
					{ dest.x = TILESX11; dest.y = TILESY5; }
				else if (((iTile == 23) || (iTile == 55)) && (iMod1 == 0))
					{ dest.x = TILESX12; dest.y = TILESY5; }
				else if (((iTile == 24) || (iTile == 56)) && (iMod1 == 0))
					{ dest.x = TILESX13; dest.y = TILESY5; }

				/*** sixth row ***/
				else if (((iTile == 25) || (iTile == 57)) && (iMod1 == 0))
					{ dest.x = TILESX1; dest.y = TILESY6; }
				else if (((iTile == 26) || (iTile == 58)) && (iMod1 == 0))
					{ dest.x = TILESX2; dest.y = TILESY6; }
				else if (((iTile == 27) || (iTile == 59)) && (iMod1 == 0))
					{ dest.x = TILESX3; dest.y = TILESY6; }
				else if (((iTile == 28) || (iTile == 60)) && (iMod1 == 0))
					{ dest.x = TILESX4; dest.y = TILESY6; }
				else if (((iTile == 29) || (iTile == 61)) && (iMod1 == 0))
					{ dest.x = TILESX5; dest.y = TILESY6; }
				else if (((iTile == 30) || (iTile == 62)) && (iMod1 == 0))
					{ dest.x = TILESX6; dest.y = TILESY6; }
				else if ((iTile == 43) && (iMod1 == 0))
					{ dest.x = TILESX7; dest.y = TILESY6; }

				else { iIsCustom = 1; }
			}

			if (iEditPoP == 2)
			{
				/*** rooftops, desert, final ***/
				if ((luLevelNr == 1) || (luLevelNr == 2) ||
					((luLevelNr >= 14) && (luLevelNr <= 16)) || (luLevelNr == 28))
				{
					/*** 1.5 row ***/
					if (iTile == 0)
						{ dest.x = TILESX4; dest.y = TILESY15; }
					else if (iTile == 1)
						{ dest.x = TILESX5; dest.y = TILESY15; }
					else if (iTile == 20)
						{ dest.x = TILESX6; dest.y = TILESY15; }
					else if (iTile == 3)
						{ dest.x = TILESX10; dest.y = TILESY15; }
					else if (iTile == 37)
						{ dest.x = TILESX11; dest.y = TILESY15; }
					else if (iTile == 38)
						{ dest.x = TILESX12; dest.y = TILESY15; }
					else if (iTile == 39)
						{ dest.x = TILESX13; dest.y = TILESY15; }

					/*** 2.5 row ***/
					else if (iTile == 4)
					{
						dest.x = TILESX3; dest.y = TILESY25;
						iChangePercent = ModAsPercent (iMod1, 4);
					}
					else if (iTile == 28)
						{ dest.x = TILESX4; dest.y = TILESY25; }
					else if (iTile == 29)
						{ dest.x = TILESX5; dest.y = TILESY25; }
					else if (iTile == 30)
						{ dest.x = TILESX6; dest.y = TILESY25; }
					else if ((iTile == 10) && (iMod1 >= 32) && (iMod1 <= 63))
						{ dest.x = TILESX10; dest.y = TILESY25; }
					else if (iTile == 31)
						{ dest.x = TILESX11; dest.y = TILESY25; }
					else if (iTile == 40)
						{ dest.x = TILESX12; dest.y = TILESY25; }
					else if (iTile == 41)
						{ dest.x = TILESX13; dest.y = TILESY25; }

					/*** fourth row ***/
					else if (iTile == 2)
						{ dest.x = TILESX1; dest.y = TILESY4; }
					else if (iTile == 5)
						{ dest.x = TILESX2; dest.y = TILESY4; iChangeEvent = iMod1; }
					else if (iTile == 6)
						{ dest.x = TILESX3; dest.y = TILESY4; iChangeEvent = iMod1; }
					else if ((iTile == 7) && (iMod1 == 0))
						{ dest.x = TILESX4; dest.y = TILESY4; }
					else if ((iTile == 7) && (iMod1 == 1))
						{ dest.x = TILESX5; dest.y = TILESY4; }
					else if ((iTile == 7) && (iMod1 == 2))
						{ dest.x = TILESX6; dest.y = TILESY4; }
					else if ((iTile == 7) && (iMod1 == 3))
						{ dest.x = TILESX7; dest.y = TILESY4; }
					else if (iTile == 8)
						{ dest.x = TILESX8; dest.y = TILESY4; }
					else if (iTile == 9)
						{ dest.x = TILESX9; dest.y = TILESY4; }
					else if ((iTile == 10) && (iMod1 >= 0) && (iMod1 <= 31))
						{ dest.x = TILESX10; dest.y = TILESY4; }
					else if ((iTile == 10) && (iMod1 >= 64) && (iMod1 <= 95))
						{ dest.x = TILESX11; dest.y = TILESY4; }
					else if ((iTile == 10) && (iMod1 >= 96) && (iMod1 <= 127))
						{ dest.x = TILESX12; dest.y = TILESY4; }
					else if ((iTile == 10) && (iMod1 >= 128) && (iMod1 <= 159))
						{ dest.x = TILESX13; dest.y = TILESY4; }

					/*** fifth row ***/
					else if ((iTile == 10) && (iMod1 >= 160) && (iMod1 <= 191))
						{ dest.x = TILESX1; dest.y = TILESY5; }
					else if ((iTile == 10) && (iMod1 >= 192) && (iMod1 <= 223))
						{ dest.x = TILESX2; dest.y = TILESY5; }
					else if ((iTile == 10) && (iMod1 >= 224) && (iMod1 <= 255))
						{ dest.x = TILESX3; dest.y = TILESY5; }
					else if (iTile == 11)
					{
						dest.x = TILESX4; dest.y = TILESY5;
						iChangeJumps = ModAsJumps (iMod1);
						iChangeFrame = ModAsFrame (iMod1);
					}
					else if (iTile == 12)
					{
						dest.x = TILESX5; dest.y = TILESY5;
						iChangeFrame = ModAsFrame (iMod1);
					}
					else if (iTile == 13)
						{ dest.x = TILESX6; dest.y = TILESY5; }
					else if (iTile == 14)
						{ dest.x = TILESX7; dest.y = TILESY5; }
					else if (iTile == 15)
						{ dest.x = TILESX8; dest.y = TILESY5; }
					else if (iTile == 16)
						{ dest.x = TILESX9; dest.y = TILESY5; }
					else if (iTile == 17)
					{
						dest.x = TILESX10; dest.y = TILESY5;
						iChangePercent = ModAsPercent (iMod1, 17);
					}

					/*** sixth row ***/
					else if (iTile == 18)
						{ dest.x = TILESX1; dest.y = TILESY6; }
					else if (iTile == 19)
						{ dest.x = TILESX2; dest.y = TILESY6; }
					else if (iTile == 21)
						{ dest.x = TILESX3; dest.y = TILESY6; }
					else if ((iTile == 22) && (iMod1 == 0))
						{ dest.x = TILESX4; dest.y = TILESY6; }
					else if ((iTile == 22) && (iMod1 == 1))
						{ dest.x = TILESX5; dest.y = TILESY6; }
					else if (iTile == 23)
						{ dest.x = TILESX6; dest.y = TILESY6; }
					else if (iTile == 24)
						{ dest.x = TILESX7; dest.y = TILESY6; }
					else if (iTile == 25)
						{ dest.x = TILESX8; dest.y = TILESY6; }
					else if (iTile == 26)
						{ dest.x = TILESX9; dest.y = TILESY6; }
					else if (iTile == 27)
						{ dest.x = TILESX10; dest.y = TILESY6; }

					/*** seventh row ***/
					else if (iTile == 32)
						{ dest.x = TILESX1; dest.y = TILESY7; }
					else if (iTile == 33)
						{ dest.x = TILESX2; dest.y = TILESY7; }
					else if (iTile == 34)
						{ dest.x = TILESX3; dest.y = TILESY7; iChangeEvent = iMod1; }
					else if (iTile == 35)
						{ dest.x = TILESX4; dest.y = TILESY7; }
					else if (iTile == 36)
					{
						dest.x = TILESX5; dest.y = TILESY7;
						iChangeFrame = ModAsFrame (iMod1);
					}
					else if (iTile == 43)
						{ dest.x = TILESX9; dest.y = TILESY7; }
					else if (iTile == 44)
						{ dest.x = TILESX10; dest.y = TILESY7; }

					else { iIsCustom = 1; }
				}
				/*** caverns ***/
				if (((luLevelNr >= 3) && (luLevelNr <= 5)) ||
					((luLevelNr >= 17) && (luLevelNr <= 19)))
				{
					/*** first row ***/
					if ((iTile == 0) && (iMod1 == 0))
					{
						dest.x = TILESX1; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, ' ', 's');
					}
					else if ((iTile == 0) && (iMod1 == 1))
					{
						dest.x = TILESX2; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, ' ', 's');
					}
					else if ((iTile == 0) && (iMod1 == 2))
					{
						dest.x = TILESX3; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, ' ', 's');
					}
					else if ((iTile == 0) && (iMod1 == 3))
					{
						dest.x = TILESX4; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, ' ', 's');
					}
					else if ((iTile == 0) && (iMod1 == 4))
					{
						dest.x = TILESX5; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, ' ', 's');
					}
					else if ((iTile == 1) && (iMod1 == 0))
					{
						dest.x = TILESX6; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 1) && (iMod1 == 1))
					{
						dest.x = TILESX7; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 1) && (iMod1 == 2))
					{
						dest.x = TILESX8; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 2) && (iMod1 == 0))
					{
						dest.x = TILESX9; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 2) && (iMod1 == 5))
					{
						dest.x = TILESX10; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if (iTile == 3)
					{
						dest.x = TILESX11; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 4) && (iMod1 == 0))
					{
						dest.x = TILESX12; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 4) && (iMod1 != 0) && (iMod1 != 200))
					{
						dest.x = TILESX13; dest.y = TILESY1;
						iChangePercent = ModAsPercent (iMod1, 4);
						ExtractFromMod2 (iMod2, 'b', 's');
					}

					/*** second row ***/
					else if ((iTile == 4) && (iMod1 == 200))
					{
						dest.x = TILESX1; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 8) && (iMod1 == 0))
					{
						dest.x = TILESX2; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 8) && ((iMod1 == 1) || (iMod1 == 5)))
					{
						dest.x = TILESX3; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 8) && (iMod1 == 2))
					{
						dest.x = TILESX4; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if (iTile == 9)
					{
						dest.x = TILESX5; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, ' ', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 0) && (iMod1 <= 31))
					{
						dest.x = TILESX6; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 32) && (iMod1 <= 63))
					{
						dest.x = TILESX7; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 64) && (iMod1 <= 95))
					{
						dest.x = TILESX8; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 96) && (iMod1 <= 127))
					{
						dest.x = TILESX9; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 128) && (iMod1 <= 159))
					{
						dest.x = TILESX10; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 160) && (iMod1 <= 191))
					{
						dest.x = TILESX11; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 192) && (iMod1 <= 223))
					{
						dest.x = TILESX12; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 224) && (iMod1 <= 255))
					{
						dest.x = TILESX13; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'b', 's');
					}

					/*** third row ***/
					else if ((iTile == 11) && (iMod1 <= 127))
					{
						dest.x = TILESX1; dest.y = TILESY3;
						iChangeJumps = ModAsJumps (iMod1);
						iChangeFrame = ModAsFrame (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 11) && (iMod1 >= 128))
					{
						dest.x = TILESX2; dest.y = TILESY3;
						iChangeJumps = ModAsJumps (iMod1);
						iChangeFrame = ModAsFrame (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 14) && (iMod1 == 0))
					{
						dest.x = TILESX3; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 14) && (iMod1 == 1))
					{
						dest.x = TILESX4; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 14) && (iMod1 == 2))
					{
						dest.x = TILESX5; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if (iTile == 16)
					{
						dest.x = TILESX6; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 17) && (iMod1 == 0))
					{
						dest.x = TILESX7; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 17) && (iMod1 != 0) && (iMod1 != 42))
					{
						dest.x = TILESX8; dest.y = TILESY3;
						iChangePercent = ModAsPercent (iMod1, 17);
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 17) && (iMod1 == 42))
					{
						dest.x = TILESX9; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if (iTile == 18)
					{
						dest.x = TILESX10; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, ' ', ' ');
					}
					else if (iTile == 19)
					{
						dest.x = TILESX11; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if ((iTile == 20) && ((iMod1 == 0) || (iMod1 == 128)))
					{
						dest.x = TILESX12; dest.y = TILESY3;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 20) && ((iMod1 == 1) || (iMod1 == 129)))
					{
						dest.x = TILESX13; dest.y = TILESY3;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}

					/*** fourth row ***/
					else if ((iTile == 20) && ((iMod1 == 2) || (iMod1 == 130)))
					{
						dest.x = TILESX1; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 20) && ((iMod1 == 3) || (iMod1 == 131)))
					{
						dest.x = TILESX2; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 20) && ((iMod1 == 4) || (iMod1 == 132)))
					{
						dest.x = TILESX3; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 20) && ((iMod1 == 5) || (iMod1 == 133)))
					{
						dest.x = TILESX4; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 20) && ((iMod1 == 6) || (iMod1 == 134)))
					{
						dest.x = TILESX5; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 20) && ((iMod1 == 7) || (iMod1 == 135)))
					{
						dest.x = TILESX6; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 20) && ((iMod1 == 8) || (iMod1 == 136)))
					{
						dest.x = TILESX7; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 20) && ((iMod1 == 9) || (iMod1 == 137)))
					{
						dest.x = TILESX8; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 20) && ((iMod1 == 10) || (iMod1 == 138)))
					{
						dest.x = TILESX9; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if (iTile == 23)
					{
						dest.x = TILESX10; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if (iTile == 24)
					{
						dest.x = TILESX11; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if (iTile == 28)
					{
						dest.x = TILESX12; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, ' ', ' ');
					}
					else if (iTile == 32)
					{
						dest.x = TILESX13; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, 'b', 's');
					}

					/*** fifth row ***/
					else if (iTile == 33)
					{
						dest.x = TILESX1; dest.y = TILESY5;
						ExtractFromMod2 (iMod2, ' ', 's');
					}
					else if ((iTile == 34) && (iMod2 % 16 >= 0) &&
						(iMod2 % 16 <= 7)) /*** raise drop (reg) ***/
					{
						dest.x = TILESX2; dest.y = TILESY5;
						iChangeEvent = iMod1;
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if ((iTile == 34) && (iMod2 % 16 >= 8) &&
						(iMod2 % 16 <= 15)) /*** raise drop (down) ***/
					{
						dest.x = TILESX3; dest.y = TILESY5;
						iChangeEvent = iMod1;
						ExtractFromMod2 (iMod2, 'b', ' ');
					}
					else if (iTile == 35)
					{
						dest.x = TILESX4; dest.y = TILESY5;
						ExtractFromMod2 (iMod2, ' ', 's');
					}
					else if (iTile == 36)
					{
						dest.x = TILESX5; dest.y = TILESY5;
						iChangeFrame = ModAsFrame (iMod1);
						ExtractFromMod2 (iMod2, 'b', 's');
					}
					else if (iTile == 44)
					{
						dest.x = TILESX6; dest.y = TILESY5;
						ExtractFromMod2 (iMod2, ' ', ' ');
					}

					else { iIsCustom = 1; }
				}
				/*** ruins ***/
				if (((luLevelNr >= 6) && (luLevelNr <= 9)) ||
					((luLevelNr >= 20) && (luLevelNr <= 23)))
				{
					/*** first row ***/
					if (iTile == 0)
					{
						dest.x = TILESX1; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 1) && (iMod1 == 0))
					{
						dest.x = TILESX2; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 1) && (iMod1 == 1))
					{
						dest.x = TILESX3; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 1) && (iMod1 == 2))
					{
						dest.x = TILESX4; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 1) && (iMod1 == 3))
					{
						dest.x = TILESX5; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if (iTile == 2)
					{
						dest.x = TILESX6; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, ' ', ' ');
					}
					else if ((iTile == 3) && (iMod1 == 0))
					{
						dest.x = TILESX7; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 3) && (iMod1 == 1))
					{
						dest.x = TILESX8; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 3) && (iMod1 == 2))
					{
						dest.x = TILESX9; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 4) && (iMod1 == 0))
					{
						dest.x = TILESX10; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 4) && (iMod1 != 0) && (iMod1 != 200))
					{
						dest.x = TILESX11; dest.y = TILESY1;
						iChangePercent = ModAsPercent (iMod1, 4);
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 4) && (iMod1 == 200))
					{
						dest.x = TILESX12; dest.y = TILESY1;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 5) && (iMod2 >= 0) && (iMod2 <= 7))
					{
						dest.x = TILESX13; dest.y = TILESY1;
						iChangeEvent = iMod1;
						ExtractFromMod2 (iMod2, ' ', ' ');
					}

					/*** second row ***/
					else if ((iTile == 5) && (iMod2 >= 8) && (iMod2 <= 15))
					{
						dest.x = TILESX1; dest.y = TILESY2;
						iChangeEvent = iMod1;
						ExtractFromMod2 (iMod2, ' ', ' ');
					}
					else if ((iTile == 6) && (iMod2 >= 0) && (iMod2 <= 7))
					{
						dest.x = TILESX2; dest.y = TILESY2;
						iChangeEvent = iMod1;
						ExtractFromMod2 (iMod2, ' ', ' ');
					}
					else if ((iTile == 6) && (iMod2 >= 8) && (iMod2 <= 15))
					{
						dest.x = TILESX3; dest.y = TILESY2;
						iChangeEvent = iMod1;
						ExtractFromMod2 (iMod2, ' ', ' ');
					}
					else if ((iTile == 7) && ((iMod1 == 0) || (iMod1 == 128)))
					{
						dest.x = TILESX4; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 7) && ((iMod1 == 1) || (iMod1 == 129)))
					{
						dest.x = TILESX5; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 7) && ((iMod1 == 2) || (iMod1 == 130)))
					{
						dest.x = TILESX6; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 7) && ((iMod1 == 3) || (iMod1 == 131)))
					{
						dest.x = TILESX7; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if (iTile == 8)
					{
						dest.x = TILESX8; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'c', ' ');
					}
					else if (iTile == 9)
					{
						dest.x = TILESX9; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'c', ' ');
					}
					else if ((iTile == 10) && (iMod1 >= 0) && (iMod1 <= 31))
					{
						dest.x = TILESX10; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 32) && (iMod1 <= 63))
					{
						dest.x = TILESX11; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 64) && (iMod1 <= 95))
					{
						dest.x = TILESX12; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 96) && (iMod1 <= 127))
					{
						dest.x = TILESX13; dest.y = TILESY2;
						ExtractFromMod2 (iMod2, 'c', 's');
					}

					/*** third row ***/
					else if ((iTile == 10) && (iMod1 >= 128) && (iMod1 <= 159))
					{
						dest.x = TILESX1; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 160) && (iMod1 <= 191))
					{
						dest.x = TILESX2; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 192) && (iMod1 <= 223))
					{
						dest.x = TILESX3; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 10) && (iMod1 >= 224) && (iMod1 <= 255))
					{
						dest.x = TILESX4; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if (iTile == 11)
					{
						dest.x = TILESX5; dest.y = TILESY3;
						iChangeJumps = ModAsJumps (iMod1);
						iChangeFrame = ModAsFrame (iMod1);
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 12) && (iMod1 >= 0) && (iMod1 <= 31))
					{
						dest.x = TILESX6; dest.y = TILESY3;
						iChangeFrame = ModAsFrame (iMod1);
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 12) && (iMod1 >= 32) && (iMod1 <= 63))
					{
						dest.x = TILESX7; dest.y = TILESY3;
						iChangeFrame = ModAsFrame (iMod1);
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 12) && (iMod1 >= 128) && (iMod1 <= 159))
					{
						dest.x = TILESX8; dest.y = TILESY3;
						iChangeFrame = ModAsFrame (iMod1);
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 12) && (iMod1 >= 160) && (iMod1 <= 191))
					{
						dest.x = TILESX9; dest.y = TILESY3;
						iChangeFrame = ModAsFrame (iMod1);
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 13) && (iMod1 == 0))
					{
						dest.x = TILESX10; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 13) && (iMod1 == 32))
					{
						dest.x = TILESX11; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 14) && (iMod1 == 16))
					{
						dest.x = TILESX12; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 14) && (iMod1 == 32))
					{
						dest.x = TILESX13; dest.y = TILESY3;
						ExtractFromMod2 (iMod2, 'c', 's');
					}

					/*** fourth row ***/
					else if ((iTile == 14) && (iMod1 == 64))
					{
						dest.x = TILESX1; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 15) && (iMod1 == 0))
					{
						dest.x = TILESX2; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 15) && (iMod1 == 1))
					{
						dest.x = TILESX3; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 15) && (iMod1 == 2))
					{
						dest.x = TILESX4; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 15) && (iMod1 == 3))
					{
						dest.x = TILESX5; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if (iTile == 16)
					{
						dest.x = TILESX6; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 17) && (iMod1 == 0))
					{
						dest.x = TILESX7; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, ' ', 's');
					}
					else if ((iTile == 17) && (iMod1 != 0) && (iMod1 != 42))
					{
						dest.x = TILESX8; dest.y = TILESY4;
						iChangePercent = ModAsPercent (iMod1, 17);
						ExtractFromMod2 (iMod2, ' ', 's');
					}
					else if ((iTile == 17) && (iMod1 == 42))
					{
						dest.x = TILESX9; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, ' ', 's');
					}
					else if (iTile == 18)
					{
						dest.x = TILESX10; dest.y = TILESY4;
						ExtractFromMod2 (iMod2, ' ', ' ');
					}
					else if ((iTile == 20) && ((iMod1 == 0) ||
						(iMod1 == 4) || (iMod1 == 8) || (iMod1 == 12)))
					{
						dest.x = TILESX11; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 20) && ((iMod1 == 1) ||
						(iMod1 == 5) || (iMod1 == 9) || (iMod1 == 13)))
					{
						dest.x = TILESX12; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 20) && ((iMod1 == 2) ||
						(iMod1 == 6) || (iMod1 == 10) || (iMod1 == 14)))
					{
						dest.x = TILESX13; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
						ExtractFromMod2 (iMod2, 'c', 's');
					}

					/*** fifth row ***/
					else if (iTile == 21)
					{
						dest.x = TILESX1; dest.y = TILESY5;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 22) && (iMod1 == 2))
					{
						dest.x = TILESX2; dest.y = TILESY5;
						ExtractFromMod2 (iMod2, 'c', 's');
					}
					else if ((iTile == 22) && (iMod1 != 2))
					{
						dest.x = TILESX3; dest.y = TILESY5;
						ExtractFromMod2 (iMod2, 'c', 's');
					}

					else { iIsCustom = 1; }
				}
				/*** temple ***/
				if (((luLevelNr >= 10) && (luLevelNr <= 13)) ||
					((luLevelNr >= 24) && (luLevelNr <= 27)))
				{
					/*** first row ***/
					if (iTile == 0)
						{ dest.x = TILESX1; dest.y = TILESY1; }
					else if ((iTile == 1) && (iMod1 == 0))
						{ dest.x = TILESX2; dest.y = TILESY1; }
					else if ((iTile == 1) && (iMod1 == 1))
						{ dest.x = TILESX3; dest.y = TILESY1; }
					else if ((iTile == 2) && (iMod1 == 0))
						{ dest.x = TILESX4; dest.y = TILESY1; }
					else if ((iTile == 2) && (iMod1 == 5))
						{ dest.x = TILESX5; dest.y = TILESY1; }
					else if ((iTile == 3) && (iMod1 == 0))
						{ dest.x = TILESX6; dest.y = TILESY1; }
					else if ((iTile == 3) && (iMod1 == 1))
						{ dest.x = TILESX7; dest.y = TILESY1; }
					else if ((iTile == 4) && (iMod1 == 0))
						{ dest.x = TILESX8; dest.y = TILESY1; }
					else if ((iTile == 4) && (iMod1 != 0) && (iMod1 != 200))
					{
						dest.x = TILESX9; dest.y = TILESY1;
						iChangePercent = ModAsPercent (iMod1, 4);
					}
					else if ((iTile == 4) && (iMod1 == 200))
						{ dest.x = TILESX10; dest.y = TILESY1; }
					else if ((iTile == 5) && (iMod2 >= 0) && (iMod2 <= 7))
						{ dest.x = TILESX11; dest.y = TILESY1; iChangeEvent = iMod1; }
					else if ((iTile == 5) && (iMod2 >= 8) && (iMod2 <= 15))
						{ dest.x = TILESX12; dest.y = TILESY1; iChangeEvent = iMod1; }
					else if ((iTile == 6) && (iMod2 >= 0) && (iMod2 <= 7))
						{ dest.x = TILESX13; dest.y = TILESY1; iChangeEvent = iMod1; }

					/*** second row ***/
					else if ((iTile == 6) && (iMod2 >= 8) && (iMod2 <= 15))
						{ dest.x = TILESX1; dest.y = TILESY2; iChangeEvent = iMod1; }
					else if (iTile == 8)
						{ dest.x = TILESX2; dest.y = TILESY2; }
					else if (iTile == 9)
						{ dest.x = TILESX3; dest.y = TILESY2; }
					else if ((iTile == 10) && (iMod1 >= 0) && (iMod1 <= 31))
						{ dest.x = TILESX4; dest.y = TILESY2; }
					else if ((iTile == 10) && (iMod1 >= 32) && (iMod1 <= 63))
						{ dest.x = TILESX5; dest.y = TILESY2; }
					else if ((iTile == 10) && (iMod1 >= 64) && (iMod1 <= 95))
						{ dest.x = TILESX6; dest.y = TILESY2; }
					else if ((iTile == 10) && (iMod1 >= 96) && (iMod1 <= 127))
						{ dest.x = TILESX7; dest.y = TILESY2; }
					else if ((iTile == 10) && (iMod1 >= 128) && (iMod1 <= 159))
						{ dest.x = TILESX8; dest.y = TILESY2; }
					else if ((iTile == 10) && (iMod1 >= 160) && (iMod1 <= 191))
						{ dest.x = TILESX9; dest.y = TILESY2; }
					else if ((iTile == 10) && (iMod1 >= 192) && (iMod1 <= 223))
						{ dest.x = TILESX10; dest.y = TILESY2; }
					else if ((iTile == 10) && (iMod1 >= 224) && (iMod1 <= 255))
						{ dest.x = TILESX11; dest.y = TILESY2; }
					else if (iTile == 11)
					{
						dest.x = TILESX12; dest.y = TILESY2;
						iChangeJumps = ModAsJumps (iMod1);
						iChangeFrame = ModAsFrame (iMod1);
					}
					else if ((iTile == 12) && (iMod1 >= 0) && (iMod1 <= 31))
					{
						dest.x = TILESX13; dest.y = TILESY2;
						iChangeFrame = ModAsFrame (iMod1);
					}

					/*** third row ***/
					else if ((iTile == 12) && (iMod1 >= 32) && (iMod1 <= 63))
					{
						dest.x = TILESX6; dest.y = TILESY3;
						iChangeFrame = ModAsFrame (iMod1);
					}
					else if ((iTile == 12) && (iMod1 >= 128) && (iMod1 <= 159))
					{
						dest.x = TILESX7; dest.y = TILESY3;
						iChangeFrame = ModAsFrame (iMod1);
					}
					else if ((iTile == 12) && (iMod1 >= 160) && (iMod1 <= 191))
					{
						dest.x = TILESX8; dest.y = TILESY3;
						iChangeFrame = ModAsFrame (iMod1);
					}
					else if ((iTile == 13) && (iMod1 == 0))
						{ dest.x = TILESX9; dest.y = TILESY3; }
					else if ((iTile == 13) && (iMod1 == 32))
						{ dest.x = TILESX10; dest.y = TILESY3; }
					else if ((iTile == 14) && (iMod1 == 0))
						{ dest.x = TILESX11; dest.y = TILESY3; }
					else if ((iTile == 14) && (iMod1 == 1))
						{ dest.x = TILESX12; dest.y = TILESY3; }
					else if (iTile == 16)
						{ dest.x = TILESX13; dest.y = TILESY3; }

					/*** fourth row ***/
					else if ((iTile == 17) && (iMod1 == 0))
						{ dest.x = TILESX6; dest.y = TILESY4; }
					else if ((iTile == 17) && (iMod1 != 0) && (iMod1 != 42))
					{
						dest.x = TILESX7; dest.y = TILESY4;
						iChangePercent = ModAsPercent (iMod1, 17);
					}
					else if ((iTile == 17) && (iMod1 == 42))
						{ dest.x = TILESX8; dest.y = TILESY4; }
					else if (iTile == 18)
						{ dest.x = TILESX9; dest.y = TILESY4; }
					else if (iTile == 19)
						{ dest.x = TILESX10; dest.y = TILESY4; }
					else if (iTile == 20)
					{
						dest.x = TILESX11; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
					}
					else if (iTile == 25)
					{
						dest.x = TILESX12; dest.y = TILESY4;
						iChangeWall = ModAsWall (iMod1);
					}
					else if (iTile == 26)
						{ dest.x = TILESX13; dest.y = TILESY4; }

					/*** fifth row ***/
					else if (iTile == 32)
						{ dest.x = TILESX6; dest.y = TILESY5; }

					else { iIsCustom = 1; }
				}
			}

			break;
		case 83:
			/*** mouse cursor doesn't need scaling ***/
			dest.x = (iXPos + 10) / iScale;
			dest.y = (iYPos + 10) / iScale;
			break;
		case 84: dest.x = 150; dest.y = 89; break; /*** popup_yn ***/
		case 85: dest.x = 440; dest.y = 317; break; /*** Yes ***/
		case 86: dest.x = 167; dest.y = 317; break; /*** No ***/
		case 87: dest.x = 629; dest.y = 63; break; /*** rooms broken ***/
		case 88: dest.x = 656; dest.y = 387; break; /*** custom ***/
		case 89:
			switch (iChangeForeRand)
			{
				case 0: dest.x = 33; break;
				case 1: dest.x = 48; break;
				case 2: dest.x = 63; break;
				case 3: dest.x = 78; break;
			}
			dest.y = 114;
			break;
		case 90:
			if (iChangeForeMod == 0) { dest.x = 114; dest.y = 114; }
			if (iChangeForeMod == 1) { dest.x = 129; dest.y = 114; }
			break;
		case 91: dest.x = 76; dest.y = 241; break; /*** current custom ***/
		case 92: dest.x = 77; dest.y = 146; break; /*** event target ***/
		case 93: dest.x = 620; dest.y = 3; break; /*** extras ***/
		case 94: dest.x = 17; dest.y = 17; break; /*** PoP1, problems ***/
		case 95: dest.x = 352; dest.y = 17; break; /*** PoP2 ***/
		case 96: dest.x = 254; dest.y = 324; break; /*** background ***/
		case 97: dest.x = 254; dest.y = 388; break; /*** guards ***/
		case 98: /*** guards in room ***/
			switch (iStaticGuards_Amount[iCurRoom - 1])
			{
				case 0: dest.x = 543; break;
				case 1: dest.x = 558; break;
				case 2: dest.x = 573; break;
				case 3: dest.x = 588; break;
				case 4: dest.x = 603; break;
				case 5: dest.x = 618; break;
			}
			dest.y = 191;
			break;
		case 99: /*** incoming sets ***/
			switch (iDynamicGuards_Sets[iCurRoom - 1])
			{
				case 0: dest.x = 566; break;
				case 1: dest.x = 581; break;
				case 2: dest.x = 596; break;
			}
			dest.y = 233;
			break;
		case 100: /*** 1st guard row ***/
			dest.x = 365;
			if (iStaticGuards_1_Locations[iCurRoom - 1][0] < 10) { dest.y = 17; }
			if ((iStaticGuards_1_Locations[iCurRoom - 1][0] >= 10) &&
				(iStaticGuards_1_Locations[iCurRoom - 1][0] < 20)) { dest.y = 24; }
			if (iStaticGuards_1_Locations[iCurRoom - 1][0] >= 20) { dest.y = 31; }
			break;
		case 101: /*** 2nd guard row ***/
			dest.x = 483;
			if (iStaticGuards_1_Locations[iCurRoom - 1][1] < 10) { dest.y = 17; }
			if ((iStaticGuards_1_Locations[iCurRoom - 1][1] >= 10) &&
				(iStaticGuards_1_Locations[iCurRoom - 1][1] < 20)) { dest.y = 24; }
			if (iStaticGuards_1_Locations[iCurRoom - 1][1] >= 20) { dest.y = 31; }
			break;
		case 102: /*** 3rd guard row ***/
			dest.x = 601;
			if (iStaticGuards_1_Locations[iCurRoom - 1][2] < 10) { dest.y = 17; }
			if ((iStaticGuards_1_Locations[iCurRoom - 1][2] >= 10) &&
				(iStaticGuards_1_Locations[iCurRoom - 1][2] < 20)) { dest.y = 24; }
			if (iStaticGuards_1_Locations[iCurRoom - 1][2] >= 20) { dest.y = 31; }
			break;
		case 103: /*** 4th guard row ***/
			dest.x = 365;
			if (iStaticGuards_1_Locations[iCurRoom - 1][3] < 10) { dest.y = 158; }
			if ((iStaticGuards_1_Locations[iCurRoom - 1][3] >= 10) &&
				(iStaticGuards_1_Locations[iCurRoom - 1][3] < 20)) { dest.y = 165; }
			if (iStaticGuards_1_Locations[iCurRoom - 1][3] >= 20) { dest.y = 172; }
			break;
		case 104: /*** 5th guard row ***/
			dest.x = 483;
			if (iStaticGuards_1_Locations[iCurRoom - 1][4] < 10) { dest.y = 158; }
			if ((iStaticGuards_1_Locations[iCurRoom - 1][4] >= 10) &&
				(iStaticGuards_1_Locations[iCurRoom - 1][4] < 20)) { dest.y = 165; }
			if (iStaticGuards_1_Locations[iCurRoom - 1][4] >= 20) { dest.y = 172; }
			break;
		case 105: /*** static guard checkboxes ***/
			/*** dest.x ***/
			switch (iStaticGuards_4_Directions[iCurRoom - 1][iModifier[1]])
			{
				case 0: /*** right ***/
					switch (iModifier[1])
					{
						case 0: dest.x = G1RIGHT; break;
						case 1: dest.x = G2RIGHT; break;
						case 2: dest.x = G3RIGHT; break;
						case 3: dest.x = G4RIGHT; break;
						case 4: dest.x = G5RIGHT; break;
					}
					break;
				case 255: /*** left ***/
					switch (iModifier[1])
					{
						case 0: dest.x = G1LEFT; break;
						case 1: dest.x = G2LEFT; break;
						case 2: dest.x = G3LEFT; break;
						case 3: dest.x = G4LEFT; break;
						case 4: dest.x = G5LEFT; break;
					}
					break;
			}
			/*** dest.y ***/
			switch (WhichStaticGuard (iModifier[1]))
			{
				case 1: dest.y = GTYPE1; break;
				case 2: dest.y = GTYPE2; break;
				case 3: dest.y = GTYPE3; break;
				case 4: dest.y = GTYPE4; break;
				case 5: dest.y = GTYPE5; break;
				case 6: dest.y = GTYPE6; break;
				case 7: dest.y = GTYPE7; break;
				case 8: dest.y = GTYPE8; break;
				case 9: dest.y = GTYPE9; break;
				case 10: dest.y = GTYPE10; break;
				case 11: dest.y = GTYPE11; break;
				case 12: dest.y = GTYPE12; break;
				case 13: dest.y = GTYPE13; break;
				default: dest.y = GTYPE1; break; /*** As a fallback. ***/
			}
			break;
		case 106: /*** entry 1st ***/
			switch (iDynamicGuards_3_Row[iCurRoom - 1][0])
			{
				case 0:
					/*** top left ***/
					if ((iDynamicGuards_4_Column[iCurRoom - 1][0] >= 0) &&
						(iDynamicGuards_4_Column[iCurRoom - 1][0] <= 5))
					{
						dest.x = 302; dest.y = 366;
					} else { /*** top right ***/
						dest.x = 319; dest.y = 366;
					}
					break;
				case 1:
					/*** middle left ***/
					if ((iDynamicGuards_4_Column[iCurRoom - 1][0] >= 0) &&
						(iDynamicGuards_4_Column[iCurRoom - 1][0] <= 5))
					{
						dest.x = 302; dest.y = 373;
					} else { /*** middle right ***/
						dest.x = 319; dest.y = 373;
					}
					break;
				case 2:
					/*** lower left ***/
					if ((iDynamicGuards_4_Column[iCurRoom - 1][0] >= 0) &&
						(iDynamicGuards_4_Column[iCurRoom - 1][0] <= 5))
					{
						dest.x = 302; dest.y = 380;
					} else { /*** lower right ***/
						dest.x = 319; dest.y = 380;
					}
					break;
			}
			break;
		case 107: /*** entry 2nd ***/
			switch (iDynamicGuards_3_Row[iCurRoom - 1][1])
			{
				case 0:
					/*** top left ***/
					if ((iDynamicGuards_4_Column[iCurRoom - 1][1] >= 0) &&
						(iDynamicGuards_4_Column[iCurRoom - 1][1] <= 5))
					{
						dest.x = 473; dest.y = 366;
					} else { /*** top right ***/
						dest.x = 490; dest.y = 366;
					}
					break;
				case 1:
					/*** middle left ***/
					if ((iDynamicGuards_4_Column[iCurRoom - 1][1] >= 0) &&
						(iDynamicGuards_4_Column[iCurRoom - 1][1] <= 5))
					{
						dest.x = 473; dest.y = 373;
					} else { /*** middle right ***/
						dest.x = 490; dest.y = 373;
					}
					break;
				case 2:
					/*** lower left ***/
					if ((iDynamicGuards_4_Column[iCurRoom - 1][1] >= 0) &&
						(iDynamicGuards_4_Column[iCurRoom - 1][1] <= 5))
					{
						dest.x = 473; dest.y = 380;
					} else { /*** lower right ***/
						dest.x = 490; dest.y = 380;
					}
					break;
			}
			break;
		case 108: /*** max alive ***/
			if (iModifier[1] == 0)
			{
				switch (iDynamicGuards_2_MaxTog[iCurRoom - 1][0])
				{
					case 1: dest.x = 438; dest.y = 311; break; /*** 1 ***/
					case 2: dest.x = 453; dest.y = 311; break; /*** 2 ***/
					default: dest.x = -100; dest.y = -100; break; /*** hide ***/
				}
			}
			if (iModifier[1] == 1)
			{
				switch (iDynamicGuards_2_MaxTog[iCurRoom - 1][1])
				{
					case 1: dest.x = 609; dest.y = 311; break; /*** 1 ***/
					case 2: dest.x = 624; dest.y = 311; break; /*** 2 ***/
					default: dest.x = -100; dest.y = -100; break; /*** hide ***/
				}
			}
			break;
		case 109: /*** can jump ***/
			if (iModifier[1] == 0)
			{
				switch (iDynamicGuards_7_CanJump[iCurRoom - 1][0])
				{
					case 255: dest.x = 438; dest.y = 331; break; /*** no ***/
					case 1: dest.x = 453; dest.y = 331; break; /*** yes ***/
					default: dest.x = -100; dest.y = -100; break; /*** hide ***/
				}
			}
			if (iModifier[1] == 1)
			{
				switch (iDynamicGuards_7_CanJump[iCurRoom - 1][1])
				{
					case 255: dest.x = 609; dest.y = 331; break; /*** no ***/
					case 1: dest.x = 624; dest.y = 331; break; /*** yes ***/
					default: dest.x = -100; dest.y = -100; break; /*** hide ***/
				}
			}
			break;
		case 110: /*** spawn if guard(s) alive ***/
			if (iModifier[1] == 0)
			{
				switch (iDynamicGuards_8_SpIfAl[iCurRoom - 1][0])
				{
					case 0: dest.x = 354; dest.y = 321; break;
					case 1: dest.x = 369; dest.y = 321; break;
					case 2: dest.x = 384; dest.y = 321; break;
				}
			}
			if (iModifier[1] == 1)
			{
				switch (iDynamicGuards_8_SpIfAl[iCurRoom - 1][1])
				{
					case 0: dest.x = 525; dest.y = 321; break;
					case 1: dest.x = 540; dest.y = 321; break;
					case 2: dest.x = 555; dest.y = 321; break;
				}
			}
			break;
		case 111: /*** static left ***/
			switch (iModifier[1])
			{
				case 0: dest.x = 14; dest.y = 14; break;
				case 1: dest.x = 56; dest.y = 14; break;
				case 2: dest.x = 98; dest.y = 14; break;
				case 3: dest.x = 140; dest.y = 14; break;
				case 4: dest.x = 182; dest.y = 14; break;
			}
			break;
		case 112: /*** static right ***/
			switch (iModifier[1])
			{
				case 0: dest.x = 287; dest.y = 14; break;
				case 1: dest.x = 405; dest.y = 14; break;
				case 2: dest.x = 523; dest.y = 14; break;
				case 3: dest.x = 287; dest.y = 155; break;
				case 4: dest.x = 405; dest.y = 155; break;
			}
			break;
		case 113: dest.x = 299; dest.y = 307; break; /*** 1st dynamic ***/
		case 114: dest.x = 470; dest.y = 307; break; /*** 2nd dynamic ***/
		case 115: /*** X-coordinate tooltip ***/
			switch (iModifier[1])
			{
				case 0: dest.x = 288; dest.y = 15; break;
				case 1: dest.x = 406; dest.y = 15; break;
				case 2: dest.x = 524; dest.y = 15; break;
				case 3: dest.x = 288; dest.y = 156; break;
				case 4: dest.x = 406; dest.y = 156; break;
			}
			break;
		case 116: /*** incoming arrow ***/
			if ((iDynamicGuards_4_Column[iCurRoom - 1][iModifier[1]] >= 0) &&
				(iDynamicGuards_4_Column[iCurRoom - 1][iModifier[1]] <= 5))
			{
				dest.x = 25;
			} else {
				dest.x = 654;
			}
			switch (iDynamicGuards_3_Row[iCurRoom - 1][iModifier[1]])
			{
				case 0: dest.y = 100; break;
				case 1: dest.y = 226; break;
				case 2: dest.y = 352; break;
			}
			break;
		case 117: /*** static guard location ***/
			dest.x = 25 + ((IntsToHexToSSLittleEndian (iStaticGuards_3_Offsets2
				[iCurRoom - 1][iModifier[1]], iStaticGuards_2_Offsets1
				[iCurRoom - 1][iModifier[1]]) - 144) * 2);
			switch (WhichStaticGuard (iModifier[1]))
			{
				case 1: case 2: case 3: iBaseY = 76; break; /*** guards ***/
				case 4: iBaseY = 121; break; /*** s. wakes up; wobbly if jump ***/
				case 5: iBaseY = 87; break; /*** skeleton already awake ***/
				case 6: iBaseY = 147; break; /*** skeleton only sleeps ***/
				case 7: iBaseY = 121; break; /*** s. wakes up; NOT wobbly if jump ***/
				case 8: iBaseY = 93; break; /*** head, long hair ***/
				case 9: iBaseY = 95; break; /*** head, short hair ***/
				case 10: iBaseY = 147; break; /*** snake ***/
				case 11: iBaseY = 75; break; /*** birdman ***/
				case 12: iBaseY = 152; break; /*** sword ***/
				case 13: iBaseY = 78; break; /*** fake prince ***/
			}
			if (iStaticGuards_1_Locations[iCurRoom - 1][iModifier[1]] < 10)
			{
				dest.y = iBaseY;
			}
			if ((iStaticGuards_1_Locations[iCurRoom - 1][iModifier[1]] >= 10) &&
				(iStaticGuards_1_Locations[iCurRoom - 1][iModifier[1]] < 20))
			{
				dest.y = iBaseY + 126;
			}
			if (iStaticGuards_1_Locations[iCurRoom - 1][iModifier[1]] >= 20)
			{
				dest.y = iBaseY + 126 + 126;
			}
			break;
		case 118: /*** foreground elements ***/
			switch (iModifier[0])
			{
				case 1: dest.x = 264; dest.y = 248; break; /*** horse ***/
				case 2: dest.x = 38; dest.y = 218; break; /*** arrow ***/
				case 3: dest.x = 30; dest.y = 409; break; /*** invisible ***/
			}
			break;
		case 119: dest.x = 637; dest.y = 305; break; /*** music ***/
		case 120: /*** music checkboxes ***/
			if ((iModifier[1] >= 0) && (iModifier[1] <= 9)) { dest.y = 130; }
			if ((iModifier[1] >= 10) && (iModifier[1] <= 19)) { dest.y = 145; }
			if ((iModifier[1] >= 20) && (iModifier[1] <= 29)) { dest.y = 160; }
			switch (iModifier[1])
			{
				case 0: case 10: case 20: dest.x = 253 + (0 * 15); break;
				case 1: case 11: case 21: dest.x = 253 + (1 * 15); break;
				case 2: case 12: case 22: dest.x = 253 + (2 * 15); break;
				case 3: case 13: case 23: dest.x = 253 + (3 * 15); break;
				case 4: case 14: case 24: dest.x = 253 + (4 * 15); break;
				case 5: case 15: case 25: dest.x = 253 + (5 * 15); break;
				case 6: case 16: case 26: dest.x = 253 + (6 * 15); break;
				case 7: case 17: case 27: dest.x = 253 + (7 * 15); break;
				case 8: case 18: case 28: dest.x = 253 + (8 * 15); break;
				case 9: case 19: case 29: dest.x = 253 + (9 * 15); break;
			}
			break;
		case 121: /*** edit/end for left/right ***/
			switch (iModifier[1])
			{
				case 1: dest.x = 251; break;
				case 2: dest.x = 346; break;
				case 3: dest.x = 476; break;
				case 4: dest.x = 571; break;
			}
			dest.y = 254;
			break;
		case 122: /* music buttons ***/
			switch (iModifier[0])
			{
				case 1: case 2: dest.x = 32; dest.y = 115; break;
				case 3: case 4: dest.x = 32; dest.y = 145; break;
				case 5: case 6: dest.x = 104; dest.y = 115; break;
				case 7: case 8: dest.x = 104; dest.y = 145; break;
				case 9: case 10: dest.x = 481; dest.y = 115; break;
				case 11: case 12: dest.x = 481; dest.y = 145; break;
				case 13: case 14: dest.x = 553; dest.y = 115; break;
				case 15: case 16: dest.x = 553; dest.y = 145; break;
			}
			break;
		case 123: /*** PoP2 prince ***/
			dest.x = TILESX8; dest.y = TILESY7; break;
		case 124: /*** guard warning static ***/
			dest.x = 18; dest.y = 47; break;
		case 125: /*** jumps ***/
			switch (iChangeJumps)
			{
				case 1: dest.x = 588; break;
				case 2: dest.x = 603; break;
				case 3: dest.x = 618; break;
				case 4: dest.x = 633; break;
			}
			dest.y = 265;
			break;
		case 126: /*** guard warning dynamic ***/
			dest.x = 297; dest.y = 305; break;
		case 127: /*** broken room links 25-32 ***/
			dest.x = 34; dest.y = 282; break;
		case 128: /*** room links 25-32 ***/
			dest.x = 175; dest.y = 290; break;
		case 129: /*** sel back ***/
			if (cCurType == 'r')
			{
				dest.x = 601; dest.y = 159; /*** Default to 0. ***/
				switch (iModifier[2])
				{
					case 1:
						switch (iModifier[3])
						{
							case 0: dest.x = 23; dest.y = 86; break;
							case 1: dest.x = 55; dest.y = 86; break;
							case 4: dest.x = 23; dest.y = 23; break;
							case 5: dest.x = 55; dest.y = 23; break;
						}
						break;
					case 2:
						switch (iModifier[3])
						{
							case 0: dest.x = 97; dest.y = 86; break;
							case 1: dest.x = 129; dest.y = 86; break;
							case 4: dest.x = 97; dest.y = 23; break;
							case 5: dest.x = 129; dest.y = 23; break;
						}
						break;
					case 3:
						switch (iModifier[3])
						{
							case 0: dest.x = 171; dest.y = 86; break;
							case 1: dest.x = 203; dest.y = 86; break;
							case 4: dest.x = 171; dest.y = 23; break;
							case 5: dest.x = 203; dest.y = 23; break;
						}
						break;
					case 4:
						switch (iModifier[3])
						{
							case 0: dest.x = 245; dest.y = 86; break;
							case 1: dest.x = 277; dest.y = 86; break;
							case 4: dest.x = 245; dest.y = 23; break;
							case 5: dest.x = 277; dest.y = 23; break;
						}
						break;
					case 5:
						switch (iModifier[3])
						{
							case 0: dest.x = 319; dest.y = 86; break;
							case 1: dest.x = 351; dest.y = 86; break;
							case 2: dest.x = 383; dest.y = 86; break;
							case 3: dest.x = 415; dest.y = 86; break;
							case 4: dest.x = 319; dest.y = 23; break;
							case 5: dest.x = 351; dest.y = 23; break;
							case 6: dest.x = 383; dest.y = 23; break;
							case 7: dest.x = 415; dest.y = 23; break;
						}
						break;
					case 6:
						switch (iModifier[3])
						{
							case 0: dest.x = 467; dest.y = 149; break;
							case 1: dest.x = 499; dest.y = 149; break;
							case 4: dest.x = 467; dest.y = 86; break;
							case 5: dest.x = 499; dest.y = 86; break;
							case 8: dest.x = 467; dest.y = 23; break;
							case 9: dest.x = 499; dest.y = 23; break;
						}
						break;
					case 7:
						switch (iModifier[3])
						{
							case 0: dest.x = 23; dest.y = 222; break;
							case 1: dest.x = 55; dest.y = 222; break;
							case 4: dest.x = 23; dest.y = 159; break;
							case 5: dest.x = 55; dest.y = 159; break;
						}
						break;
					case 8:
						switch (iModifier[3])
						{
							case 0: dest.x = 97; dest.y = 222; break;
							case 1: dest.x = 129; dest.y = 222; break;
							case 2: dest.x = 161; dest.y = 222; break;
							case 3: dest.x = 193; dest.y = 222; break;
							case 4: dest.x = 97; dest.y = 159; break;
							case 5: dest.x = 129; dest.y = 159; break;
							case 6: dest.x = 161; dest.y = 159; break;
							case 7: dest.x = 193; dest.y = 159; break;
						}
						break;
					case 9:
						switch (iModifier[3])
						{
							case 0: dest.x = 235; dest.y = 159; break;
							case 1: dest.x = 267; dest.y = 159; break;
						}
						break;
					case 10:
						switch (iModifier[3])
						{
							case 0: dest.x = 309; dest.y = 159; break;
							case 1: dest.x = 341; dest.y = 159; break;
						}
						break;
					case 12: dest.x = 383; dest.y = 159; break;
					case 13: dest.x = 425; dest.y = 159; break;
					case 14: dest.x = 23; dest.y = 295; break;
					case 15: dest.x = 65; dest.y = 295; break;
					case 16: dest.x = 107; dest.y = 295; break;
					case 17: dest.x = 149; dest.y = 295; break;
					case 18: dest.x = 191; dest.y = 295; break;
					case 19: dest.x = 233; dest.y = 295; break;
					case 20: dest.x = 275; dest.y = 295; break;
					case 22: dest.x = 23; dest.y = 368; break;
					case 23: dest.x = 65; dest.y = 368; break;
					case 24: dest.x = 107; dest.y = 368; break;
					case 25: dest.x = 149; dest.y = 368; break;
					case 26: dest.x = 191; dest.y = 368; break;
					case 30: dest.x = 233; dest.y = 368; break;
					case 31: dest.x = 275; dest.y = 368; break;
				}
			}
			if (cCurType == 't')
			{
				dest.x = 344; dest.y = 373; /*** Default to 0. ***/
				switch (iModifier[2])
				{
					case 1: dest.x = 18; dest.y = 18; break;
					case 2: dest.x = 60; dest.y = 18; break;
					case 3: dest.x = 102; dest.y = 18; break;
					case 4: dest.x = 144; dest.y = 18; break;
					case 5: dest.x = 186; dest.y = 18; break;
					case 6: dest.x = 228; dest.y = 18; break;
					case 7: dest.x = 270; dest.y = 18; break;
					case 8: dest.x = 60; dest.y = 91; break;
					case 9: dest.x = 144; dest.y = 91; break;
					case 10: dest.x = 102; dest.y = 91; break;
					case 11: dest.x = 18; dest.y = 91; break;
					case 12: dest.x = 186; dest.y = 91; break;
					case 13: dest.x = 228; dest.y = 91; break;
					case 14: dest.x = 270; dest.y = 91; break;
					case 15: dest.x = 186; dest.y = 164; break;
					case 16: dest.x = 228; dest.y = 164; break;
					case 17: dest.x = 270; dest.y = 164; break;
					case 18:
						switch (iModifier[3])
						{
							case 0: dest.x = 312; dest.y = 18; break;
							case 1: dest.x = 344; dest.y = 18; break;
						}
						break;
					case 19:
						switch (iModifier[3])
						{
							case 0: dest.x = 18; dest.y = 227; break;
							case 1: dest.x = 50; dest.y = 227; break;
							case 4: dest.x = 18; dest.y = 164; break;
							case 5: dest.x = 50; dest.y = 164; break;
						}
						break;
					case 20:
						switch (iModifier[3])
						{
							case 0: dest.x = 102; dest.y = 227; break;
							case 1: dest.x = 134; dest.y = 227; break;
							case 4: dest.x = 102; dest.y = 164; break;
							case 5: dest.x = 134; dest.y = 164; break;
						}
						break;
					case 21: dest.x = 102; dest.y = 373; break;
					case 22: dest.x = 18; dest.y = 300; break;
					case 23: dest.x = 60; dest.y = 300; break;
					case 24: dest.x = 102; dest.y = 300; break;
					case 25:
						switch (iModifier[3])
						{
							case 0: dest.x = 144; dest.y = 300; break;
							case 1: dest.x = 176; dest.y = 300; break;
						}
						break;
					case 26:
						switch (iModifier[3])
						{
							case 0: dest.x = 218; dest.y = 300; break;
							case 1: dest.x = 250; dest.y = 300; break;
						}
						break;
					case 27: dest.x = 292; dest.y = 300; break;
					case 28: dest.x = 18; dest.y = 373; break;
					case 29: dest.x = 60; dest.y = 373; break;
					case 30: dest.x = 144; dest.y = 373; break;
					case 31: dest.x = 186; dest.y = 373; break;
				}
			}
			break;
		case 130: /*** alt wall ***/
			dest.y = 287;
			if (cCurType == 'c')
			{
				switch (iChangeWall)
				{
					case 1: dest.x = 618; break;
					case 2: dest.x = 633; break;
				}
			}
			if (cCurType == 'r')
			{
				switch (iChangeWall)
				{
					case 1: dest.x = 588; break;
					case 2: dest.x = 603; break;
					case 3: dest.x = 618; break;
					case 4: dest.x = 633; break;
				}
			}
			if (cCurType == 't')
			{
				switch (iChangeWall)
				{
					case 1: dest.x = 603; break;
					case 2: dest.x = 618; break;
					case 3: dest.x = 633; break;
				}
			}
			break;
		case 131: /*** stal ***/
			switch (iChangeStal)
			{
				case 0: dest.x = 231; dest.y = 393; break;
				case 1: dest.x = 11; dest.y = 347; break;
				case 2: dest.x = 67; dest.y = 347; break;
				case 3: dest.x = 117; dest.y = 347; break;
				case 4: dest.x = 185; dest.y = 347; break;
				case 5: dest.x = 11; dest.y = 393; break;
				case 6: dest.x = 95; dest.y = 393; break;
				case 7: dest.x = 145; dest.y = 393; break;
				default: printf ("[ WARN ] iChangeStal is: %i\n", iChangeStal); break;
			}
			break;
		case 132: /*** bot ***/
			switch (iChangeBot)
			{
				case 0: dest.x = 411; dest.y = 283; break;
				case 1: dest.x = 411; dest.y = 304; break;
				case 2: dest.x = 411; dest.y = 325; break;
				case 3: dest.x = 411; dest.y = 346; break;
				case 4: dest.x = 411; dest.y = 367; break;
				case 5: dest.x = 411; dest.y = 388; break;
				case 6: dest.x = 411; dest.y = 409; break;
				case 7: dest.x = 411; dest.y = 430; break;
				default: printf ("[ WARN ] iChangeBot is: %i\n", iChangeBot); break;
			}
			break;
		case 133: /*** sym ***/
			switch (iChangeSym)
			{
				case 0: dest.x = 411; dest.y = 429; break;
				case 1: dest.x = 411; dest.y = 304; break;
				case 2: dest.x = 411; dest.y = 362; break;
				case 3: dest.x = 411; dest.y = 404; break;
				default:
					printf ("[ WARN ] iChangeSym is: %i\n", iChangeSym);
					break;
			}
			break;
		case 134: /*** crk ***/
			switch (iChangeCrk)
			{
				case 0: dest.x = 231; dest.y = 415; break;
				case 1: dest.x = 11; dest.y = 352; break;
				case 3: dest.x = 115; dest.y = 331; break;
				case 4: dest.x = 161; dest.y = 331; break;
				case 5: dest.x = 202; dest.y = 331; break;
				case 6: dest.x = 11; dest.y = 380; break;
				case 7: dest.x = 85; dest.y = 380; break;
				case 8: dest.x = 85; dest.y = 415; break;
				case 9: dest.x = 11; dest.y = 415; break;
				case 10: dest.x = 143; dest.y = 415; break;
				case 11: dest.x = 143; dest.y = 380; break;
				case 12: dest.x = 231; dest.y = 380; break;
				default:
					printf ("[ WARN ] iChangeCrk is: %i\n", iChangeCrk);
					break;
			}
			break;
		case 135: dest.x = 327; dest.y = 183; break; /*** hourglass sprite ***/
		case 136: dest.x = 345; dest.y = 215; break; /*** sand sprite ***/
		case 137: dest.x = 4; dest.y = 4; break; /*** distort ***/
	}
	if ((iLocation >= 2010) && (iLocation <= 2324)) /*** adjacent rooms ***/
	{
		snprintf (sAdj, MAX_ADJ, "%i", iLocation);
		snprintf (sAdjR, MAX_ADJ, "%c%c", sAdj[1], sAdj[2]);
		snprintf (sAdjL, MAX_ADJ, "%c", sAdj[3]);

		switch (atoi (sAdjL))
		{
			case 0:
				dest.x = BROKEN_ROOM_X;
				dest.y = BROKEN_ROOM_Y;
				break;
			case 1:
				dest.x = BROKEN_LEFT_X;
				dest.y = BROKEN_LEFT_Y;
				break;
			case 2:
				dest.x = BROKEN_RIGHT_X;
				dest.y = BROKEN_RIGHT_Y;
				break;
			case 3:
				dest.x = BROKEN_UP_X;
				dest.y = BROKEN_UP_Y;
				break;
			case 4:
				dest.x = BROKEN_DOWN_X;
				dest.y = BROKEN_DOWN_Y;
				break;
		}

		switch (atoi (sAdjR))
		{
			case 1: dest.x += (63 * 0); dest.y += (63 * 0); break;
			case 2: dest.x += (63 * 1); dest.y += (63 * 0); break;
			case 3: dest.x += (63 * 2); dest.y += (63 * 0); break;
			case 4: dest.x += (63 * 3); dest.y += (63 * 0); break;
			case 5: dest.x += (63 * 0); dest.y += (63 * 1); break;
			case 6: dest.x += (63 * 1); dest.y += (63 * 1); break;
			case 7: dest.x += (63 * 2); dest.y += (63 * 1); break;
			case 8: dest.x += (63 * 3); dest.y += (63 * 1); break;
			case 9: dest.x += (63 * 0); dest.y += (63 * 2); break;
			case 10: dest.x += (63 * 1); dest.y += (63 * 2); break;
			case 11: dest.x += (63 * 2); dest.y += (63 * 2); break;
			case 12: dest.x += (63 * 3); dest.y += (63 * 2); break;
			case 13: dest.x += (63 * 0); dest.y += (63 * 3); break;
			case 14: dest.x += (63 * 1); dest.y += (63 * 3); break;
			case 15: dest.x += (63 * 2); dest.y += (63 * 3); break;
			case 16: dest.x += (63 * 3); dest.y += (63 * 3); break;
			case 17: dest.x += (63 * 0); dest.y += (63 * 4); break;
			case 18: dest.x += (63 * 1); dest.y += (63 * 4); break;
			case 19: dest.x += (63 * 2); dest.y += (63 * 4); break;
			case 20: dest.x += (63 * 3); dest.y += (63 * 4); break;
			case 21: dest.x += (63 * 0); dest.y += (63 * 5); break;
			case 22: dest.x += (63 * 1); dest.y += (63 * 5); break;
			case 23: dest.x += (63 * 2); dest.y += (63 * 5); break;
			case 24: dest.x += (63 * 3); dest.y += (63 * 5); break;
			case 25: dest.x -= (63 * 4); dest.y += (63 * 4); break;
			case 26: dest.x -= (63 * 3); dest.y += (63 * 4); break;
			case 27: dest.x -= (63 * 2); dest.y += (63 * 4); break;
			case 28: dest.x -= (63 * 1); dest.y += (63 * 4); break;
			case 29: dest.x -= (63 * 4); dest.y += (63 * 5); break;
			case 30: dest.x -= (63 * 3); dest.y += (63 * 5); break;
			case 31: dest.x -= (63 * 2); dest.y += (63 * 5); break;
			case 32: dest.x -= (63 * 1); dest.y += (63 * 5); break;
		}
	}
	if ((iLocation >= 10101) && (iLocation <= 12532)) /*** room grid ***/
	{
		snprintf (sGrid, MAX_GRID, "%i", iLocation);
		snprintf (sGridX, MAX_GRID, "%c%c", sGrid[1], sGrid[2]);
		snprintf (sGridY, MAX_GRID, "%c%c", sGrid[3], sGrid[4]);

		/*** save room location ***/
		if (iThing != -7)
		{
			iRoomArray[atoi (sGridX)][atoi (sGridY)] = iModifier[0];
		}

		if (strcmp (sGridX, "25") == 0) /*** side pane ***/
		{
			if (atoi (sGridY) <= 24)
			{
				dest.x = 272; /*** right pane ***/
			} else {
				dest.x = 247; /*** left pane ***/
			}
		} else {
			dest.x = 282 + (atoi (sGridX) * 15); /*** grid, 24x24 ***/
		}
		if (atoi (sGridY) <= 24)
		{
			dest.y = 46 + (atoi (sGridY) * 15); /*** grid, 24x24 & right pane ***/
		} else {
			dest.y = 286 + ((atoi (sGridY) - 24) * 15); /*** left pane ***/
		}
	}

	/*** For some reason I don't need to set ***/
	/*** dest.w = img->w and dest.h = img->h ***/

	if (iThing == -4)
	{
		switch (iModifier[0])
		{
			case 1: /*** black background ***/
				CustomBlitSurface (imgblack, "imgblack", &loc,
					screen, &dest, arImage); break;
			case 2: /*** hourglass ***/
				CustomBlitSurface (imghourglass, "imghourglass", &loc,
					screen, &dest, arImage); break;
			case 3: /*** hourglass sprite ***/
				loc.x = ((iHourglassFrame * 38) * iScale);
				CustomBlitSurface (imghourglasssprite, "imghourglasssprite", &loc,
					screen, &dest, arImage); break;
			case 4: /*** sand sprite ***/
				loc.x = (((iSandFrame - 1) * 2) * iScale);
				CustomBlitSurface (imgsandsprite, "imgsandsprite", &loc,
					screen, &dest, arImage); break;
		}
	}

	/*** room links selections ***/
	if (iThing == -7)
	{
		switch (iModifier[0])
		{
			case 0:
				CustomBlitSurface (imgsrc, "imgsrc", &loc,
					screen, &dest, arImage); break;
			case 1:
				CustomBlitSurface (imgsrs, "imgsrs", &loc,
					screen, &dest, arImage); break;
			case 2:
				CustomBlitSurface (imgsrm, "imgsrm", &loc,
					screen, &dest, arImage); break;
			case 3:
				CustomBlitSurface (imgsrp, "imgsrp", &loc,
					screen, &dest, arImage); break;
			case 4:
				CustomBlitSurface (imgsrb, "imgsrb", &loc,
					screen, &dest, arImage); break;
		}
	}

	/*** rooms ***/
	if (iThing == -6)
	{
		switch (iModifier[0])
		{
			case 0:
				CustomBlitSurface (imgrl, "imgrl", &loc,
					screen, &dest, arImage); break;
			case 1:
				CustomBlitSurface (imgroom1, "imgroom1", &loc,
					screen, &dest, arImage); break;
			case 2:
				CustomBlitSurface (imgroom2, "imgroom2", &loc,
					screen, &dest, arImage); break;
			case 3:
				CustomBlitSurface (imgroom3, "imgroom3", &loc,
					screen, &dest, arImage); break;
			case 4:
				CustomBlitSurface (imgroom4, "imgroom4", &loc,
					screen, &dest, arImage); break;
			case 5:
				CustomBlitSurface (imgroom5, "imgroom5", &loc,
					screen, &dest, arImage); break;
			case 6:
				CustomBlitSurface (imgroom6, "imgroom6", &loc,
					screen, &dest, arImage); break;
			case 7:
				CustomBlitSurface (imgroom7, "imgroom7", &loc,
					screen, &dest, arImage); break;
			case 8:
				CustomBlitSurface (imgroom8, "imgroom8", &loc,
					screen, &dest, arImage); break;
			case 9:
				CustomBlitSurface (imgroom9, "imgroom9", &loc,
					screen, &dest, arImage); break;
			case 10:
				CustomBlitSurface (imgroom10, "imgroom10", &loc,
					screen, &dest, arImage); break;
			case 11:
				CustomBlitSurface (imgroom11, "imgroom11", &loc,
					screen, &dest, arImage); break;
			case 12:
				CustomBlitSurface (imgroom12, "imgroom12", &loc,
					screen, &dest, arImage); break;
			case 13:
				CustomBlitSurface (imgroom13, "imgroom13", &loc,
					screen, &dest, arImage); break;
			case 14:
				CustomBlitSurface (imgroom14, "imgroom14", &loc,
					screen, &dest, arImage); break;
			case 15:
				CustomBlitSurface (imgroom15, "imgroom15", &loc,
					screen, &dest, arImage); break;
			case 16:
				CustomBlitSurface (imgroom16, "imgroom16", &loc,
					screen, &dest, arImage); break;
			case 17:
				CustomBlitSurface (imgroom17, "imgroom17", &loc,
					screen, &dest, arImage); break;
			case 18:
				CustomBlitSurface (imgroom18, "imgroom18", &loc,
					screen, &dest, arImage); break;
			case 19:
				CustomBlitSurface (imgroom19, "imgroom19", &loc,
					screen, &dest, arImage); break;
			case 20:
				CustomBlitSurface (imgroom20, "imgroom20", &loc,
					screen, &dest, arImage); break;
			case 21:
				CustomBlitSurface (imgroom21, "imgroom21", &loc,
					screen, &dest, arImage); break;
			case 22:
				CustomBlitSurface (imgroom22, "imgroom22", &loc,
					screen, &dest, arImage); break;
			case 23:
				CustomBlitSurface (imgroom23, "imgroom23", &loc,
					screen, &dest, arImage); break;
			case 24:
				CustomBlitSurface (imgroom24, "imgroom24", &loc,
					screen, &dest, arImage); break;
			case 25:
				CustomBlitSurface (imgroom25, "imgroom25", &loc,
					screen, &dest, arImage); break;
			case 26:
				CustomBlitSurface (imgroom26, "imgroom26", &loc,
					screen, &dest, arImage); break;
			case 27:
				CustomBlitSurface (imgroom27, "imgroom27", &loc,
					screen, &dest, arImage); break;
			case 28:
				CustomBlitSurface (imgroom28, "imgroom28", &loc,
					screen, &dest, arImage); break;
			case 29:
				CustomBlitSurface (imgroom29, "imgroom29", &loc,
					screen, &dest, arImage); break;
			case 30:
				CustomBlitSurface (imgroom30, "imgroom30", &loc,
					screen, &dest, arImage); break;
			case 31:
				CustomBlitSurface (imgroom31, "imgroom31", &loc,
					screen, &dest, arImage); break;
			case 32:
				CustomBlitSurface (imgroom32, "imgroom32", &loc,
					screen, &dest, arImage); break;
			case 33:
				CustomBlitSurface (imgbrl, "imgbrl", &loc,
					screen, &dest, arImage); break;
			case 34:
				CustomBlitSurface (imgbrl2, "imgbrl2", &loc,
					screen, &dest, arImage); break;
			case 35:
				CustomBlitSurface (imgrl2, "imgrl2", &loc,
					screen, &dest, arImage); break;
		}
	}

	/*** events ***/
	if (iThing == -10)
	{
		switch (iModifier[0])
		{
			case 0:
				CustomBlitSurface (imgevents, "imgevents", &loc,
					screen, &dest, arImage); break;
			case 1:
				CustomBlitSurface (imgsele, "imgsele", &loc,
					screen, &dest, arImage); break;
		}
	}

	/*** faded tile ***/
	if (iThing == -11)
	{
		if (iModifier[0] == 0)
			{ CustomBlitSurface (imgfadeds, "imgfadeds", &loc,
				screen, &dest, arImage); }
	}

	/*** changing ***/
	if (iThing == -12)
	{
		switch (iModifier[0])
		{
			case 0:
				CustomBlitSurface (imgdungeon, "imgdungeon", &loc,
					screen, &dest, arImage); break;
			case 1:
				CustomBlitSurface (imgpalace, "imgpalace", &loc,
					screen, &dest, arImage); break;
			case 2:
				CustomBlitSurface (imgclose_0, "imgclose_0", &loc,
					screen, &dest, arImage); break;
			case 3:
				CustomBlitSurface (imgclose_1, "imgclose_1", &loc,
					screen, &dest, arImage); break;
			case 4:
				if (IsDisabled (iOnTile) == 0)
				{
					/*** Show bright green border. ***/
					if (iOnTile <= (7 * 13))
						CustomBlitSurface (imgborderb, "imgborderb", &loc,
							screen, &dest, arImage);
					else CustomBlitSurface (imgborders, "imgborders", &loc,
						screen, &dest, arImage);
				}
				break;
			case 5:
				CustomBlitSurface (imgdother, "imgdother", &loc,
					screen, &dest, arImage); break;
			case 6:
				CustomBlitSurface (imgdother, "imgdother", &loc,
					screen, &dest, arImage); break;
			case 7:
				CustomBlitSurface (imgdjaffar, "imgdjaffar", &loc,
					screen, &dest, arImage); break;
			case 8:
				CustomBlitSurface (imgdother, "imgdother", &loc,
					screen, &dest, arImage); break;
			case 9:
				CustomBlitSurface (imgdguards, "imgdguards", &loc,
					screen, &dest, arImage); break;
			case 10:
				CustomBlitSurface (imgbordersl, "imgbordersl", &loc,
					screen, &dest, arImage); break;
			case 11:
				if (iIsCustom == 0)
					CustomBlitSurface (imgborderbl, "imgborderbl", &loc,
						screen, &dest, arImage);
				break;
			case 12:
				CustomBlitSurface (imgsell, "imgsell", &loc,
					screen, &dest, arImage); break;
			case 13:
				CustomBlitSurface (imgcustoma0, "imgcustoma0", &loc,
					screen, &dest, arImage); break;
			case 14:
				CustomBlitSurface (imgcustomi0, "imgcustomi0", &loc,
					screen, &dest, arImage); break;
			case 15:
				CustomBlitSurface (imgcustoma1, "imgcustoma1", &loc,
					screen, &dest, arImage); break;
			case 16:
				CustomBlitSurface (imgcustomi1, "imgcustomi1", &loc,
					screen, &dest, arImage); break;
			case 17:
				CustomBlitSurface (imgcustom, "imgcustom", &loc,
					screen, &dest, arImage); break;
			case 18:
				CustomBlitSurface (imgcustom_0, "imgcustom_0", &loc,
					screen, &dest, arImage); break;
			case 19:
				CustomBlitSurface (imgcustom_1, "imgcustom_1", &loc,
					screen, &dest, arImage); break;
			case 20:
				CustomBlitSurface (imgoth, "imgoth", &loc,
					screen, &dest, arImage); break;
			case 21:
				CustomBlitSurface (imgcaverns, "imgcaverns", &loc,
					screen, &dest, arImage); break;
			case 22:
				CustomBlitSurface (imgruins, "imgruins", &loc,
					screen, &dest, arImage); break;
			case 23:
				CustomBlitSurface (imgruins_back, "imgruins_back", &loc,
					screen, &dest, arImage); break;
			case 24:
				CustomBlitSurface (imgtemple, "imgtemple", &loc,
					screen, &dest, arImage); break;
			case 25:
				CustomBlitSurface (imgtemple_back, "imgtemple_back", &loc,
					screen, &dest, arImage); break;
			case 26:
				CustomBlitSurface (imgback_0, "imgback_0", &loc,
					screen, &dest, arImage); break;
			case 27:
				CustomBlitSurface (imgback_1, "imgback_1", &loc,
					screen, &dest, arImage); break;
			case 28:
				CustomBlitSurface (imgguards_0, "imgguards_0", &loc,
					screen, &dest, arImage); break;
			case 29:
				CustomBlitSurface (imgguards_1, "imgguards_1", &loc,
					screen, &dest, arImage); break;
			case 30:
				CustomBlitSurface (imgguards, "imgguards", &loc,
					screen, &dest, arImage); break;
			case 31:
				CustomBlitSurface (imgclosebig_0, "imgclosebig_0", &loc,
					screen, &dest, arImage); break;
			case 32:
				CustomBlitSurface (imgclosebig_1, "imgclosebig_1", &loc,
					screen, &dest, arImage); break;
			case 33:
				CustomBlitSurface (imgguard_row, "imgguard_row", &loc,
					screen, &dest, arImage); break;
			case 34:
				CustomBlitSurface (imgguard_entry, "imgguard_entry", &loc,
					screen, &dest, arImage); break;
			case 35:
				CustomBlitSurface (imgfadeddg, "imgfadeddg", &loc,
					screen, &dest, arImage); break;
			case 36:
				CustomBlitSurface (imgfadedsgl, "imgfadedsgl", &loc,
					screen, &dest, arImage); break;
			case 37:
				CustomBlitSurface (imgfadedsgr, "imgfadedsgr", &loc,
					screen, &dest, arImage); break;
			case 38:
				CustomBlitSurface (imgxtooltip, "imgxtooltip", &loc,
					screen, &dest, arImage); break;
			case 39:
				CustomBlitSurface (imgmusic_0, "imgmusic_0", &loc,
					screen, &dest, arImage); break;
			case 40:
				CustomBlitSurface (imgmusic_1, "imgmusic_1", &loc,
					screen, &dest, arImage); break;
			case 41:
				CustomBlitSurface (imgmusic, "imgmusic", &loc,
					screen, &dest, arImage); break;
			case 42:
				CustomBlitSurface (imgee_0, "imgee_0", &loc,
					screen, &dest, arImage); break;
			case 43:
				CustomBlitSurface (imgee_1, "imgee_1", &loc,
					screen, &dest, arImage); break;
			case 44:
				CustomBlitSurface (imgec_0, "imgec_0", &loc,
					screen, &dest, arImage); break;
			case 45:
				CustomBlitSurface (imgec_1, "imgec_1", &loc,
					screen, &dest, arImage); break;
			case 46:
				CustomBlitSurface (imgprincew, "imgprincew", &loc,
					screen, &dest, arImage); break;
			case 47:
				CustomBlitSurface (imggw_rooftops, "imggw_rooftops", &loc,
					screen, &dest, arImage); break;
			case 48:
				CustomBlitSurface (imggw_desert, "imggw_desert", &loc,
					screen, &dest, arImage); break;
			case 49:
				CustomBlitSurface (imggw_caverns, "imggw_caverns", &loc,
					screen, &dest, arImage); break;
			case 50:
				CustomBlitSurface (imggw_ruins, "imggw_ruins", &loc,
					screen, &dest, arImage); break;
			case 51:
				CustomBlitSurface (imggw_temple, "imggw_temple", &loc,
					screen, &dest, arImage); break;
			case 52:
				CustomBlitSurface (imggw_final, "imggw_final", &loc,
					screen, &dest, arImage); break;
			case 53:
				CustomBlitSurface (imggw_dynamic, "imggw_dynamic", &loc,
					screen, &dest, arImage); break;
			case 54:
				CustomBlitSurface (imgsb, "imgsb", &loc,
					screen, &dest, arImage); break;
			case 55:
				CustomBlitSurface (imghb, "imghb", &loc,
					screen, &dest, arImage); break;
			case 56:
				CustomBlitSurface (imgdistort, "imgdistort", &loc,
					screen, &dest, arImage); break;
		}
	}

	/*** PoP select ***/
	if (iThing == -13)
	{
		switch (iModifier[0])
		{
			case 1:
				CustomBlitSurface (imgpop, "imgpop", &loc,
					screen, &dest, arImage); break;
			case 2:
				CustomBlitSurface (imgpop1dis, "imgpop1dis", &loc,
					screen, &dest, arImage); break;
			case 3:
				CustomBlitSurface (imgpop1off, "imgpop1off", &loc,
					screen, &dest, arImage); break;
			case 4:
				CustomBlitSurface (imgpop1on, "imgpop1on", &loc,
					screen, &dest, arImage); break;
			case 5:
				CustomBlitSurface (imgpop2dis, "imgpop2dis", &loc,
					screen, &dest, arImage); break;
			case 6:
				CustomBlitSurface (imgpop2off, "imgpop2off", &loc,
					screen, &dest, arImage); break;
			case 7:
				CustomBlitSurface (imgpop2on, "imgpop2on", &loc,
					screen, &dest, arImage); break;
			case 8:
				CustomBlitSurface (imgprprob, "imgprprob", &loc,
					screen, &dest, arImage); break;
			case 9:
				CustomBlitSurface (imgpopprob, "imgpopprob", &loc,
					screen, &dest, arImage); break;
		}
	}

	/*** PoP2 backgrounds ***/
	if (iThing == -14)
	{
		if ((iModifier[0] == 1) && (iModifier[1] == 1))
			CustomBlitSurface (backl1r1, "backl1r1", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 1) && (iModifier[1] == 2))
			CustomBlitSurface (backl1r2, "backl1r2", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 1) && (iModifier[1] == 3))
			CustomBlitSurface (backl1r3, "backl1r3", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 1) && (iModifier[1] == 4))
			CustomBlitSurface (backl1r4, "backl1r4", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 1) && (iModifier[1] == 5))
			CustomBlitSurface (backl1r5, "backl1r5", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 1) && (iModifier[1] == 10))
			CustomBlitSurface (backl1r10, "backl1r10", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 1) && (iModifier[1] == 11))
			CustomBlitSurface (backl1r11, "backl1r11", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 1) && (iModifier[1] == 12))
			CustomBlitSurface (backl1r12, "backl1r12", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 1) && (iModifier[1] == 15))
			CustomBlitSurface (backl1r15, "backl1r15", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 1) && (iModifier[1] == 16))
			CustomBlitSurface (backl1r16, "backl1r16", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 1) && (iModifier[1] == 19))
			CustomBlitSurface (backl1r19, "backl1r19", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 2) && (iModifier[1] == 1))
			CustomBlitSurface (backl2r1, "backl2r1", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 2) && (iModifier[1] == 2))
			CustomBlitSurface (backl2r2, "backl2r2", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 2) && (iModifier[1] == 3))
			CustomBlitSurface (backl2r3, "backl2r3", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 14) && (iModifier[1] == 1))
			CustomBlitSurface (backl14r1, "backl14r1", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 14) && (iModifier[1] == 2))
			CustomBlitSurface (backl14r2, "backl14r2", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 14) && (iModifier[1] == 3))
			CustomBlitSurface (backl14r3, "backl14r3", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 14) && (iModifier[1] == 4))
			CustomBlitSurface (backl14r4, "backl14r4", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 14) && (iModifier[1] == 5))
			CustomBlitSurface (backl14r5, "backl14r5", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 14) && (iModifier[1] == 6))
			CustomBlitSurface (backl14r6, "backl14r6", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 14) && (iModifier[1] == 7))
			CustomBlitSurface (backl14r7, "backl14r7", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 14) && (iModifier[1] == 8))
			CustomBlitSurface (backl14r8, "backl14r8", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 6) && (iModifier[1] == 27))
			CustomBlitSurface (backl6r27, "backl6r27", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 9) && (iModifier[1] == 2))
			CustomBlitSurface (backl9r2, "backl9r2", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 9) && (iModifier[1] == 11))
			CustomBlitSurface (backl9r11, "backl9r11", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 9) && (iModifier[1] == 12))
			CustomBlitSurface (backl9r12, "backl9r12", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 9) && (iModifier[1] == 13))
			CustomBlitSurface (backl9r13, "backl9r13", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 9) && (iModifier[1] == 14))
			CustomBlitSurface (backl9r14, "backl9r14", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 9) && (iModifier[1] == 15))
			CustomBlitSurface (backl9r15, "backl9r15", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 9) && (iModifier[1] == 16))
			CustomBlitSurface (backl9r16, "backl9r16", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 13) && (iModifier[1] == 4))
			CustomBlitSurface (backl13r4, "backl13r4", &loc, screen, &dest, arImage);
		if ((iModifier[0] == 8) && (iModifier[1] == 9))
			CustomBlitSurface (backl8r9, "backl8r9", &loc, screen, &dest, arImage);
	}

	/*** details ***/
	if (iThing == -15)
	{
		/*** For cracks and symbols in the ruins. ***/
		if ((iModifier[2] == -54) && (iModifier[3] == -38))
		{
			dest.x-=54;
			dest.y-=38;
		}

		/*** stalactites ***/
		if ((iModifier[0] == 0) && (iModifier[1] == 8))
			ShowDetail (stalactite8, loc, screen, dest, iLocation);
		if ((iModifier[0] == 0) && (iModifier[1] == 16))
			ShowDetail (stalactite16, loc, screen, dest, iLocation);
		if ((iModifier[0] == 0) && (iModifier[1] == 24))
			ShowDetail (stalactite24, loc, screen, dest, iLocation);
		if ((iModifier[0] == 0) && (iModifier[1] == 32))
			ShowDetail (stalactite32, loc, screen, dest, iLocation);
		if ((iModifier[0] == 0) && (iModifier[1] == 40))
			ShowDetail (stalactite40, loc, screen, dest, iLocation);
		if ((iModifier[0] == 0) && (iModifier[1] == 48))
			ShowDetail (stalactite48, loc, screen, dest, iLocation);
		if ((iModifier[0] == 0) && (iModifier[1] == 56))
			ShowDetail (stalactite56, loc, screen, dest, iLocation);

		/*** bottoms ***/
		if ((iModifier[0] == 1) && (iModifier[1] == 1))
			ShowDetail (bottom1, loc, screen, dest, iLocation);
		if ((iModifier[0] == 1) && (iModifier[1] == 2))
			ShowDetail (bottom2, loc, screen, dest, iLocation);
		if ((iModifier[0] == 1) && (iModifier[1] == 3))
			ShowDetail (bottom3, loc, screen, dest, iLocation);
		if ((iModifier[0] == 1) && (iModifier[1] == 4))
			ShowDetail (bottom4, loc, screen, dest, iLocation);
		if ((iModifier[0] == 1) && (iModifier[1] == 5))
			ShowDetail (bottom5, loc, screen, dest, iLocation);
		if ((iModifier[0] == 1) && (iModifier[1] == 6))
			ShowDetail (bottom6, loc, screen, dest, iLocation);
		if ((iModifier[0] == 1) && (iModifier[1] == 7))
			ShowDetail (bottom7, loc, screen, dest, iLocation);

		/*** cracks ***/
		if ((iModifier[0] == 2) && (iModifier[1] == 1))
			ShowDetail (crack1, loc, screen, dest, iLocation);
		if ((iModifier[0] == 2) && (iModifier[1] == 3))
			ShowDetail (crack3, loc, screen, dest, iLocation);
		if ((iModifier[0] == 2) && (iModifier[1] == 4))
			ShowDetail (crack4, loc, screen, dest, iLocation);
		if ((iModifier[0] == 2) && (iModifier[1] == 5))
			ShowDetail (crack5, loc, screen, dest, iLocation);
		if ((iModifier[0] == 2) && (iModifier[1] == 6))
			ShowDetail (crack6, loc, screen, dest, iLocation);
		if ((iModifier[0] == 2) && (iModifier[1] == 7))
			ShowDetail (crack7, loc, screen, dest, iLocation);
		if ((iModifier[0] == 2) && (iModifier[1] == 8))
			ShowDetail (crack8, loc, screen, dest, iLocation);
		if ((iModifier[0] == 2) && (iModifier[1] == 9))
			ShowDetail (crack9, loc, screen, dest, iLocation);
		if ((iModifier[0] == 2) && (iModifier[1] == 10))
			ShowDetail (crack10, loc, screen, dest, iLocation);
		if ((iModifier[0] == 2) && (iModifier[1] == 11))
			ShowDetail (crack11, loc, screen, dest, iLocation);
		if ((iModifier[0] == 2) && (iModifier[1] == 12))
			ShowDetail (crack12, loc, screen, dest, iLocation);

		/*** symbols ***/
		if ((iModifier[0] == 3) && (iModifier[1] == 16))
			ShowDetail (symbol16, loc, screen, dest, iLocation);
		if ((iModifier[0] == 3) && (iModifier[1] == 32))
			ShowDetail (symbol32, loc, screen, dest, iLocation);
		if ((iModifier[0] == 3) && (iModifier[1] == 48))
			ShowDetail (symbol48, loc, screen, dest, iLocation);
	}

	/*** foreground elements ***/
	if (iThing == -16)
	{
		switch (iModifier[0])
		{
			case 1: /*** horse ***/
				CustomBlitSurface (imghorse, "imghorse", &loc,
					screen, &dest, arImage); break;
			case 2: /*** arrow ***/
				CustomBlitSurface (imgarrow, "imgarrow", &loc,
					screen, &dest, arImage); break;
			case 3: /*** invisible ***/
				CustomBlitSurface (imginvisible, "imginvisible", &loc,
					screen, &dest, arImage); break;
		}
	}

	/*** music note and buttons ***/
	if (iThing == -17)
	{
		switch (iModifier[0])
		{
			case 0:
				CustomBlitSurface (imgmusic_note, "imgmusic_note", &loc,
					screen, &dest, arImage); break;
			case 1:
				CustomBlitSurface (imgm_all_0, "imgm_all_0", &loc,
					screen, &dest, arImage); break;
			case 2:
				CustomBlitSurface (imgm_all_1, "imgm_all_1", &loc,
					screen, &dest, arImage); break;
			case 3:
				CustomBlitSurface (imgm_none_0, "imgm_none_0", &loc,
					screen, &dest, arImage); break;
			case 4:
				CustomBlitSurface (imgm_none_1, "imgm_none_1", &loc,
					screen, &dest, arImage); break;
			case 5:
				CustomBlitSurface (imgm_left_0, "imgm_left_0", &loc,
					screen, &dest, arImage); break;
			case 6:
				CustomBlitSurface (imgm_left_1, "imgm_left_1", &loc,
					screen, &dest, arImage); break;
			case 7:
				CustomBlitSurface (imgm_right_0, "imgm_right_0", &loc,
					screen, &dest, arImage); break;
			case 8:
				CustomBlitSurface (imgm_right_1, "imgm_right_1", &loc,
					screen, &dest, arImage); break;
			case 9:
				CustomBlitSurface (imgm_top_0, "imgm_top_0", &loc,
					screen, &dest, arImage); break;
			case 10:
				CustomBlitSurface (imgm_top_1, "imgm_top_1", &loc,
					screen, &dest, arImage); break;
			case 11:
				CustomBlitSurface (imgm_middle_0, "imgm_middle_0", &loc,
					screen, &dest, arImage); break;
			case 12:
				CustomBlitSurface (imgm_middle_1, "imgm_middle_1", &loc,
					screen, &dest, arImage); break;
			case 13:
				CustomBlitSurface (imgm_bottom_0, "imgm_bottom_0", &loc,
					screen, &dest, arImage); break;
			case 14:
				CustomBlitSurface (imgm_bottom_1, "imgm_bottom_1", &loc,
					screen, &dest, arImage); break;
			case 15:
				CustomBlitSurface (imgm_invert_0, "imgm_invert_0", &loc,
					screen, &dest, arImage); break;
			case 16:
				CustomBlitSurface (imgm_invert_1, "imgm_invert_1", &loc,
					screen, &dest, arImage); break;
		}
	}

	/*** arrows ***/
	if (iThing == -1)
	{
		switch (iModifier[0])
		{
			case 1:
				if (iDownAt == 1)
				{
					CustomBlitSurface (imgleft_1, "imgleft_1", &loc,
						screen, &dest, arImage);
				} else {
					CustomBlitSurface (imgleft_0, "imgleft_0", &loc,
						screen, &dest, arImage);
				}
				break;
			case 2:
				if (iDownAt == 2)
				{
					CustomBlitSurface (imgright_1, "imgright_1", &loc,
						screen, &dest, arImage);
				} else {
					CustomBlitSurface (imgright_0, "imgright_0", &loc,
						screen, &dest, arImage);
				}
				break;
			case 3:
				if (iDownAt == 3)
				{
					CustomBlitSurface (imgup_1, "imgup_1", &loc, screen, &dest, arImage);
				} else {
					CustomBlitSurface (imgup_0, "imgup_0", &loc, screen, &dest, arImage);
				}
				break;
			case 4:
				if (iDownAt == 4)
				{
					CustomBlitSurface (imgdown_1, "imgdown_1", &loc,
						screen, &dest, arImage);
				} else {
					CustomBlitSurface (imgdown_0, "imgdown_0", &loc,
						screen, &dest, arImage);
				}
				break;
			case 5:
				CustomBlitSurface (imgudno, "imgudno", &loc,
					screen, &dest, arImage); break;
			case 6:
				CustomBlitSurface (imglrno, "imglrno", &loc,
					screen, &dest, arImage); break;
			case 7:
				CustomBlitSurface (imgudnonfo, "imgudnonfo", &loc,
					screen, &dest, arImage); break;
		}
	}

	/*** buttons ***/
	if (iThing == -2)
	{
		switch (iModifier[0])
		{
			case 1: /*** rooms on ***/
				if (iDownAt == 5)
				{
					CustomBlitSurface (imgroomson_1, "imgroomson_1", &loc,
						screen, &dest, arImage);
				} else {
					CustomBlitSurface (imgroomson_0, "imgroomson_0", &loc,
						screen, &dest, arImage);
				}
				break;
			case 8: /*** rooms broken on ***/
				if (iDownAt == 11)
				{
					CustomBlitSurface (imgbroomson_1, "imgbroomson_1", &loc,
						screen, &dest, arImage);
				} else {
					CustomBlitSurface (imgbroomson_0, "imgbroomson_0", &loc,
						screen, &dest, arImage);
				}
				break;
			case 6: /*** rooms off ***/
				CustomBlitSurface (imgroomsoff, "imgroomsoff", &loc,
					screen, &dest, arImage); break;
			case 9: /*** rooms broken off ***/
				CustomBlitSurface (imgbroomsoff, "imgbroomsoff", &loc,
					screen, &dest, arImage); break;
			case 2: /*** events on ***/
				if (iDownAt == 6)
				{
					CustomBlitSurface (imgeventson_1, "imgeventson_1", &loc,
						screen, &dest, arImage);
				} else {
					CustomBlitSurface (imgeventson_0, "imgeventson_0", &loc,
						screen, &dest, arImage);
				}
				break;
			case 7: /*** events off ***/
				CustomBlitSurface (imgeventsoff, "imgeventsoff", &loc,
					screen, &dest, arImage); break;
			case 3: /*** save on ***/
				if (iDownAt == 7)
				{
					CustomBlitSurface (imgsaveon_1, "imgsaveon_1", &loc,
						screen, &dest, arImage);
				} else {
					CustomBlitSurface (imgsaveon_0, "imgsaveon_0", &loc,
						screen, &dest, arImage);
				}
				break;
			case 4: /*** quit ***/
				if (iDownAt == 8)
				{
					CustomBlitSurface (imgquit_1, "imgquit_1", &loc,
						screen, &dest, arImage);
				} else {
					CustomBlitSurface (imgquit_0, "imgquit_0", &loc,
						screen, &dest, arImage);
				}
				break;
			case 5: /*** save off ***/
				CustomBlitSurface (imgsaveoff, "imgsaveoff", &loc,
					screen, &dest, arImage); break;
		}
	}

	/*** previous and next ***/
	if (iThing == -5)
	{
		switch (iModifier[0])
		{
			case 1:
				if (iDownAt == 9)
				{
					CustomBlitSurface (imgprevon_1, "imgprevon_1", &loc,
						screen, &dest, arImage);
				} else {
					CustomBlitSurface (imgprevon_0, "imgprevon_0", &loc,
						screen, &dest, arImage);
				}
				break;
			case 2:
				if (iDownAt == 10)
				{
					CustomBlitSurface (imgnexton_1, "imgnexton_1", &loc,
						screen, &dest, arImage);
				} else {
					CustomBlitSurface (imgnexton_0, "imgnexton_0", &loc,
						screen, &dest, arImage);
				}
				break;
			case 3:
				CustomBlitSurface (imgbar, "imgbar", &loc,
					screen, &dest, arImage); break;
			case 4:
				CustomBlitSurface (imgprevoff, "imgprevoff", &loc,
					screen, &dest, arImage); break;
			case 5:
				CustomBlitSurface (imgnextoff, "imgnextoff", &loc,
					screen, &dest, arImage); break;
			case 10:
				CustomBlitSurface (extras_0, "extras_0", &loc,
					screen, &dest, arImage); break;
			case 11:
				CustomBlitSurface (extras_1, "extras_1", &loc,
					screen, &dest, arImage); break;
			case 12:
				CustomBlitSurface (extras_2, "extras_2", &loc,
					screen, &dest, arImage); break;
			case 13:
				CustomBlitSurface (extras_3, "extras_3", &loc,
					screen, &dest, arImage); break;
			case 14:
				CustomBlitSurface (extras_4, "extras_4", &loc,
					screen, &dest, arImage); break;
			case 15:
				CustomBlitSurface (extras_5, "extras_5", &loc,
					screen, &dest, arImage); break;
			case 16:
				CustomBlitSurface (extras_6, "extras_6", &loc,
					screen, &dest, arImage); break;
			case 17:
				CustomBlitSurface (extras_7, "extras_7", &loc,
					screen, &dest, arImage); break;
			case 18:
				CustomBlitSurface (extras_8, "extras_8", &loc,
					screen, &dest, arImage); break;
			case 19:
				CustomBlitSurface (imgl10warn, "imgl10warn", &loc,
					screen, &dest, arImage); break;
		}
	}

	/*** positions ***/
	if (iThing >= 0)
	{
		iPosShown = 0;

		if (iEditPoP == 1)
		{
			if (((iThing == 0) || (iThing == 32)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd0_0, imgp0_0, "");
			if (((iThing == 0) || (iThing == 32)) && (iModifier[0] == 1))
				ShowPos (loc, dest, iLocation, imgd0_1, imgp0_1, "");
			if (((iThing == 0) || (iThing == 32)) && (iModifier[0] == 2))
				ShowPos (loc, dest, iLocation, imgd0_2, imgp0_2, "");
			if (((iThing == 0) || (iThing == 32)) && (iModifier[0] == 3))
				ShowPos (loc, dest, iLocation, imgd0_3, imgp0_3, "");
			if (((iThing == 0) || (iThing == 32)) && (iModifier[0] == 255))
				ShowPos (loc, dest, iLocation, imgd0_255, imgp0_255, "");
			if (((iThing == 1) || (iThing == 33)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd1_0, imgp1_0, "");
			if (((iThing == 1) || (iThing == 33)) && (iModifier[0] == 1))
				ShowPos (loc, dest, iLocation, imgd1_1, imgp1_1, "");
			if (((iThing == 1) || (iThing == 33)) && (iModifier[0] == 2))
				ShowPos (loc, dest, iLocation, imgd1_2, imgp1_2, "");
			if (((iThing == 1) || (iThing == 33)) && (iModifier[0] == 3))
				ShowPos (loc, dest, iLocation, imgd1_3, imgp1_3, "");
			if (((iThing == 1) || (iThing == 33)) && (iModifier[0] == 255))
				ShowPos (loc, dest, iLocation, imgd1_255, imgp1_255, "");
			if (((iThing == 2) || (iThing == 34)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd2_0, imgp2_0, "");
			if (((iThing == 2) || (iThing == 34)) && (iModifier[0] == 1))
				ShowPos (loc, dest, iLocation, imgd2_1, imgp2_1, "");
			if (((iThing == 2) || (iThing == 34)) && (iModifier[0] == 2))
				ShowPos (loc, dest, iLocation, imgd2_2, imgp2_2, "");
			if (((iThing == 2) || (iThing == 34)) && (iModifier[0] == 3))
				ShowPos (loc, dest, iLocation, imgd2_3, imgp2_3, "");
			if (((iThing == 2) || (iThing == 34)) && (iModifier[0] == 4))
				ShowPos (loc, dest, iLocation, imgd2_4, imgp2_4, "");
			if (((iThing == 2) || (iThing == 34)) && (iModifier[0] == 5))
				ShowPos (loc, dest, iLocation, imgd2_5, imgp2_5, "");
			if (((iThing == 2) || (iThing == 34)) && (iModifier[0] == 6))
				ShowPos (loc, dest, iLocation, imgd2_6, imgp2_6, "");
			if (((iThing == 2) || (iThing == 34)) && (iModifier[0] == 7))
				ShowPos (loc, dest, iLocation, imgd2_7, imgp2_7, "");
			if (((iThing == 2) || (iThing == 34)) && (iModifier[0] == 8))
				ShowPos (loc, dest, iLocation, imgd2_8, imgp2_8, "");
			if (((iThing == 2) || (iThing == 34)) && (iModifier[0] == 9))
				ShowPos (loc, dest, iLocation, imgd2_9, imgp2_9, "");
			if (((iThing == 3) || (iThing == 35)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd3_0, imgp3_0, "");
			if (((iThing == 4) || (iThing == 36)) && (iModifier[0] == 1))
				ShowPos (loc, dest, iLocation, imgd4_1, imgp4_1, "");
			/*** 0 = default for level editors, 2 = default of the game ***/
			if (((iThing == 4) || (iThing == 36)) &&
				((iModifier[0] == 2) || (iModifier[0] == 0)))
				ShowPos (loc, dest, iLocation, imgd4_0, imgp4_0, "");
			if (((iThing == 5) || (iThing == 37)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd5_0, imgp5_0, "");
			if ((iThing == 6) || (iThing == 38)) /*** drop button ***/
			{
				snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
				ShowPos (loc, dest, iLocation, imgd6, imgp6, sText);
			}
			if (((iThing == 7) || (iThing == 39)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd7_0, imgp7_0, "");
			if (((iThing == 7) || (iThing == 39)) && (iModifier[0] == 1))
				ShowPos (loc, dest, iLocation, imgd7_1, imgp7_1, "");
			if (((iThing == 7) || (iThing == 39)) && (iModifier[0] == 2))
				ShowPos (loc, dest, iLocation, imgd7_2, imgp7_2, "");
			if (((iThing == 7) || (iThing == 39)) && (iModifier[0] == 3))
				ShowPos (loc, dest, iLocation, imgd7_3, imgp7_3, "");
			if (((iThing == 8) || (iThing == 40)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd8_0, imgp8_0, "");
			if (((iThing == 9) || (iThing == 41)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd9_0, imgp9_0, "");
			if (((iThing == 10) || (iThing == 42)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd10_0, imgp10_0, "");
			if (((iThing == 10) || (iThing == 42)) && (iModifier[0] == 1))
				ShowPos (loc, dest, iLocation, imgd10_1, imgp10_1, "");
			if (((iThing == 10) || (iThing == 42)) && (iModifier[0] == 2))
				ShowPos (loc, dest, iLocation, imgd10_2, imgp10_2, "");
			if (((iThing == 10) || (iThing == 42)) && (iModifier[0] == 3))
				ShowPos (loc, dest, iLocation, imgd10_3, imgp10_3, "");
			if (((iThing == 10) || (iThing == 42)) && (iModifier[0] == 4))
				ShowPos (loc, dest, iLocation, imgd10_4, imgp10_4, "");
			if (((iThing == 10) || (iThing == 42)) && (iModifier[0] == 5))
				ShowPos (loc, dest, iLocation, imgd10_5, imgp10_5, "");
			if (((iThing == 10) || (iThing == 42)) && (iModifier[0] == 6))
				ShowPos (loc, dest, iLocation, imgd10_6, imgp10_6, "");
			if ((iThing == 11) && (iModifier[0] == 0)) /*** loose tile ***/
				ShowPos (loc, dest, iLocation, imgd11_0, imgp11_0, "");
			if (((iThing == 12) || (iThing == 44)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd12_0, imgp12_0, "");
			if (((iThing == 12) || (iThing == 44)) && (iModifier[0] == 1))
				ShowPos (loc, dest, iLocation, imgd12_1, imgp12_1, "");
			if (((iThing == 12) || (iThing == 44)) && (iModifier[0] == 2))
				ShowPos (loc, dest, iLocation, imgd12_2, imgp12_2, "");
			if (((iThing == 12) || (iThing == 44)) && (iModifier[0] == 3))
				ShowPos (loc, dest, iLocation, imgd12_3, imgp12_3, "");
			if (((iThing == 12) || (iThing == 44)) && (iModifier[0] == 4))
				ShowPos (loc, dest, iLocation, imgd12_4, imgp12_4, "");
			if (((iThing == 12) || (iThing == 44)) && (iModifier[0] == 5))
				ShowPos (loc, dest, iLocation, imgd12_5, imgp12_5, "");
			if (((iThing == 12) || (iThing == 44)) && (iModifier[0] == 6))
				ShowPos (loc, dest, iLocation, imgd12_6, imgp12_6, "");
			if (((iThing == 12) || (iThing == 44)) && (iModifier[0] == 7))
				ShowPos (loc, dest, iLocation, imgd12_3, imgp12_7, "");
			if (((iThing == 13) || (iThing == 45)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd13_0, imgp13_0, "");
			if (((iThing == 14) || (iThing == 46)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd14_0, imgp14_0, "");
			if ((iThing == 15) || (iThing == 47)) /*** raise button ***/
			{
				snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
				ShowPos (loc, dest, iLocation, imgd15, imgp15, sText);
			}
			if (((iThing == 16) || (iThing == 48)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd16_0, imgp16_0, "");
			if (((iThing == 17) || (iThing == 49)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd17_0, imgp17_0, "");
			if (((iThing == 18) || (iThing == 50)) && (iModifier[0] == 0))
			{
				if (iNoAnim == 0)
				{
					switch (iChomperFrameDP)
					{
						case 10: iUseSprite = 1; break;
						case 11:
							iUseSprite = 2;
							if (iChomped == 0)
							{
								if ((iNoChomp != 1) && (iLocation >= 1))
								{
									PlaySound ("wav/chomper.wav");
									iChomped = 1;
								}
							}
							break;
						case 12: iUseSprite = 3; iChomped = 0; break;
						case 13: iUseSprite = 4; break;
						default: iUseSprite = 0; break; /*** 1-9 ***/
					}
					loc.x = ((iUseSprite * 120) * iScale);
					if (cCurType == 'd')
					{
						CustomBlitSurface (spritechomperd, "spritechomperd", &loc,
							screen, &dest, arImage);
					} else {
						CustomBlitSurface (spritechomperp, "spritechomperp", &loc,
							screen, &dest, arImage);
					}
					if (iSelected == iLocation)
					{
						if (cCurType == 'd')
						{
							CustomBlitSurface (spritechomperdsel, "spritechomperdsel", &loc,
								screen, &dest, arImage);
						} else {
							CustomBlitSurface (spritechomperpsel, "spritechomperpsel", &loc,
								screen, &dest, arImage);
						}
					}
					iPosShown = 1;
				} else {
					ShowPos (loc, dest, iLocation, imgd18_0, imgp18_0, "");
				}
			}
			if (((iThing == 18) || (iThing == 50)) && (iModifier[0] == 1))
				ShowPos (loc, dest, iLocation, imgd18_1, imgp18_1, "");
			if (((iThing == 18) || (iThing == 50)) && (iModifier[0] == 2))
				ShowPos (loc, dest, iLocation, imgd18_2, imgp18_2, "");
			if (((iThing == 18) || (iThing == 50)) && (iModifier[0] == 3))
				ShowPos (loc, dest, iLocation, imgd18_3, imgp18_3, "");
			if (((iThing == 18) || (iThing == 50)) && (iModifier[0] == 4))
				ShowPos (loc, dest, iLocation, imgd18_4, imgp18_4, "");
			if (((iThing == 18) || (iThing == 50)) && (iModifier[0] == 5))
				ShowPos (loc, dest, iLocation, imgd18_5, imgp18_5, "");
			if (((iThing == 19) || (iThing == 51)) && (iModifier[0] == 0))
			{
				if (iNoAnim == 0)
				{
					loc.x = (((iFlameFrameDP - 1) * 120) * iScale);
					if (cCurType == 'd')
					{
						CustomBlitSurface (spriteflamed1, "spriteflamed1", &loc,
							screen, &dest, arImage);
					} else {
						CustomBlitSurface (spriteflamep1, "spriteflamep1", &loc,
							screen, &dest, arImage);
					}
					if (iSelected == iLocation)
					{
						loc.x = iFromImageX * iScale;
						if (cCurType == 'd')
						{
							CustomBlitSurface (imgd19_0[2], "imgd19_0[2]", &loc,
								screen, &dest, arImage);
						} else {
							CustomBlitSurface (imgp19_0[2], "imgp19_0[2]", &loc,
								screen, &dest, arImage);
						}
					}
					iPosShown = 1;
				} else {
					ShowPos (loc, dest, iLocation, imgd19_0, imgp19_0, "");
				}
			}
			if (((iThing == 20) || (iThing == 52)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd20_0, imgp20_0, "");
			if (((iThing == 20) || (iThing == 52)) && (iModifier[0] == 1))
				ShowPos (loc, dest, iLocation, imgd20_1, imgp20_1, "");
			if (((iThing == 21) || (iThing == 53)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd21_0, imgp21_0, "");
			if (((iThing == 22) || (iThing == 54)) && (iModifier[0] == 0))
			{
				if (iNoAnim == 0)
				{
					switch (iSwordFrameDP)
					{
						case 48: iUseSprite = 1; break;
						default: iUseSprite = 0; break; /*** 1-47 ***/
					}
					loc.x = ((iUseSprite * 120) * iScale);
					if (cCurType == 'd')
					{
						CustomBlitSurface (spriteswordd, "spriteswordd", &loc,
							screen, &dest, arImage);
					} else {
						CustomBlitSurface (spriteswordp, "spriteswordp", &loc,
							screen, &dest, arImage);
					}
					if (iSelected == iLocation)
					{
						if (cCurType == 'd')
						{
							CustomBlitSurface (spritesworddsel, "spritesworddsel", &loc,
								screen, &dest, arImage);
						} else {
							CustomBlitSurface (spriteswordpsel, "spriteswordpsel", &loc,
								screen, &dest, arImage);
						}
					}
					iPosShown = 1;
				} else {
					ShowPos (loc, dest, iLocation, imgd22_0, imgp22_0, "");
				}
			}
			if (((iThing == 23) || (iThing == 55)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd23_0, imgp23_0, "");
			if (((iThing == 24) || (iThing == 56)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd24_0, imgp24_0, "");
			if (((iThing == 25) || (iThing == 57)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd25_0, imgp25_0, "");
			if (((iThing == 26) || (iThing == 58)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd26_0, imgp26_0, "");
			if (((iThing == 27) || (iThing == 59)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd27_0, imgp27_0, "");
			if (((iThing == 28) || (iThing == 60)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd28_0, imgp28_0, "");
			if (((iThing == 29) || (iThing == 61)) && (iModifier[0] == 0))
				ShowPos (loc, dest, iLocation, imgd29_0, imgp29_0, "");
			if (((iThing == 30) || (iThing == 62)) && (iModifier[0] == 0))
			{
				if (iNoAnim == 0)
				{
					loc.x = (((iFlameFrameDP - 1) * 120) * iScale);
					if (cCurType == 'd')
					{
						CustomBlitSurface (spriteflamed2, "spriteflamed2", &loc,
							screen, &dest, arImage);
					} else {
						CustomBlitSurface (spriteflamep2, "spriteflamep2", &loc,
							screen, &dest, arImage);
					}
					if (iSelected == iLocation)
					{
						loc.x = iFromImageX * iScale;
						if (cCurType == 'd')
						{
							CustomBlitSurface (imgd30_0[2], "imgd30_0[2]", &loc,
								screen, &dest, arImage);
						} else {
							CustomBlitSurface (imgp30_0[2], "imgp30_0[2]", &loc,
								screen, &dest, arImage);
						}
					}
					iPosShown = 1;
				} else {
					ShowPos (loc, dest, iLocation, imgd30_0, imgp30_0, "");
				}
			}
			if ((iThing == 43) && (iModifier[0] == 0)) /*** loose tile, stuck ***/
				ShowPos (loc, dest, iLocation, imgd43_0, imgp43_0, "");
		}

		if (iEditPoP == 2)
		{
			switch (cCurType)
			{
				case 'o':
					if (iThing == 0)
						ShowPosPoP2 (loc, dest, iLocation, imgo0, "");
					if (iThing == 1)
						ShowPosPoP2 (loc, dest, iLocation, imgo1, "");
					if (iThing == 2)
						ShowPosPoP2 (loc, dest, iLocation, imgo2, "");
					if (iThing == 3)
						ShowPosPoP2 (loc, dest, iLocation, imgo3, "");
					if (iThing == 4)
					{
						snprintf (sText, MAX_TEXT, "%i%%", ModAsPercent (iModifier[0], 4));
						ShowPosPoP2 (loc, dest, iLocation, imgo4, sText);
					}
					if (iThing == 5) /*** raise button ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgo5, sText);
					}
					if (iThing == 6) /*** drop button ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgo6, sText);
					}
					if ((iThing == 7) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgo7_0, "");
					if ((iThing == 7) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgo7_1, "");
					if ((iThing == 7) && (iModifier[0] == 2))
						ShowPosPoP2 (loc, dest, iLocation, imgo7_2, "");
					if ((iThing == 7) && (iModifier[0] == 3))
						ShowPosPoP2 (loc, dest, iLocation, imgo7_3, "");
					if (iThing == 8)
						ShowPosPoP2 (loc, dest, iLocation, imgo8, "");
					if (iThing == 9)
						ShowPosPoP2 (loc, dest, iLocation, imgo9, "");
					if ((iThing == 10) && (iModifier[0] >= 0) && (iModifier[0] <= 31))
						ShowPosPoP2 (loc, dest, iLocation, imgo10_0, "");
					if ((iThing == 10) && (iModifier[0] >= 32) && (iModifier[0] <= 63))
						ShowPosPoP2 (loc, dest, iLocation, imgo10_32, "");
					if ((iThing == 10) && (iModifier[0] >= 64) && (iModifier[0] <= 95))
						ShowPosPoP2 (loc, dest, iLocation, imgo10_64, "");
					if ((iThing == 10) && (iModifier[0] >= 96) && (iModifier[0] <= 127))
						ShowPosPoP2 (loc, dest, iLocation, imgo10_96, "");
					if ((iThing == 10) && (iModifier[0] >= 128) && (iModifier[0] <= 159))
						ShowPosPoP2 (loc, dest, iLocation, imgo10_128, "");
					if ((iThing == 10) && (iModifier[0] >= 160) && (iModifier[0] <= 191))
						ShowPosPoP2 (loc, dest, iLocation, imgo10_160, "");
					if ((iThing == 10) && (iModifier[0] >= 192) && (iModifier[0] <= 223))
						ShowPosPoP2 (loc, dest, iLocation, imgo10_192, "");
					if ((iThing == 10) && (iModifier[0] >= 224) && (iModifier[0] <= 255))
						ShowPosPoP2 (loc, dest, iLocation, imgo10_224, "");
					if (iThing == 11)
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							snprintf (sText, MAX_TEXT, "J:%i", ModAsJumps (iModifier[0]));
						} else {
							snprintf (sText, MAX_TEXT, "J:%i|F:%i",
								ModAsJumps (iModifier[0]), ModAsFrame (iModifier[0]));
						}
						ShowPosPoP2 (loc, dest, iLocation, imgo11, sText);
					}
					if (iThing == 12)
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							ShowPosPoP2 (loc, dest, iLocation, imgo12, "");
						} else {
							snprintf (sText, MAX_TEXT, "F:%i", ModAsFrame (iModifier[0]));
							ShowPosPoP2 (loc, dest, iLocation, imgo12, sText);
						}
					}
					if (iThing == 13)
						ShowPosPoP2 (loc, dest, iLocation, imgo13, "");
					if (iThing == 14)
						ShowPosPoP2 (loc, dest, iLocation, imgo14, "");
					if (iThing == 15)
						ShowPosPoP2 (loc, dest, iLocation, imgo15, "");
					if (iThing == 16)
						ShowPosPoP2 (loc, dest, iLocation, imgo16, "");
					if (iThing == 17)
					{
						snprintf (sText, MAX_TEXT, "%i%%",
							ModAsPercent (iModifier[0], 17));
						ShowPosPoP2 (loc, dest, iLocation, imgo17, sText);
					}
					if (iThing == 18)
						ShowPosPoP2 (loc, dest, iLocation, imgo18, "");
					if (iThing == 19)
						ShowPosPoP2 (loc, dest, iLocation, imgo19, "");
					if (iThing == 20)
						ShowPosPoP2 (loc, dest, iLocation, imgo20, "");
					if (iThing == 21)
						ShowPosPoP2 (loc, dest, iLocation, imgo21, "");
					if ((iThing == 22) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgo22_0, "");
					if ((iThing == 22) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgo22_1, "");
					if (iThing == 23)
						ShowPosPoP2 (loc, dest, iLocation, imgo23, "");
					if (iThing == 24)
						ShowPosPoP2 (loc, dest, iLocation, imgo24, "");
					if (iThing == 25)
						ShowPosPoP2 (loc, dest, iLocation, imgo25, "");
					if (iThing == 26)
						ShowPosPoP2 (loc, dest, iLocation, imgo26, "");
					if (iThing == 27)
						ShowPosPoP2 (loc, dest, iLocation, imgo27, "");
					if (iThing == 28)
						ShowPosPoP2 (loc, dest, iLocation, imgo28, "");
					if (iThing == 29)
						ShowPosPoP2 (loc, dest, iLocation, imgo29, "");
					if (iThing == 30)
						ShowPosPoP2 (loc, dest, iLocation, imgo30, "");
					if (iThing == 31)
						ShowPosPoP2 (loc, dest, iLocation, imgo31, "");
					if (iThing == 32)
						ShowPosPoP2 (loc, dest, iLocation, imgo32, "");
					if (iThing == 33)
						ShowPosPoP2 (loc, dest, iLocation, imgo33, "");
					if (iThing == 34) /*** raise drop ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgo34, sText);
					}
					if (iThing == 35)
						ShowPosPoP2 (loc, dest, iLocation, imgo35, "");
					if (iThing == 36)
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							ShowPosPoP2 (loc, dest, iLocation, imgo36, "");
						} else {
							snprintf (sText, MAX_TEXT, "F:%i", ModAsFrame (iModifier[0]));
							ShowPosPoP2 (loc, dest, iLocation, imgo36, sText);
						}
					}
					if (iThing == 37)
						ShowPosPoP2 (loc, dest, iLocation, imgo37, "");
					if (iThing == 38)
						ShowPosPoP2 (loc, dest, iLocation, imgo38, "");
					if (iThing == 39)
						ShowPosPoP2 (loc, dest, iLocation, imgo39, "");
					if (iThing == 40)
						ShowPosPoP2 (loc, dest, iLocation, imgo40, "");
					if (iThing == 41)
						ShowPosPoP2 (loc, dest, iLocation, imgo41, "");
					if (iThing == 43)
						ShowPosPoP2 (loc, dest, iLocation, imgo43, "");
					if (iThing == 44)
						ShowPosPoP2 (loc, dest, iLocation, imgo44, "");
					break;
				case 'c':
					if ((iThing == 0) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgc0_0, "");
					if ((iThing == 0) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgc0_1, "");
					if ((iThing == 0) && (iModifier[0] == 2))
						ShowPosPoP2 (loc, dest, iLocation, imgc0_2, "");
					if ((iThing == 0) && (iModifier[0] == 3))
						ShowPosPoP2 (loc, dest, iLocation, imgc0_3, "");
					if ((iThing == 0) && (iModifier[0] == 4))
						ShowPosPoP2 (loc, dest, iLocation, imgc0_4, "");
					if ((iThing == 1) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgc1_0, "");
					if ((iThing == 1) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgc1_1, "");
					if ((iThing == 1) && (iModifier[0] == 2))
						ShowPosPoP2 (loc, dest, iLocation, imgc1_2, "");
					if ((iThing == 2) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgc2_0, "");
					if ((iThing == 2) && (iModifier[0] == 5))
						ShowPosPoP2 (loc, dest, iLocation, imgc2_5, "");
					if (iThing == 3)
						ShowPosPoP2 (loc, dest, iLocation, imgc3, "");
					if ((iThing == 4) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgc4_0, "");
					if ((iThing == 4) && (iModifier[0] != 0) && (iModifier[0] != 200))
					{
						snprintf (sText, MAX_TEXT, "%i%%", ModAsPercent (iModifier[0], 4));
						ShowPosPoP2 (loc, dest, iLocation, imgc4_100, sText);
					}
					if ((iThing == 4) && (iModifier[0] == 200))
						ShowPosPoP2 (loc, dest, iLocation, imgc4_200, "");
					if ((iThing == 8) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgc8_0, "");
					if ((iThing == 8) && ((iModifier[0] == 1) || (iModifier[0] == 5)))
						ShowPosPoP2 (loc, dest, iLocation, imgc8_1, "");
					if ((iThing == 8) && (iModifier[0] == 2))
						ShowPosPoP2 (loc, dest, iLocation, imgc8_2, "");
					if (iThing == 9)
						ShowPosPoP2 (loc, dest, iLocation, imgc9, "");
					if ((iThing == 10) && (iModifier[0] >= 0) && (iModifier[0] <= 31))
						ShowPosPoP2 (loc, dest, iLocation, imgc10_0, "");
					if ((iThing == 10) && (iModifier[0] >= 32) && (iModifier[0] <= 63))
						ShowPosPoP2 (loc, dest, iLocation, imgc10_32, "");
					if ((iThing == 10) && (iModifier[0] >= 64) && (iModifier[0] <= 95))
						ShowPosPoP2 (loc, dest, iLocation, imgc10_64, "");
					if ((iThing == 10) && (iModifier[0] >= 96) && (iModifier[0] <= 127))
						ShowPosPoP2 (loc, dest, iLocation, imgc10_96, "");
					if ((iThing == 10) && (iModifier[0] >= 128) && (iModifier[0] <= 159))
						ShowPosPoP2 (loc, dest, iLocation, imgc10_128, "");
					if ((iThing == 10) && (iModifier[0] >= 160) && (iModifier[0] <= 191))
						ShowPosPoP2 (loc, dest, iLocation, imgc10_160, "");
					if ((iThing == 10) && (iModifier[0] >= 192) && (iModifier[0] <= 223))
						ShowPosPoP2 (loc, dest, iLocation, imgc10_192, "");
					if ((iThing == 10) && (iModifier[0] >= 224) && (iModifier[0] <= 255))
						ShowPosPoP2 (loc, dest, iLocation, imgc10_224, "");
					if ((iThing == 11) && (iModifier[0] >= 0) && (iModifier[0] <= 127))
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							snprintf (sText, MAX_TEXT, "J:%i", ModAsJumps (iModifier[0]));
						} else {
							snprintf (sText, MAX_TEXT, "J:%i|F:%i",
								ModAsJumps (iModifier[0]), ModAsFrame (iModifier[0]));
						}
						ShowPosPoP2 (loc, dest, iLocation, imgc11_0, sText);
					}
					if ((iThing == 11) && (iModifier[0] >= 128) && (iModifier[0] <= 255))
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							snprintf (sText, MAX_TEXT, "J:%i", ModAsJumps (iModifier[0]));
						} else {
							snprintf (sText, MAX_TEXT, "J:%i|F:%i",
								ModAsJumps (iModifier[0]), ModAsFrame (iModifier[0]));
						}
						ShowPosPoP2 (loc, dest, iLocation, imgc11_128, sText);
					}
					if ((iThing == 14) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgc14_0, "");
					if ((iThing == 14) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgc14_1, "");
					if ((iThing == 14) && (iModifier[0] == 2))
						ShowPosPoP2 (loc, dest, iLocation, imgc14_2, "");
					if (iThing == 16)
						ShowPosPoP2 (loc, dest, iLocation, imgc16, "");
					if ((iThing == 17) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgc17_0, "");
					if ((iThing == 17) && (iModifier[0] != 0) && (iModifier[0] != 42))
					{
						snprintf (sText, MAX_TEXT, "%i%%",
							ModAsPercent (iModifier[0], 17));
						ShowPosPoP2 (loc, dest, iLocation, imgc17_21, sText);
					}
					if ((iThing == 17) && (iModifier[0] == 42))
						ShowPosPoP2 (loc, dest, iLocation, imgc17_42, "");
					if (iThing == 18)
						ShowPosPoP2 (loc, dest, iLocation, imgc18, "");
					if (iThing == 19)
					{
						if (iNoAnim == 0)
						{
							loc.x = (((iFlameFrameC - 1) * 116) * iScale);
							CustomBlitSurface (spriteflamec1, "spriteflamec1", &loc,
								screen, &dest, arImage);
							if (iSelected == iLocation)
							{
								loc.x = iFromImageX * iScale;
								CustomBlitSurface (imgc19[2], "imgc19[2]", &loc,
									screen, &dest, arImage);
							}
							iPosShown = 1;
						} else {
							ShowPosPoP2 (loc, dest, iLocation, imgc19, "");
						}
					}
					if ((iThing == 20) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_0, "");
					if ((iThing == 20) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_1, "");
					if ((iThing == 20) && (iModifier[0] == 2))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_2, "");
					if ((iThing == 20) && (iModifier[0] == 3))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_3, "");
					if ((iThing == 20) && (iModifier[0] == 4))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_4, "");
					if ((iThing == 20) && (iModifier[0] == 5))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_5, "");
					if ((iThing == 20) && (iModifier[0] == 6))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_6, "");
					if ((iThing == 20) && (iModifier[0] == 7))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_7, "");
					if ((iThing == 20) && (iModifier[0] == 8))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_8, "");
					if ((iThing == 20) && (iModifier[0] == 9))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_9, "");
					if ((iThing == 20) && (iModifier[0] == 10))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_10, "");
					if ((iThing == 20) && (iModifier[0] == 128))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_128, "");
					if ((iThing == 20) && (iModifier[0] == 129))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_129, "");
					if ((iThing == 20) && (iModifier[0] == 130))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_130, "");
					if ((iThing == 20) && (iModifier[0] == 131))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_131, "");
					if ((iThing == 20) && (iModifier[0] == 132))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_132, "");
					if ((iThing == 20) && (iModifier[0] == 133))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_133, "");
					if ((iThing == 20) && (iModifier[0] == 134))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_134, "");
					if ((iThing == 20) && (iModifier[0] == 135))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_135, "");
					if ((iThing == 20) && (iModifier[0] == 136))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_136, "");
					if ((iThing == 20) && (iModifier[0] == 137))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_137, "");
					if ((iThing == 20) && (iModifier[0] == 138))
						ShowPosPoP2 (loc, dest, iLocation, imgc20_138, "");
					if (iThing == 23)
						ShowPosPoP2 (loc, dest, iLocation, imgc23, "");
					if (iThing == 24)
						ShowPosPoP2 (loc, dest, iLocation, imgc24, "");
					if (iThing == 28)
						ShowPosPoP2 (loc, dest, iLocation, imgc28, "");
					if (iThing == 32)
					{
						if (iNoAnim == 0)
						{
							loc.x = (((iFlameFrameC - 1) * 116) * iScale);
							CustomBlitSurface (spriteflamec2, "spriteflamec2", &loc,
								screen, &dest, arImage);
							if (iSelected == iLocation)
							{
								loc.x = iFromImageX * iScale;
								CustomBlitSurface (imgc32[2], "imgc32[2]", &loc,
									screen, &dest, arImage);
							}
							iPosShown = 1;
						} else {
							ShowPosPoP2 (loc, dest, iLocation, imgc32, "");
						}
					}
					if (iThing == 33)
						ShowPosPoP2 (loc, dest, iLocation, imgc33, "");
					if ((iThing == 34) && (iModifier[1] % 16 >= 0) &&
						(iModifier[1] % 16 <= 7)) /*** raise drop (reg) ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgc34_E_0, sText);
					}
					if ((iThing == 34) && (iModifier[1] % 16 >= 8) &&
						(iModifier[1] % 16 <= 15)) /*** raise drop (down) ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgc34_E_8, sText);
					}
					if (iThing == 35)
						ShowPosPoP2 (loc, dest, iLocation, imgc35, "");
					if (iThing == 36)
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							ShowPosPoP2 (loc, dest, iLocation, imgc36, "");
						} else {
							snprintf (sText, MAX_TEXT, "F:%i", ModAsFrame (iModifier[0]));
							ShowPosPoP2 (loc, dest, iLocation, imgc36, sText);
						}
					}
					if (iThing == 44)
						ShowPosPoP2 (loc, dest, iLocation, imgc44, "");

					/*** stalactites ***/
					if ((iThing == 0) || (iThing == 1) || (iThing == 3) ||
						(iThing == 4) || (iThing == 8) || (iThing == 9) ||
						(iThing == 10) || (iThing == 16) || (iThing == 17) ||
						(iThing == 19) || (iThing == 23) || (iThing == 24) ||
						(iThing == 32) || (iThing == 33) || (iThing == 35) ||
						(iThing == 36))
					{
						if ((iModifier[1] >= 8) && (iModifier[1] <= 15))
						{
							ShowImage (-15, (int[]){0, 8, 0, 0}, screen,
								iLocation, 0, 0, 116, 164);
						}
						if ((iModifier[1] >= 16) && (iModifier[1] <= 23))
						{
							ShowImage (-15, (int[]){0, 16, 0, 0}, screen,
								iLocation, 0, 0, 116, 164);
						}
						if ((iModifier[1] >= 24) && (iModifier[1] <= 31))
						{
							ShowImage (-15, (int[]){0, 24, 0, 0}, screen,
								iLocation, 0, 0, 116, 164);
						}
						if ((iModifier[1] >= 32) && (iModifier[1] <= 39))
						{
							ShowImage (-15, (int[]){0, 32, 0, 0}, screen,
								iLocation, 0, 0, 116, 164);
						}
						if ((iModifier[1] >= 40) && (iModifier[1] <= 47))
						{
							ShowImage (-15, (int[]){0, 40, 0, 0}, screen,
								iLocation, 0, 0, 116, 164);
						}
						if ((iModifier[1] >= 48) && (iModifier[1] <= 55))
						{
							ShowImage (-15, (int[]){0, 48, 0, 0}, screen,
								iLocation, 0, 0, 116, 164);
						}
						if ((iModifier[1] >= 56) && (iModifier[1] <= 63))
						{
							ShowImage (-15, (int[]){0, 56, 0, 0}, screen,
								iLocation, 0, 0, 116, 164);
						}
					}

					/*** bottoms ***/
					if ((iThing == 1) || (iThing == 2) || (iThing == 3) ||
						(iThing == 4) || (iThing == 8) || (iThing == 10) ||
						(iThing == 11) || (iThing == 14) || (iThing == 16) ||
						(iThing == 17) || (iThing == 19) || (iThing == 20) ||
						(iThing == 23) || (iThing == 24) || (iThing == 32) ||
						(iThing == 34) || (iThing == 36))
					{
						switch (iModifier[1] % 8)
						{
							case 0: break; /*** no bottom ***/
							case 1:
								ShowImage (-15, (int[]){1, 1, 0, 0}, screen,
									iLocation, 0, 0, 116, 164); break;
							case 2:
								ShowImage (-15, (int[]){1, 2, 0, 0}, screen,
									iLocation, 0, 0, 116, 164); break;
							case 3:
								ShowImage (-15, (int[]){1, 3, 0, 0}, screen,
									iLocation, 0, 0, 116, 164); break;
							case 4:
								ShowImage (-15, (int[]){1, 4, 0, 0}, screen,
									iLocation, 0, 0, 116, 164); break;
							case 5:
								ShowImage (-15, (int[]){1, 5, 0, 0}, screen,
									iLocation, 0, 0, 116, 164); break;
							case 6:
								ShowImage (-15, (int[]){1, 6, 0, 0}, screen,
									iLocation, 0, 0, 116, 164); break;
							case 7:
								ShowImage (-15, (int[]){1, 7, 0, 0}, screen,
									iLocation, 0, 0, 116, 164); break;
						}
					}
					break;
				case 'r':
					if (iThing == 0)
						ShowPosPoP2 (loc, dest, iLocation, imgr0, "");
					if ((iThing == 1) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgr1_0, "");
					if ((iThing == 1) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgr1_1, "");
					if ((iThing == 1) && (iModifier[0] == 2))
						ShowPosPoP2 (loc, dest, iLocation, imgr1_2, "");
					if ((iThing == 1) && (iModifier[0] == 3))
						ShowPosPoP2 (loc, dest, iLocation, imgr1_3, "");
					if (iThing == 2)
						ShowPosPoP2 (loc, dest, iLocation, imgr2, "");
					if ((iThing == 3) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgr3_0, "");
					if ((iThing == 3) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgr3_1, "");
					if ((iThing == 3) && (iModifier[0] == 2))
						ShowPosPoP2 (loc, dest, iLocation, imgr3_2, "");
					if ((iThing == 4) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgr4_0, "");
					if ((iThing == 4) && (iModifier[0] != 0) && (iModifier[0] != 200))
					{
						snprintf (sText, MAX_TEXT, "%i%%", ModAsPercent (iModifier[0], 4));
						ShowPosPoP2 (loc, dest, iLocation, imgr4_100, sText);
					}
					if ((iThing == 4) && (iModifier[0] == 200))
						ShowPosPoP2 (loc, dest, iLocation, imgr4_200, "");
					if ((iThing == 5) && (iModifier[1] == 0)) /*** raise button ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgr5_E_0, sText);
					}
					if ((iThing == 5) && (iModifier[1] == 8)) /*** raise button ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgr5_E_8, sText);
					}
					if ((iThing == 6) && (iModifier[1] == 0)) /*** drop button ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgr6_E_0, sText);
					}
					if ((iThing == 6) && (iModifier[1] == 8)) /*** drop button ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgr6_E_8, sText);
					}
					if ((iThing == 7) && ((iModifier[0] == 0) || (iModifier[0] == 128)))
						ShowPosPoP2 (loc, dest, iLocation, imgr7_0, "");
					if ((iThing == 7) && ((iModifier[0] == 1) || (iModifier[0] == 129)))
						ShowPosPoP2 (loc, dest, iLocation, imgr7_1, "");
					if ((iThing == 7) && ((iModifier[0] == 2) || (iModifier[0] == 130)))
						ShowPosPoP2 (loc, dest, iLocation, imgr7_2, "");
					if ((iThing == 7) && ((iModifier[0] == 3) || (iModifier[0] == 131)))
						ShowPosPoP2 (loc, dest, iLocation, imgr7_3, "");
					if (iThing == 8)
						ShowPosPoP2 (loc, dest, iLocation, imgr8, "");
					if (iThing == 9)
						ShowPosPoP2 (loc, dest, iLocation, imgr9, "");
					if ((iThing == 10) && (iModifier[0] >= 0) && (iModifier[0] <= 31))
						ShowPosPoP2 (loc, dest, iLocation, imgr10_0, "");
					if ((iThing == 10) && (iModifier[0] >= 32) && (iModifier[0] <= 63))
						ShowPosPoP2 (loc, dest, iLocation, imgr10_32, "");
					if ((iThing == 10) && (iModifier[0] >= 64) && (iModifier[0] <= 95))
						ShowPosPoP2 (loc, dest, iLocation, imgr10_64, "");
					if ((iThing == 10) && (iModifier[0] >= 96) && (iModifier[0] <= 127))
						ShowPosPoP2 (loc, dest, iLocation, imgr10_96, "");
					if ((iThing == 10) && (iModifier[0] >= 128) && (iModifier[0] <= 159))
						ShowPosPoP2 (loc, dest, iLocation, imgr10_128, "");
					if ((iThing == 10) && (iModifier[0] >= 160) && (iModifier[0] <= 191))
						ShowPosPoP2 (loc, dest, iLocation, imgr10_160, "");
					if ((iThing == 10) && (iModifier[0] >= 192) && (iModifier[0] <= 223))
						ShowPosPoP2 (loc, dest, iLocation, imgr10_192, "");
					if ((iThing == 10) && (iModifier[0] >= 224) && (iModifier[0] <= 255))
						ShowPosPoP2 (loc, dest, iLocation, imgr10_224, "");
					if (iThing == 11)
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							snprintf (sText, MAX_TEXT, "J:%i", ModAsJumps (iModifier[0]));
						} else {
							snprintf (sText, MAX_TEXT, "J:%i|F:%i",
								ModAsJumps (iModifier[0]), ModAsFrame (iModifier[0]));
						}
						ShowPosPoP2 (loc, dest, iLocation, imgr11, sText);
					}
					if ((iThing == 12) && (iModifier[0] >= 0) && (iModifier[0] <= 31))
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							ShowPosPoP2 (loc, dest, iLocation, imgr12_0, "");
						} else {
							snprintf (sText, MAX_TEXT, "F:%i", ModAsFrame (iModifier[0]));
							ShowPosPoP2 (loc, dest, iLocation, imgr12_0, sText);
						}
					}
					if ((iThing == 12) && (iModifier[0] >= 32) && (iModifier[0] <= 63))
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							ShowPosPoP2 (loc, dest, iLocation, imgr12_32, "");
						} else {
							snprintf (sText, MAX_TEXT, "F:%i", ModAsFrame (iModifier[0]));
							ShowPosPoP2 (loc, dest, iLocation, imgr12_32, sText);
						}
					}
					if ((iThing == 12) && (iModifier[0] >= 128) && (iModifier[0] <= 159))
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							ShowPosPoP2 (loc, dest, iLocation, imgr12_128, "");
						} else {
							snprintf (sText, MAX_TEXT, "F:%i", ModAsFrame (iModifier[0]));
							ShowPosPoP2 (loc, dest, iLocation, imgr12_128, sText);
						}
					}
					if ((iThing == 12) && (iModifier[0] >= 160) && (iModifier[0] <= 191))
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							ShowPosPoP2 (loc, dest, iLocation, imgr12_160, "");
						} else {
							snprintf (sText, MAX_TEXT, "F:%i", ModAsFrame (iModifier[0]));
							ShowPosPoP2 (loc, dest, iLocation, imgr12_160, sText);
						}
					}
					if ((iThing == 13) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgr13_0, "");
					if ((iThing == 13) && (iModifier[0] == 32))
						ShowPosPoP2 (loc, dest, iLocation, imgr13_32, "");
					if ((iThing == 14) && (iModifier[0] == 16))
						ShowPosPoP2 (loc, dest, iLocation, imgr14_16, "");
					if ((iThing == 14) && (iModifier[0] == 32))
						ShowPosPoP2 (loc, dest, iLocation, imgr14_32, "");
					if ((iThing == 14) && (iModifier[0] == 64))
						ShowPosPoP2 (loc, dest, iLocation, imgr14_64, "");
					if ((iThing == 15) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgr15_0, "");
					if ((iThing == 15) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgr15_1, "");
					if ((iThing == 15) && (iModifier[0] == 2))
						ShowPosPoP2 (loc, dest, iLocation, imgr15_2, "");
					if ((iThing == 15) && (iModifier[0] == 3))
						ShowPosPoP2 (loc, dest, iLocation, imgr15_3, "");
					if (iThing == 16)
						ShowPosPoP2 (loc, dest, iLocation, imgr16, "");
					if ((iThing == 17) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgr17_0, "");
					if ((iThing == 17) && (iModifier[0] != 0) && (iModifier[0] != 42))
					{
						snprintf (sText, MAX_TEXT, "%i%%",
							ModAsPercent (iModifier[0], 17));
						ShowPosPoP2 (loc, dest, iLocation, imgr17_21, sText);
					}
					if ((iThing == 17) && (iModifier[0] == 42))
						ShowPosPoP2 (loc, dest, iLocation, imgr17_42, "");
					if (iThing == 18)
						ShowPosPoP2 (loc, dest, iLocation, imgr18, "");
					if ((iThing == 20) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgr20_0, "");
					if ((iThing == 20) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgr20_1, "");
					if ((iThing == 20) && (iModifier[0] == 2))
						ShowPosPoP2 (loc, dest, iLocation, imgr20_2, "");
					if ((iThing == 20) && (iModifier[0] == 4))
						ShowPosPoP2 (loc, dest, iLocation, imgr20_4, "");
					if ((iThing == 20) && (iModifier[0] == 5))
						ShowPosPoP2 (loc, dest, iLocation, imgr20_5, "");
					if ((iThing == 20) && (iModifier[0] == 6))
						ShowPosPoP2 (loc, dest, iLocation, imgr20_6, "");
					if ((iThing == 20) && (iModifier[0] == 8))
						ShowPosPoP2 (loc, dest, iLocation, imgr20_8, "");
					if ((iThing == 20) && (iModifier[0] == 9))
						ShowPosPoP2 (loc, dest, iLocation, imgr20_9, "");
					if ((iThing == 20) && (iModifier[0] == 10))
						ShowPosPoP2 (loc, dest, iLocation, imgr20_10, "");
					if ((iThing == 20) && (iModifier[0] == 12))
						ShowPosPoP2 (loc, dest, iLocation, imgr20_12, "");
					if ((iThing == 20) && (iModifier[0] == 13))
						ShowPosPoP2 (loc, dest, iLocation, imgr20_13, "");
					if ((iThing == 20) && (iModifier[0] == 14))
						ShowPosPoP2 (loc, dest, iLocation, imgr20_14, "");
					if (iThing == 21)
						ShowPosPoP2 (loc, dest, iLocation, imgr21, "");
					if ((iThing == 22) && (iModifier[0] != 2)) /*** big sword ***/
						ShowPosPoP2 (loc, dest, iLocation, imgr22_1, "");
					if ((iThing == 22) && (iModifier[0] == 2)) /*** small sword ***/
						ShowPosPoP2 (loc, dest, iLocation, imgr22_2, "");

					/*** symbols ***/
					if ((iThing == 0) || (iThing == 1) || (iThing == 3) ||
						(iThing == 4) || (iThing == 7) || (iThing == 10) ||
						(iThing == 11) || (iThing == 12) || (iThing == 13) ||
						(iThing == 14) || (iThing == 15) || (iThing == 16) ||
						(iThing == 17) || (iThing == 20) || (iThing == 21) ||
						(iThing == 22))
					{
						if ((iModifier[1] >= 16) && (iModifier[1] <= 31))
						{
							ShowImage (-15, (int[]){3, 16, -54, -38}, screen,
								iLocation, 0, 0, 182, 254);
						}
						if ((iModifier[1] >= 32) && (iModifier[1] <= 47))
						{
							ShowImage (-15, (int[]){3, 32, -54, -38}, screen,
								iLocation, 0, 0, 182, 254);
						}
						if ((iModifier[1] >= 48) && (iModifier[1] <= 63))
						{
							ShowImage (-15, (int[]){3, 48, -54, -38}, screen,
								iLocation, 0, 0, 182, 254);
						}
					}

					/*** cracks ***/
					if ((iThing == 0) || (iThing == 1) || (iThing == 3) ||
						(iThing == 4) || (iThing == 7) || (iThing == 8) ||
						(iThing == 9) || (iThing == 10) || (iThing == 11) ||
						(iThing == 12) || (iThing == 13) || (iThing == 14) ||
						(iThing == 15) || (iThing == 16) || (iThing == 20) ||
						(iThing == 21) || (iThing == 22))
					{
						switch (iModifier[1] % 16)
						{
							case 0: break; /*** no crack ***/
							case 1:
								ShowImage (-15, (int[]){2, 1, -54, -38}, screen,
									iLocation, 0, 0, 182, 254); break;
							case 2: break; /*** no crack ***/
							case 3:
								ShowImage (-15, (int[]){2, 3, -54, -38}, screen,
									iLocation, 0, 0, 182, 254); break;
							case 4:
								ShowImage (-15, (int[]){2, 4, -54, -38}, screen,
									iLocation, 0, 0, 182, 254); break;
							case 5:
								ShowImage (-15, (int[]){2, 5, -54, -38}, screen,
									iLocation, 0, 0, 182, 254); break;
							case 6:
								ShowImage (-15, (int[]){2, 6, -54, -38}, screen,
									iLocation, 0, 0, 182, 254); break;
							case 7:
								ShowImage (-15, (int[]){2, 7, -54, -38}, screen,
									iLocation, 0, 0, 182, 254); break;
							case 8:
								ShowImage (-15, (int[]){2, 8, -54, -38}, screen,
									iLocation, 0, 0, 182, 254); break;
							case 9:
								ShowImage (-15, (int[]){2, 9, -54, -38}, screen,
									iLocation, 0, 0, 182, 254); break;
							case 10:
								ShowImage (-15, (int[]){2, 10, -54, -38}, screen,
									iLocation, 0, 0, 182, 254); break;
							case 11:
								ShowImage (-15, (int[]){2, 11, -54, -38}, screen,
									iLocation, 0, 0, 182, 254); break;
							case 12:
								ShowImage (-15, (int[]){2, 12, -54, -38}, screen,
									iLocation, 0, 0, 182, 254); break;
							case 13: case 14: case 15: break; /*** no crack ***/
						}
					}

					break;
				case 't':
					if (iThing == 0)
						ShowPosPoP2 (loc, dest, iLocation, imgt0, "");
					if ((iThing == 1) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgt1_0, "");
					if ((iThing == 1) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgt1_1, "");
					if ((iThing == 2) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgt2_0, "");
					if ((iThing == 2) && (iModifier[0] == 5))
						ShowPosPoP2 (loc, dest, iLocation, imgt2_5, "");
					if ((iThing == 3) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgt3_0, "");
					if ((iThing == 3) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgt3_1, "");
					if ((iThing == 3) && (iModifier[0] == 2))
						ShowPosPoP2 (loc, dest, iLocation, imgt18, ""); /*** inv. tile ***/
					if ((iThing == 4) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgt4_0, "");
					if ((iThing == 4) && (iModifier[0] != 0) && (iModifier[0] != 200))
					{
						snprintf (sText, MAX_TEXT, "%i%%", ModAsPercent (iModifier[0], 4));
						ShowPosPoP2 (loc, dest, iLocation, imgt4_100, sText);
					}
					if ((iThing == 4) && (iModifier[0] == 200))
						ShowPosPoP2 (loc, dest, iLocation, imgt4_200, "");
					if ((iThing == 5) && (iModifier[1] == 0)) /*** raise button ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgt5_E_0, sText);
					}
					if ((iThing == 5) && (iModifier[1] == 8)) /*** raise button ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgt5_E_8, sText);
					}
					if ((iThing == 6) && (iModifier[1] == 0)) /*** drop button ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgt6_E_0, sText);
					}
					if ((iThing == 6) && (iModifier[1] == 8)) /*** drop button ***/
					{
						snprintf (sText, MAX_TEXT, "E:%i", iModifier[0] + 1);
						ShowPosPoP2 (loc, dest, iLocation, imgt6_E_8, sText);
					}
					if (iThing == 8)
						ShowPosPoP2 (loc, dest, iLocation, imgt8, "");
					if (iThing == 9)
						ShowPosPoP2 (loc, dest, iLocation, imgt9, "");
					if ((iThing == 10) && (iModifier[0] >= 0) && (iModifier[0] <= 31))
						ShowPosPoP2 (loc, dest, iLocation, imgt10_0, "");
					if ((iThing == 10) && (iModifier[0] >= 32) && (iModifier[0] <= 63))
						ShowPosPoP2 (loc, dest, iLocation, imgt10_32, "");
					if ((iThing == 10) && (iModifier[0] >= 64) && (iModifier[0] <= 95))
						ShowPosPoP2 (loc, dest, iLocation, imgt10_64, "");
					if ((iThing == 10) && (iModifier[0] >= 96) && (iModifier[0] <= 127))
						ShowPosPoP2 (loc, dest, iLocation, imgt10_96, "");
					if ((iThing == 10) && (iModifier[0] >= 128) && (iModifier[0] <= 159))
						ShowPosPoP2 (loc, dest, iLocation, imgt10_128, "");
					if ((iThing == 10) && (iModifier[0] >= 160) && (iModifier[0] <= 191))
						ShowPosPoP2 (loc, dest, iLocation, imgt10_160, "");
					if ((iThing == 10) && (iModifier[0] >= 192) && (iModifier[0] <= 223))
						ShowPosPoP2 (loc, dest, iLocation, imgt10_192, "");
					if ((iThing == 10) && (iModifier[0] >= 224) && (iModifier[0] <= 255))
						ShowPosPoP2 (loc, dest, iLocation, imgt10_224, "");
					if (iThing == 11)
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							snprintf (sText, MAX_TEXT, "J:%i", ModAsJumps (iModifier[0]));
						} else {
							snprintf (sText, MAX_TEXT, "J:%i|F:%i",
								ModAsJumps (iModifier[0]), ModAsFrame (iModifier[0]));
						}
						ShowPosPoP2 (loc, dest, iLocation, imgt11, sText);
					}
					if ((iThing == 12) && (iModifier[0] >= 0) && (iModifier[0] <= 31))
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							ShowPosPoP2 (loc, dest, iLocation, imgt12_0, "");
						} else {
							snprintf (sText, MAX_TEXT, "F:%i", ModAsFrame (iModifier[0]));
							ShowPosPoP2 (loc, dest, iLocation, imgt12_0, sText);
						}
					}
					if ((iThing == 12) && (iModifier[0] >= 32) && (iModifier[0] <= 63))
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							ShowPosPoP2 (loc, dest, iLocation, imgt12_32, "");
						} else {
							snprintf (sText, MAX_TEXT, "F:%i", ModAsFrame (iModifier[0]));
							ShowPosPoP2 (loc, dest, iLocation, imgt12_32, sText);
						}
					}
					if ((iThing == 12) && (iModifier[0] >= 128) && (iModifier[0] <= 159))
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							ShowPosPoP2 (loc, dest, iLocation, imgt12_128, "");
						} else {
							snprintf (sText, MAX_TEXT, "F:%i", ModAsFrame (iModifier[0]));
							ShowPosPoP2 (loc, dest, iLocation, imgt12_128, sText);
						}
					}
					if ((iThing == 12) && (iModifier[0] >= 160) && (iModifier[0] <= 191))
					{
						if (ModAsFrame (iModifier[0]) == 0)
						{
							ShowPosPoP2 (loc, dest, iLocation, imgt12_160, "");
						} else {
							snprintf (sText, MAX_TEXT, "F:%i", ModAsFrame (iModifier[0]));
							ShowPosPoP2 (loc, dest, iLocation, imgt12_160, sText);
						}
					}
					if ((iThing == 13) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgt13_0, "");
					if ((iThing == 13) && (iModifier[0] == 32))
						ShowPosPoP2 (loc, dest, iLocation, imgt13_32, "");
					if ((iThing == 14) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgt14_0, "");
					if ((iThing == 14) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgt14_1, "");
					if (iThing == 16)
						ShowPosPoP2 (loc, dest, iLocation, imgt16, "");
					if ((iThing == 17) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgt17_0, "");
					if ((iThing == 17) && (iModifier[0] != 0) && (iModifier[0] != 42))
					{
						snprintf (sText, MAX_TEXT, "%i%%",
							ModAsPercent (iModifier[0], 17));
						ShowPosPoP2 (loc, dest, iLocation, imgt17_21, sText);
					}
					if ((iThing == 17) && (iModifier[0] == 42))
						ShowPosPoP2 (loc, dest, iLocation, imgt17_42, "");
					if (iThing == 18)
						ShowPosPoP2 (loc, dest, iLocation, imgt18, "");
					if (iThing == 19)
					{
						if (iNoAnim == 0)
						{
							loc.x = (((iFlameFrameT - 1) * 116) * iScale);
							CustomBlitSurface (spriteflamet1, "spriteflamet1", &loc,
								screen, &dest, arImage);
							if (iSelected == iLocation)
							{
								loc.x = iFromImageX * iScale;
								CustomBlitSurface (imgt19[2], "imgt19[2]", &loc,
									screen, &dest, arImage);
							}
							iPosShown = 1;
						} else {
							ShowPosPoP2 (loc, dest, iLocation, imgt19, "");
						}
					}
					if ((iThing == 20) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgt20_0, "");
					if ((iThing == 20) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgt20_1, "");
					if ((iThing == 20) && ((iModifier[0] == 2) || (iModifier[0] == 3)))
						ShowPosPoP2 (loc, dest, iLocation, imgt20_2, "");
					if ((iThing == 25) && (iModifier[0] == 0))
						ShowPosPoP2 (loc, dest, iLocation, imgt25_0, "");
					if ((iThing == 25) && (iModifier[0] == 1))
						ShowPosPoP2 (loc, dest, iLocation, imgt25_1, "");
					if ((iThing == 25) && ((iModifier[0] == 2) || (iModifier[0] == 3)))
						ShowPosPoP2 (loc, dest, iLocation, imgt25_2, "");
					if (iThing == 26)
						ShowPosPoP2 (loc, dest, iLocation, imgt26, "");
					if (iThing == 32)
					{
						if (iNoAnim == 0)
						{
							loc.x = (((iFlameFrameT - 1) * 116) * iScale);
							CustomBlitSurface (spriteflamet2, "spriteflamet2", &loc,
								screen, &dest, arImage);
							if (iSelected == iLocation)
							{
								loc.x = iFromImageX * iScale;
								CustomBlitSurface (imgt32[2], "imgt32[2]", &loc,
									screen, &dest, arImage);
							}
							iPosShown = 1;
						} else {
							ShowPosPoP2 (loc, dest, iLocation, imgt32, "");
						}
					}
					break;
			}
		}

		if (iPosShown == 0) /*** Custom tile. ***/
		{
			ShowPos (loc, dest, iLocation, imgunk, imgunk, "");
			CustomTile (iThing, (int[]){iModifier[0], iModifier[1],
				iModifier[2], iModifier[3]}, sCustomTile);
			snprintf (sText, MAX_TEXT, "%s", sCustomTile);
			message = TTF_RenderText_Shaded (font2, sText, color_bl, color_wh);
			offset.x = dest.x;
			offset.y = dest.y + 145 - FONT_SIZE_2;
			CustomBlitSurface (message, "message", NULL, screen, &offset, arFont);
		}
	}

	if (iThing == -8)
	{
		if (iModifier[0] == 1) /*** kid ***/
		{
			if (iEditPoP == 1)
				{ dest.y = dest.y + 51; }
					else { dest.y = dest.y + 52; }
			switch (luKidDir)
			{
				case 0: /*** looks right ***/
					if (iEditPoP == 1)
					{
						dest.x = dest.x + 74;
					} else {
						dest.x = dest.x + 22;
					}
					CustomBlitSurface (imgkidr[1], "imgkidr[1]", &loc,
						screen, &dest, arImage);
					if (iSelected == iLocation)
					{ CustomBlitSurface (imgkidr[2], "imgkidr[2]", &loc,
						screen, &dest, arImage); }
					break;
				case 255: /*** looks left ***/
					if (iEditPoP == 1)
					{
						dest.x = dest.x + 36;
					} else {
						dest.x = dest.x + 36 - 24;
					}
					CustomBlitSurface (imgkidl[1], "imgkidl[1]", &loc,
						screen, &dest, arImage);
					if (iSelected == iLocation)
					{ CustomBlitSurface (imgkidl[2], "imgkidl[2]", &loc,
						screen, &dest, arImage); }
					break;
				default: printf ("[FAILED] Wrong dir.!\n"); exit (EXIT_ERROR); break;
			}
		}
		if (iModifier[0] == 2) /*** PoP1 guard ***/
		{
			iTemp = dest.x;
			iTemp2 = dest.y;
			if (((int)luLevelNr == 3) || ((int)luLevelNr == 12))
			{
				dest.y = dest.y + 63; /*** skeleton + shadow ***/
			} else if ((int)luLevelNr == 13) {
				dest.y = dest.y + 43; /*** jaffar ***/
			} else {
				dest.y = dest.y + 51; /*** normal guards + fat ***/
			}
			if (sGuardDirections[iCurRoom - 1] == 255) /*** looking to the left ***/
			{
				if (((int)luLevelNr == 3) || ((int)luLevelNr == 12))
				{
					dest.x = dest.x + 34; /*** skeleton + shadow ***/
				} else {
					dest.x = dest.x + 54; /*** normal guards + fat + jaffar ***/
				}
				switch ((int)luLevelNr)
				{
					case 0:
					case 1:
					case 2:
					case 4:
					case 5:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
						switch (sGuardColors[iCurRoom - 1] - 1)
						{
							case 0: CustomBlitSurface (imgglp[1], "imgglp[1]", &loc,
								screen, &dest, arImage); break;
							case 1: CustomBlitSurface (imggrp[1], "imggrp[1]", &loc,
								screen, &dest, arImage); break;
							case 2: CustomBlitSurface (imggoy[1], "imggoy[1]", &loc,
								screen, &dest, arImage); break;
							case 3: CustomBlitSurface (imgggy[1], "imgggy[1]", &loc,
								screen, &dest, arImage); break;
							case 4: CustomBlitSurface (imggdb[1], "imggdb[1]", &loc,
								screen, &dest, arImage); break;
							case 5: CustomBlitSurface (imggpb[1], "imggpb[1]", &loc,
								screen, &dest, arImage); break;
							case 6: CustomBlitSurface (imggyo[1], "imggyo[1]", &loc,
								screen, &dest, arImage); break;
							default:
								CustomBlitSurface (imggyo[1], "imggyo[1]", &loc,
									screen, &dest, arImage);
								if (iDebug == 1)
								{
									printf ("[ INFO ] Strange guard color: %i (jaffar, shadow, "
										"fat, skeleton?)\n", sGuardColors[iCurRoom - 1] - 1);
								}
								break;
						}
						break;
					case 3:
						CustomBlitSurface (imgskel[1], "imgskel[1]", &loc,
							screen, &dest, arImage); break;
					case 6:
						CustomBlitSurface (imgfat[1], "imgfat[1]", &loc,
							screen, &dest, arImage); break;
					case 13: CustomBlitSurface (imgjaffar[1], "imgjaffar[1]", &loc,
						screen, &dest, arImage); break;
					case 12: CustomBlitSurface (imgshadow[1], "imgshadow[1]", &loc,
						screen, &dest, arImage); break;
				}
			} else {
				if (((int)luLevelNr == 3) || ((int)luLevelNr == 12))
				{
					dest.x = dest.x + 8; /*** skeleton + shadow ***/
				} else if ((int)luLevelNr == 13) {
					dest.x = dest.x + 20; /*** jaffar ***/
				} else {
					dest.x = dest.x + 6; /*** normal guards + fat ***/
				}
				switch ((int)luLevelNr)
				{
					case 0:
					case 1:
					case 2:
					case 4:
					case 5:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
						switch (sGuardColors[iCurRoom - 1] - 1)
						{
							case 0: CustomBlitSurface (imgglp[2], "imgglp[2]", &loc,
								screen, &dest, arImage); break;
							case 1: CustomBlitSurface (imggrp[2], "imggrp[2]", &loc,
								screen, &dest, arImage); break;
							case 2: CustomBlitSurface (imggoy[2], "imggoy[2]", &loc,
								screen, &dest, arImage); break;
							case 3: CustomBlitSurface (imgggy[2], "imgggy[2]", &loc,
								screen, &dest, arImage); break;
							case 4: CustomBlitSurface (imggdb[2], "imggdb[2]", &loc,
								screen, &dest, arImage); break;
							case 5: CustomBlitSurface (imggpb[2], "imggpb[2]", &loc,
								screen, &dest, arImage); break;
							case 6: CustomBlitSurface (imggyo[2], "imggyo[2]", &loc,
								screen, &dest, arImage); break;
							default:
								CustomBlitSurface (imggyo[2], "imggyo[2]", &loc,
									screen, &dest, arImage);
								if (iDebug == 1)
								{
									printf ("[ INFO ] Strange guard color: %i (jaffar, shadow, "
										"fat, skeleton?)\n", sGuardColors[iCurRoom - 1] - 1);
								}
								break;
						}
						break;
					case 3:
						CustomBlitSurface (imgskel[2], "imgskel[2]", &loc,
							screen, &dest, arImage); break;
					case 6:
						CustomBlitSurface (imgfat[2], "imgfat[2]", &loc,
							screen, &dest, arImage); break;
					case 13: CustomBlitSurface (imgjaffar[2], "imgjaffar[2]", &loc,
						screen, &dest, arImage); break;
					case 12: CustomBlitSurface (imgshadow[2], "imgshadow[2]", &loc,
						screen, &dest, arImage); break;
				}
			}
			snprintf (sText, MAX_TEXT, "LVL:%i", sGuardSkills[iCurRoom - 1]);
			message = TTF_RenderText_Shaded (font2, sText, color_bl, color_wh);
			offset.x = iTemp + 7;
			offset.y = iTemp2 + 145 - FONT_SIZE_2;
			CustomBlitSurface (message, "message", NULL, screen, &offset, arFont);
			if (iSelected == iLocation)
			{
				/*** looking to the left ***/
				if (sGuardDirections[iCurRoom - 1] == 255)
				{
					if ((int)luLevelNr == 3)
					{ CustomBlitSurface (imgskelsel[1], "imgskelsel[1]", &loc,
						screen, &dest, arImage); }
					else if ((int)luLevelNr == 6)
					{ CustomBlitSurface (imgfatsel[1], "imgfatsel[1]", &loc,
						screen, &dest, arImage); }
					else if ((int)luLevelNr == 13)
					{ CustomBlitSurface (imgjaffarsel[1], "imgjaffarsel[1]", &loc,
						screen, &dest, arImage); }
					else if ((int)luLevelNr == 12)
					{ CustomBlitSurface (imgshadowsel[1], "imgshadowsel[1]", &loc,
						screen, &dest, arImage); }
					else if (((int)luLevelNr != 14) && ((int)luLevelNr != 15))
					{ CustomBlitSurface (imggsel[1], "imggsel[1]", &loc,
						screen, &dest, arImage); }
				} else {
					if ((int)luLevelNr == 3)
					{ CustomBlitSurface (imgskelsel[2], "imgskelsel[2]", &loc,
						screen, &dest, arImage); }
					else if ((int)luLevelNr == 6)
					{ CustomBlitSurface (imgfatsel[2], "imgfatsel[2]", &loc,
						screen, &dest, arImage); }
					else if ((int)luLevelNr == 13)
					{ CustomBlitSurface (imgjaffarsel[2], "imgjaffarsel[2]", &loc,
						screen, &dest, arImage); }
					else if ((int)luLevelNr == 12)
					{ CustomBlitSurface (imgshadowsel[2], "imgshadowsel[2]", &loc,
						screen, &dest, arImage); }
					else if (((int)luLevelNr != 14) && ((int)luLevelNr != 15))
					{ CustomBlitSurface (imggsel[2], "imggsel[2]", &loc,
						screen, &dest, arImage); }
				}
			}
		}
		if (iModifier[0] == 3) /*** incoming arrow ***/
		{
			if ((iDynamicGuards_4_Column[iCurRoom - 1][iModifier[1]] >= 0) &&
				(iDynamicGuards_4_Column[iCurRoom - 1][iModifier[1]] <= 5))
			{
				CustomBlitSurface (imgincomingl[iDynamicGuards_9_Amount[iCurRoom - 1]
					[iModifier[1]]], "imgincomingl[]", &loc, screen, &dest, arImage);
			} else {
				CustomBlitSurface (imgincomingr[iDynamicGuards_9_Amount[iCurRoom - 1]
					[iModifier[1]]], "imgincomingr[]", &loc, screen, &dest, arImage);
			}
		}
		if (iModifier[0] == 4) /*** PoP2 guard ***/
		{
			if (iStaticGuards_4_Directions[iCurRoom - 1][iModifier[1]] == 0)
			{
				iDirection = 2; /*** right ***/
			} else { /*** 255 ***/
				iDirection = 1; /*** left ***/
			}
			switch (WhichStaticGuard (iModifier[1]))
			{
				case 1:
					CustomBlitSurface (imgguardblack[iDirection],
						"imgguardblack[iDirection]",
						&loc, screen, &dest, arImage); break;
				case 2:
					CustomBlitSurface (imgguardblue[iDirection],
						"imgguardblue[iDirection]",
						&loc, screen, &dest, arImage); break;
				case 3:
					CustomBlitSurface (imgguardred[iDirection],
						"imgguardred[iDirection]",
						&loc, screen, &dest, arImage); break;
				case 4:
					CustomBlitSurface (imgskelawakes[iDirection],
						"imgskelawakes[iDirection]",
						&loc, screen, &dest, arImage); break;
				case 5:
					CustomBlitSurface (imgskelawake[iDirection],
						"imgskelawake[iDirection]",
						&loc, screen, &dest, arImage); break;
				case 6:
					CustomBlitSurface (imgskelsleeps[iDirection],
						"imgskelsleeps[iDirection]",
						&loc, screen, &dest, arImage); break;
				case 7:
					CustomBlitSurface (imgskelawakessws[iDirection],
						"imgskelawakessws[iDirection]",
						&loc, screen, &dest, arImage); break;
				case 8:
					CustomBlitSurface (imgheadlong[iDirection],
						"imgheadlong[iDirection]",
						&loc, screen, &dest, arImage); break;
				case 9:
					CustomBlitSurface (imgheadshort[iDirection],
						"imgheadshort[iDirection]",
						&loc, screen, &dest, arImage); break;
				case 10:
					CustomBlitSurface (imgsnake[iDirection],
						"imgsnake[iDirection]",
						&loc, screen, &dest, arImage); break;
				case 11:
					CustomBlitSurface (imgbirdman[iDirection],
						"imgbirdman[iDirection]",
						&loc, screen, &dest, arImage); break;
				case 12:
					CustomBlitSurface (imgsword[iDirection],
						"imgsword[iDirection]",
						&loc, screen, &dest, arImage); break;
				case 13:
					CustomBlitSurface (imgfakeprince[iDirection],
						"imgfakeprince[iDirection]",
						&loc, screen, &dest, arImage); break;
			}
			snprintf (sText, MAX_TEXT, "S:%i",
				iStaticGuards_5_Skills[iCurRoom - 1][iModifier[1]]);
			message = TTF_RenderText_Shaded (font2, sText, color_bl, color_wh);
			offset.x = dest.x + 7;
			offset.y = dest.y + iFromImageHeight - FONT_SIZE_2;
			CustomBlitSurface (message, "message", NULL, screen, &offset, arFont);
		}
	}

	if (iThing == -9)
	{
		switch (iModifier[0])
		{
			case 1: /*** faded background ***/
				CustomBlitSurface (imgfadedl, "imgfadedl", &loc,
					screen, &dest, arImage); break;
			case 2: /*** popup ***/
				CustomBlitSurface (imgpopup, "imgpopup", &loc,
					screen, &dest, arImage); break;
			case 3: /*** OK off ***/
				CustomBlitSurface (imgok[1], "imgok[1]", &loc,
					screen, &dest, arImage); break;
			case 4: /*** OK on ***/
				CustomBlitSurface (imgok[2], "imgok[2]", &loc,
					screen, &dest, arImage); break;
			case 5: /*** popup_yn ***/
				CustomBlitSurface (imgpopup_yn, "imgpopup_yn", &loc,
					screen, &dest, arImage); break;
			case 6: /*** Yes off ***/
				CustomBlitSurface (imgyes[1], "imgyes[1]", &loc,
					screen, &dest, arImage); break;
			case 7: /*** Yes on ***/
				CustomBlitSurface (imgyes[2], "imgyes[2]", &loc,
					screen, &dest, arImage); break;
			case 8: /*** No off ***/
				CustomBlitSurface (imgno[1], "imgno[1]", &loc,
					screen, &dest, arImage); break;
			case 9: /*** No on ***/
				CustomBlitSurface (imgno[2], "imgno[2]", &loc,
					screen, &dest, arImage); break;
			case 10: /*** help ***/
				CustomBlitSurface (imghelp, "imghelp", &loc,
					screen, &dest, arImage); break;
		}
	}
}
/*****************************************************************************/
int InArea (SDL_Event event, int iUpperLeftX, int iUpperLeftY,
	int iLowerRightX, int iLowerRightY)
/*****************************************************************************/
{
	if ((iUpperLeftX * iScale <= event.motion.x) &&
		(iLowerRightX * iScale >= event.motion.x) &&
		(iUpperLeftY * iScale <= event.motion.y) &&
		(iLowerRightY * iScale >= event.motion.y))
	{
		return (1);
	} else {
		return (0);
	}
}
/*****************************************************************************/
void InitScreen (void)
/*****************************************************************************/
{
	SDL_Event event;
	SDL_Surface *imgicon;
	SDL_AudioSpec fmt;
	int iTemp2;
	int iEventRoom;
	int iEventDoor;
	int iOldXPos, iOldYPos;
	Uint8 *keystate;
	Uint32 color1;
	SDL_Rect barbox;
	Uint32 newticks;
	Uint32 oldticks;
	int iTile;

	/*** The following are used for changing bits. ***/
	int iTemp;

	if (SDL_Init (SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0)
	{
		printf ("[FAILED] Unable to init SDL: %s!\n", SDL_GetError());
		exit (EXIT_ERROR);
	}
	atexit(SDL_Quit);

	PUTENV ("SDL_VIDEO_CENTERED=1");
	screen = SDL_SetVideoMode ((SCREEN_WIDTH) * iScale, (SCREEN_HEIGHT) * iScale,
		SCREEN_BPP, SDL_DOUBLEBUF|SDL_SWSURFACE);
	if ( screen == NULL )
	{
		printf ("[FAILED] Unable to set video mode: %s!\n", SDL_GetError());
		exit (EXIT_ERROR);
	}

	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	if (TTF_Init() == -1)
	{
		exit (EXIT_ERROR);
	}

	LoadFonts();

	if (iNoAudio != 1)
	{
		PrIfDe ("[  OK  ] Initializing Audio\n");
		fmt.freq = 22050;
		fmt.format = AUDIO_S16;
		fmt.channels = 2;
		fmt.samples = 512;
		fmt.callback = MixAudio;
		fmt.userdata = NULL;
		if (SDL_OpenAudio (&fmt, NULL) < 0)
		{
			printf ("[FAILED] Unable to open audio: %s!\n", SDL_GetError());
			exit (EXIT_ERROR);
		}
		SDL_PauseAudio (0);
	}

	/*** caption, icon ***/
	PreLoad (PNG_VARIOUS, "apoplexy_icon.png", &imgicon);
	SDL_WM_SetCaption (EDITOR_NAME " " EDITOR_VERSION, NULL);
	SDL_WM_SetIcon (imgicon, NULL);

	/*** PoP select ***/
	if (iEditPoP == 0)
	{
		PreLoad (PNG_VARIOUS, "PoP1_PoP2.png", &imgpop);
		PreLoad (PNG_VARIOUS, "PoP1_disabled.png", &imgpop1dis);
		PreLoad (PNG_VARIOUS, "PoP1_off.png", &imgpop1off);
		PreLoad (PNG_VARIOUS, "PoP1_on.png", &imgpop1on);
		PreLoad (PNG_VARIOUS, "PoP2_disabled.png", &imgpop2dis);
		PreLoad (PNG_VARIOUS, "PoP2_off.png", &imgpop2off);
		PreLoad (PNG_VARIOUS, "PoP2_on.png", &imgpop2on);
		PreLoad (PNG_VARIOUS, "PR_problem.png", &imgprprob);
		PreLoad (PNG_VARIOUS, "PoP_problem.png", &imgpopprob);
		PoP1OrPoP2();
		PlaySound ("wav/ok_close.wav");
	}

	/*** loading ***/
	PreLoad (PNG_VARIOUS, "black.png", &imgblack);
	PreLoad (PNG_VARIOUS, "hourglass.png", &imghourglass);
	PreLoad (PNG_VARIOUS, "hourglass_sprite.png", &imghourglasssprite);
	PreLoad (PNG_VARIOUS, "sand_sprite.png", &imgsandsprite);
	ShowImage (-4, (int[]){1, 0, 0, 0}, screen, 31, 0, 0, 692, 455);
	ShowImage (-4, (int[]){2, 0, 0, 0}, screen, 46, 0, 0, 93, 89);
	color1 = SDL_MapRGB (screen->format, 0x22, 0x22, 0x22);
	barbox.x = 10;
	barbox.y = 10;
	barbox.w = 20;
	barbox.h = 435;
	SDL_FillRect (screen, &barbox, color1);
	iHourglassFrame = 1;
	iSandFrame = 1;

	/*** refresh screen ***/
	if (SDL_Flip (screen) == -1)
		{ printf ("[FAILED] SDL_Flip error!\n"); exit (EXIT_ERROR); }

	/*****************************/
	/* Preload all other images. */
	/*****************************/

	iPreLoaded = 0;
	iCurrentBarHeight = 0;
	if (iEditPoP == 1)
	{
		iNrToPreLoad = 454; /*** Shown in debug mode. ***/
	} else {
		iNrToPreLoad = 843; /*** Shown in debug mode. ***/
	}

	/*** locations; dungeon ***/
	if (iEditPoP == 1)
	{
		PreLoadSet (PNG_DUNGEON, 'd', "0_0", &imgd0_0[1], &imgd0_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "0_1", &imgd0_1[1], &imgd0_1[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "0_2", &imgd0_2[1], &imgd0_2[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "0_3", &imgd0_3[1], &imgd0_3[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "0_255", &imgd0_255[1], &imgd0_255[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "1_0", &imgd1_0[1], &imgd1_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "1_1", &imgd1_1[1], &imgd1_1[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "1_2", &imgd1_2[1], &imgd1_2[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "1_3", &imgd1_3[1], &imgd1_3[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "1_255", &imgd1_255[1], &imgd1_255[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "2_0", &imgd2_0[1], &imgd2_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "2_1", &imgd2_1[1], &imgd2_1[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "2_2", &imgd2_2[1], &imgd2_2[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "2_3", &imgd2_3[1], &imgd2_3[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "2_4", &imgd2_4[1], &imgd2_4[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "2_5", &imgd2_5[1], &imgd2_5[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "2_6", &imgd2_6[1], &imgd2_6[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "2_7", &imgd2_7[1], &imgd2_7[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "2_8", &imgd2_8[1], &imgd2_8[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "2_9", &imgd2_9[1], &imgd2_9[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "3_0", &imgd3_0[1], &imgd3_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "4_0", &imgd4_0[1], &imgd4_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "4_1", &imgd4_1[1], &imgd4_1[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "5_0", &imgd5_0[1], &imgd5_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "6", &imgd6[1], &imgd6[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "7_0", &imgd7_0[1], &imgd7_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "7_1", &imgd7_1[1], &imgd7_1[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "7_2", &imgd7_2[1], &imgd7_2[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "7_3", &imgd7_3[1], &imgd7_3[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "8_0", &imgd8_0[1], &imgd8_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "9_0", &imgd9_0[1], &imgd9_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "10_0", &imgd10_0[1], &imgd10_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "10_1", &imgd10_1[1], &imgd10_1[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "10_2", &imgd10_2[1], &imgd10_2[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "10_3", &imgd10_3[1], &imgd10_3[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "10_4", &imgd10_4[1], &imgd10_4[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "10_5", &imgd10_5[1], &imgd10_5[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "10_6", &imgd10_6[1], &imgd10_6[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "11_0", &imgd11_0[1], &imgd11_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "12_0", &imgd12_0[1], &imgd12_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "12_1", &imgd12_1[1], &imgd12_1[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "12_2", &imgd12_2[1], &imgd12_2[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "12_3", &imgd12_3[1], &imgd12_3[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "12_4", &imgd12_4[1], &imgd12_4[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "12_5", &imgd12_5[1], &imgd12_5[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "12_6", &imgd12_6[1], &imgd12_6[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "12_7", &imgd12_7[1], &imgd12_7[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "13_0", &imgd13_0[1], &imgd13_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "14_0", &imgd14_0[1], &imgd14_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "15", &imgd15[1], &imgd15[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "16_0", &imgd16_0[1], &imgd16_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "17_0", &imgd17_0[1], &imgd17_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "18_0", &imgd18_0[1], &imgd18_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "18_1", &imgd18_1[1], &imgd18_1[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "18_2", &imgd18_2[1], &imgd18_2[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "18_3", &imgd18_3[1], &imgd18_3[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "18_4", &imgd18_4[1], &imgd18_4[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "18_5", &imgd18_5[1], &imgd18_5[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "19_0", &imgd19_0[1], &imgd19_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "20_0", &imgd20_0[1], &imgd20_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "20_1", &imgd20_1[1], &imgd20_1[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "21_0", &imgd21_0[1], &imgd21_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "22_0", &imgd22_0[1], &imgd22_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "23_0", &imgd23_0[1], &imgd23_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "24_0", &imgd24_0[1], &imgd24_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "25_0", &imgd25_0[1], &imgd25_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "26_0", &imgd26_0[1], &imgd26_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "27_0", &imgd27_0[1], &imgd27_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "28_0", &imgd28_0[1], &imgd28_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "29_0", &imgd29_0[1], &imgd29_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "30_0", &imgd30_0[1], &imgd30_0[2]);
		PreLoadSet (PNG_DUNGEON, 'd', "43_0", &imgd43_0[1], &imgd43_0[2]);
		PreLoad (PNG_DUNGEON, "d_19_0_sprite.png", &spriteflamed1);
		PreLoad (PNG_DUNGEON, "d_30_0_sprite.png", &spriteflamed2);
		PreLoad (PNG_DUNGEON, "d_18_0_sprite.png", &spritechomperd);
		PreLoad (PNG_DUNGEON, "d_sel_18_0_sprite.png", &spritechomperdsel);
		PreLoad (PNG_DUNGEON, "d_22_0_sprite.png", &spriteswordd);
		PreLoad (PNG_DUNGEON, "d_sel_22_0_sprite.png", &spritesworddsel);
	}

	/*** locations; palace ***/
	if (iEditPoP == 1)
	{
		PreLoadSet (PNG_PALACE, 'p', "0_0", &imgp0_0[1], &imgp0_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "0_1", &imgp0_1[1], &imgp0_1[2]);
		PreLoadSet (PNG_PALACE, 'p', "0_2", &imgp0_2[1], &imgp0_2[2]);
		PreLoadSet (PNG_PALACE, 'p', "0_3", &imgp0_3[1], &imgp0_3[2]);
		PreLoadSet (PNG_PALACE, 'p', "0_255", &imgp0_255[1], &imgp0_255[2]);
		PreLoadSet (PNG_PALACE, 'p', "1_0", &imgp1_0[1], &imgp1_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "1_1", &imgp1_1[1], &imgp1_1[2]);
		PreLoadSet (PNG_PALACE, 'p', "1_2", &imgp1_2[1], &imgp1_2[2]);
		PreLoadSet (PNG_PALACE, 'p', "1_3", &imgp1_3[1], &imgp1_3[2]);
		PreLoadSet (PNG_PALACE, 'p', "1_255", &imgp1_255[1], &imgp1_255[2]);
		PreLoadSet (PNG_PALACE, 'p', "2_0", &imgp2_0[1], &imgp2_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "2_1", &imgp2_1[1], &imgp2_1[2]);
		PreLoadSet (PNG_PALACE, 'p', "2_2", &imgp2_2[1], &imgp2_2[2]);
		PreLoadSet (PNG_PALACE, 'p', "2_3", &imgp2_3[1], &imgp2_3[2]);
		PreLoadSet (PNG_PALACE, 'p', "2_4", &imgp2_4[1], &imgp2_4[2]);
		PreLoadSet (PNG_PALACE, 'p', "2_5", &imgp2_5[1], &imgp2_5[2]);
		PreLoadSet (PNG_PALACE, 'p', "2_6", &imgp2_6[1], &imgp2_6[2]);
		PreLoadSet (PNG_PALACE, 'p', "2_7", &imgp2_7[1], &imgp2_7[2]);
		PreLoadSet (PNG_PALACE, 'p', "2_8", &imgp2_8[1], &imgp2_8[2]);
		PreLoadSet (PNG_PALACE, 'p', "2_9", &imgp2_9[1], &imgp2_9[2]);
		PreLoadSet (PNG_PALACE, 'p', "3_0", &imgp3_0[1], &imgp3_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "4_0", &imgp4_0[1], &imgp4_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "4_1", &imgp4_1[1], &imgp4_1[2]);
		PreLoadSet (PNG_PALACE, 'p', "5_0", &imgp5_0[1], &imgp5_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "6", &imgp6[1], &imgp6[2]);
		PreLoadSet (PNG_PALACE, 'p', "7_0", &imgp7_0[1], &imgp7_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "7_1", &imgp7_1[1], &imgp7_1[2]);
		PreLoadSet (PNG_PALACE, 'p', "7_2", &imgp7_2[1], &imgp7_2[2]);
		PreLoadSet (PNG_PALACE, 'p', "7_3", &imgp7_3[1], &imgp7_3[2]);
		PreLoadSet (PNG_PALACE, 'p', "8_0", &imgp8_0[1], &imgp8_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "9_0", &imgp9_0[1], &imgp9_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "10_0", &imgp10_0[1], &imgp10_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "10_1", &imgp10_1[1], &imgp10_1[2]);
		PreLoadSet (PNG_PALACE, 'p', "10_2", &imgp10_2[1], &imgp10_2[2]);
		PreLoadSet (PNG_PALACE, 'p', "10_3", &imgp10_3[1], &imgp10_3[2]);
		PreLoadSet (PNG_PALACE, 'p', "10_4", &imgp10_4[1], &imgp10_4[2]);
		PreLoadSet (PNG_PALACE, 'p', "10_5", &imgp10_5[1], &imgp10_5[2]);
		PreLoadSet (PNG_PALACE, 'p', "10_6", &imgp10_6[1], &imgp10_6[2]);
		PreLoadSet (PNG_PALACE, 'p', "11_0", &imgp11_0[1], &imgp11_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "12_0", &imgp12_0[1], &imgp12_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "12_1", &imgp12_1[1], &imgp12_1[2]);
		PreLoadSet (PNG_PALACE, 'p', "12_2", &imgp12_2[1], &imgp12_2[2]);
		PreLoadSet (PNG_PALACE, 'p', "12_3", &imgp12_3[1], &imgp12_3[2]);
		PreLoadSet (PNG_PALACE, 'p', "12_4", &imgp12_4[1], &imgp12_4[2]);
		PreLoadSet (PNG_PALACE, 'p', "12_5", &imgp12_5[1], &imgp12_5[2]);
		PreLoadSet (PNG_PALACE, 'p', "12_6", &imgp12_6[1], &imgp12_6[2]);
		PreLoadSet (PNG_PALACE, 'p', "12_7", &imgp12_7[1], &imgp12_7[2]);
		PreLoadSet (PNG_PALACE, 'p', "13_0", &imgp13_0[1], &imgp13_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "14_0", &imgp14_0[1], &imgp14_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "15", &imgp15[1], &imgp15[2]);
		PreLoadSet (PNG_PALACE, 'p', "16_0", &imgp16_0[1], &imgp16_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "17_0", &imgp17_0[1], &imgp17_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "18_0", &imgp18_0[1], &imgp18_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "18_1", &imgp18_1[1], &imgp18_1[2]);
		PreLoadSet (PNG_PALACE, 'p', "18_2", &imgp18_2[1], &imgp18_2[2]);
		PreLoadSet (PNG_PALACE, 'p', "18_3", &imgp18_3[1], &imgp18_3[2]);
		PreLoadSet (PNG_PALACE, 'p', "18_4", &imgp18_4[1], &imgp18_4[2]);
		PreLoadSet (PNG_PALACE, 'p', "18_5", &imgp18_5[1], &imgp18_5[2]);
		PreLoadSet (PNG_PALACE, 'p', "19_0", &imgp19_0[1], &imgp19_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "20_0", &imgp20_0[1], &imgp20_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "20_1", &imgp20_1[1], &imgp20_1[2]);
		PreLoadSet (PNG_PALACE, 'p', "21_0", &imgp21_0[1], &imgp21_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "22_0", &imgp22_0[1], &imgp22_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "23_0", &imgp23_0[1], &imgp23_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "24_0", &imgp24_0[1], &imgp24_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "25_0", &imgp25_0[1], &imgp25_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "26_0", &imgp26_0[1], &imgp26_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "27_0", &imgp27_0[1], &imgp27_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "28_0", &imgp28_0[1], &imgp28_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "29_0", &imgp29_0[1], &imgp29_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "30_0", &imgp30_0[1], &imgp30_0[2]);
		PreLoadSet (PNG_PALACE, 'p', "43_0", &imgp43_0[1], &imgp43_0[2]);
		PreLoad (PNG_PALACE, "p_19_0_sprite.png", &spriteflamep1);
		PreLoad (PNG_PALACE, "p_30_0_sprite.png", &spriteflamep2);
		PreLoad (PNG_PALACE, "p_18_0_sprite.png", &spritechomperp);
		PreLoad (PNG_PALACE, "p_sel_18_0_sprite.png", &spritechomperpsel);
		PreLoad (PNG_PALACE, "p_22_0_sprite.png", &spriteswordp);
		PreLoad (PNG_PALACE, "p_sel_22_0_sprite.png", &spriteswordpsel);
	}

	/*** locations; rooftops, desert, final ***/
	if (iEditPoP == 2)
	{
		PreLoadSet (PNG_OTH, 'o', "0", &imgo0[1], &imgo0[2]);
		PreLoadSet (PNG_OTH, 'o', "1", &imgo1[1], &imgo1[2]);
		PreLoadSet (PNG_OTH, 'o', "2", &imgo2[1], &imgo2[2]);
		PreLoadSet (PNG_OTH, 'o', "3", &imgo3[1], &imgo3[2]);
		PreLoadSet (PNG_OTH, 'o', "4", &imgo4[1], &imgo4[2]);
		PreLoadSet (PNG_OTH, 'o', "5", &imgo5[1], &imgo5[2]);
		PreLoadSet (PNG_OTH, 'o', "6", &imgo6[1], &imgo6[2]);
		PreLoadSet (PNG_OTH, 'o', "7_0", &imgo7_0[1], &imgo7_0[2]);
		PreLoadSet (PNG_OTH, 'o', "7_1", &imgo7_1[1], &imgo7_1[2]);
		PreLoadSet (PNG_OTH, 'o', "7_2", &imgo7_2[1], &imgo7_2[2]);
		PreLoadSet (PNG_OTH, 'o', "7_3", &imgo7_3[1], &imgo7_3[2]);
		PreLoadSet (PNG_OTH, 'o', "8", &imgo8[1], &imgo8[2]);
		PreLoadSet (PNG_OTH, 'o', "9", &imgo9[1], &imgo9[2]);
		PreLoadSet (PNG_OTH, 'o', "10_0", &imgo10_0[1], &imgo10_0[2]);
		PreLoadSet (PNG_OTH, 'o', "10_32", &imgo10_32[1], &imgo10_32[2]);
		PreLoadSet (PNG_OTH, 'o', "10_64", &imgo10_64[1], &imgo10_64[2]);
		PreLoadSet (PNG_OTH, 'o', "10_96", &imgo10_96[1], &imgo10_96[2]);
		PreLoadSet (PNG_OTH, 'o', "10_128", &imgo10_128[1], &imgo10_128[2]);
		PreLoadSet (PNG_OTH, 'o', "10_160", &imgo10_160[1], &imgo10_160[2]);
		PreLoadSet (PNG_OTH, 'o', "10_192", &imgo10_192[1], &imgo10_192[2]);
		PreLoadSet (PNG_OTH, 'o', "10_224", &imgo10_224[1], &imgo10_224[2]);
		PreLoadSet (PNG_OTH, 'o', "11", &imgo11[1], &imgo11[2]);
		PreLoadSet (PNG_OTH, 'o', "12", &imgo12[1], &imgo12[2]);
		PreLoadSet (PNG_OTH, 'o', "13", &imgo13[1], &imgo13[2]);
		PreLoadSet (PNG_OTH, 'o', "14", &imgo14[1], &imgo14[2]);
		PreLoadSet (PNG_OTH, 'o', "15", &imgo15[1], &imgo15[2]);
		PreLoadSet (PNG_OTH, 'o', "16", &imgo16[1], &imgo16[2]);
		PreLoadSet (PNG_OTH, 'o', "17", &imgo17[1], &imgo17[2]);
		PreLoadSet (PNG_OTH, 'o', "18", &imgo18[1], &imgo18[2]);
		PreLoadSet (PNG_OTH, 'o', "19", &imgo19[1], &imgo19[2]);
		PreLoadSet (PNG_OTH, 'o', "20", &imgo20[1], &imgo20[2]);
		PreLoadSet (PNG_OTH, 'o', "21", &imgo21[1], &imgo21[2]);
		PreLoadSet (PNG_OTH, 'o', "22_0", &imgo22_0[1], &imgo22_0[2]);
		PreLoadSet (PNG_OTH, 'o', "22_1", &imgo22_1[1], &imgo22_1[2]);
		PreLoadSet (PNG_OTH, 'o', "23", &imgo23[1], &imgo23[2]);
		PreLoadSet (PNG_OTH, 'o', "24", &imgo24[1], &imgo24[2]);
		PreLoadSet (PNG_OTH, 'o', "25", &imgo25[1], &imgo25[2]);
		PreLoadSet (PNG_OTH, 'o', "26", &imgo26[1], &imgo26[2]);
		PreLoadSet (PNG_OTH, 'o', "27", &imgo27[1], &imgo27[2]);
		PreLoadSet (PNG_OTH, 'o', "28", &imgo28[1], &imgo28[2]);
		PreLoadSet (PNG_OTH, 'o', "29", &imgo29[1], &imgo29[2]);
		PreLoadSet (PNG_OTH, 'o', "30", &imgo30[1], &imgo30[2]);
		PreLoadSet (PNG_OTH, 'o', "31", &imgo31[1], &imgo31[2]);
		PreLoadSet (PNG_OTH, 'o', "32", &imgo32[1], &imgo32[2]);
		PreLoadSet (PNG_OTH, 'o', "33", &imgo33[1], &imgo33[2]);
		PreLoadSet (PNG_OTH, 'o', "34", &imgo34[1], &imgo34[2]);
		PreLoadSet (PNG_OTH, 'o', "35", &imgo35[1], &imgo35[2]);
		PreLoadSet (PNG_OTH, 'o', "36", &imgo36[1], &imgo36[2]);
		PreLoadSet (PNG_OTH, 'o', "37", &imgo37[1], &imgo37[2]);
		PreLoadSet (PNG_OTH, 'o', "38", &imgo38[1], &imgo38[2]);
		PreLoadSet (PNG_OTH, 'o', "39", &imgo39[1], &imgo39[2]);
		PreLoadSet (PNG_OTH, 'o', "40", &imgo40[1], &imgo40[2]);
		PreLoadSet (PNG_OTH, 'o', "41", &imgo41[1], &imgo41[2]);
		PreLoadSet (PNG_OTH, 'o', "43", &imgo43[1], &imgo43[2]);
		PreLoadSet (PNG_OTH, 'o', "44", &imgo44[1], &imgo44[2]);
	}

	/*** locations; caverns ***/
	if (iEditPoP == 2)
	{
		PreLoadSet (PNG_CAVERNS, 'c', "0_0", &imgc0_0[1], &imgc0_0[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "0_1", &imgc0_1[1], &imgc0_1[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "0_2", &imgc0_2[1], &imgc0_2[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "0_3", &imgc0_3[1], &imgc0_3[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "0_4", &imgc0_4[1], &imgc0_4[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "1_0", &imgc1_0[1], &imgc1_0[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "1_1", &imgc1_1[1], &imgc1_1[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "1_2", &imgc1_2[1], &imgc1_2[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "2_0", &imgc2_0[1], &imgc2_0[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "2_5", &imgc2_5[1], &imgc2_5[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "3", &imgc3[1], &imgc3[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "4_0", &imgc4_0[1], &imgc4_0[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "4_100", &imgc4_100[1], &imgc4_100[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "4_200", &imgc4_200[1], &imgc4_200[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "8_0", &imgc8_0[1], &imgc8_0[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "8_1", &imgc8_1[1], &imgc8_1[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "8_2", &imgc8_2[1], &imgc8_2[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "9", &imgc9[1], &imgc9[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "10_0", &imgc10_0[1], &imgc10_0[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "10_32", &imgc10_32[1], &imgc10_32[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "10_64", &imgc10_64[1], &imgc10_64[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "10_96", &imgc10_96[1], &imgc10_96[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "10_128", &imgc10_128[1], &imgc10_128[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "10_160", &imgc10_160[1], &imgc10_160[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "10_192", &imgc10_192[1], &imgc10_192[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "10_224", &imgc10_224[1], &imgc10_224[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "11_0", &imgc11_0[1], &imgc11_0[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "11_128", &imgc11_128[1], &imgc11_128[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "14_0", &imgc14_0[1], &imgc14_0[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "14_1", &imgc14_1[1], &imgc14_1[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "14_2", &imgc14_2[1], &imgc14_2[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "16", &imgc16[1], &imgc16[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "17_0", &imgc17_0[1], &imgc17_0[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "17_21", &imgc17_21[1], &imgc17_21[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "17_42", &imgc17_42[1], &imgc17_42[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "18", &imgc18[1], &imgc18[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "19", &imgc19[1], &imgc19[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_0", &imgc20_0[1], &imgc20_0[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_1", &imgc20_1[1], &imgc20_1[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_2", &imgc20_2[1], &imgc20_2[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_3", &imgc20_3[1], &imgc20_3[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_4", &imgc20_4[1], &imgc20_4[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_5", &imgc20_5[1], &imgc20_5[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_6", &imgc20_6[1], &imgc20_6[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_7", &imgc20_7[1], &imgc20_7[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_8", &imgc20_8[1], &imgc20_8[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_9", &imgc20_9[1], &imgc20_9[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_10", &imgc20_10[1], &imgc20_10[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_128", &imgc20_128[1], &imgc20_128[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_129", &imgc20_129[1], &imgc20_129[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_130", &imgc20_130[1], &imgc20_130[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_131", &imgc20_131[1], &imgc20_131[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_132", &imgc20_132[1], &imgc20_132[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_133", &imgc20_133[1], &imgc20_133[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_134", &imgc20_134[1], &imgc20_134[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_135", &imgc20_135[1], &imgc20_135[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_136", &imgc20_136[1], &imgc20_136[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_137", &imgc20_137[1], &imgc20_137[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "20_138", &imgc20_138[1], &imgc20_138[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "23", &imgc23[1], &imgc23[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "24", &imgc24[1], &imgc24[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "28", &imgc28[1], &imgc28[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "32", &imgc32[1], &imgc32[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "33", &imgc33[1], &imgc33[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "34_E_0", &imgc34_E_0[1], &imgc34_E_0[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "34_E_8", &imgc34_E_8[1], &imgc34_E_8[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "35", &imgc35[1], &imgc35[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "36", &imgc36[1], &imgc36[2]);
		PreLoadSet (PNG_CAVERNS, 'c', "44", &imgc44[1], &imgc44[2]);
		PreLoad (PNG_CAVERNS, "c_19_sprite.png", &spriteflamec1);
		PreLoad (PNG_CAVERNS, "c_32_sprite.png", &spriteflamec2);
	}

	/*** locations; ruins ***/
	if (iEditPoP == 2)
	{
		PreLoadSet (PNG_RUINS, 'r', "0", &imgr0[1], &imgr0[2]);
		PreLoadSet (PNG_RUINS, 'r', "1_0", &imgr1_0[1], &imgr1_0[2]);
		PreLoadSet (PNG_RUINS, 'r', "1_1", &imgr1_1[1], &imgr1_1[2]);
		PreLoadSet (PNG_RUINS, 'r', "1_2", &imgr1_2[1], &imgr1_2[2]);
		PreLoadSet (PNG_RUINS, 'r', "1_3", &imgr1_3[1], &imgr1_3[2]);
		PreLoadSet (PNG_RUINS, 'r', "2", &imgr2[1], &imgr2[2]);
		PreLoadSet (PNG_RUINS, 'r', "3_0", &imgr3_0[1], &imgr3_0[2]);
		PreLoadSet (PNG_RUINS, 'r', "3_1", &imgr3_1[1], &imgr3_1[2]);
		PreLoadSet (PNG_RUINS, 'r', "3_2", &imgr3_2[1], &imgr3_2[2]);
		PreLoadSet (PNG_RUINS, 'r', "4_0", &imgr4_0[1], &imgr4_0[2]);
		PreLoadSet (PNG_RUINS, 'r', "4_100", &imgr4_100[1], &imgr4_100[2]);
		PreLoadSet (PNG_RUINS, 'r', "4_200", &imgr4_200[1], &imgr4_200[2]);
		PreLoadSet (PNG_RUINS, 'r', "5_E_0", &imgr5_E_0[1], &imgr5_E_0[2]);
		PreLoadSet (PNG_RUINS, 'r', "5_E_8", &imgr5_E_8[1], &imgr5_E_8[2]);
		PreLoadSet (PNG_RUINS, 'r', "6_E_0", &imgr6_E_0[1], &imgr6_E_0[2]);
		PreLoadSet (PNG_RUINS, 'r', "6_E_8", &imgr6_E_8[1], &imgr6_E_8[2]);
		PreLoadSet (PNG_RUINS, 'r', "7_0", &imgr7_0[1], &imgr7_0[2]);
		PreLoadSet (PNG_RUINS, 'r', "7_1", &imgr7_1[1], &imgr7_1[2]);
		PreLoadSet (PNG_RUINS, 'r', "7_2", &imgr7_2[1], &imgr7_2[2]);
		PreLoadSet (PNG_RUINS, 'r', "7_3", &imgr7_3[1], &imgr7_3[2]);
		PreLoadSet (PNG_RUINS, 'r', "8", &imgr8[1], &imgr8[2]);
		PreLoadSet (PNG_RUINS, 'r', "9", &imgr9[1], &imgr9[2]);
		PreLoadSet (PNG_RUINS, 'r', "10_0", &imgr10_0[1], &imgr10_0[2]);
		PreLoadSet (PNG_RUINS, 'r', "10_32", &imgr10_32[1], &imgr10_32[2]);
		PreLoadSet (PNG_RUINS, 'r', "10_64", &imgr10_64[1], &imgr10_64[2]);
		PreLoadSet (PNG_RUINS, 'r', "10_96", &imgr10_96[1], &imgr10_96[2]);
		PreLoadSet (PNG_RUINS, 'r', "10_128", &imgr10_128[1], &imgr10_128[2]);
		PreLoadSet (PNG_RUINS, 'r', "10_160", &imgr10_160[1], &imgr10_160[2]);
		PreLoadSet (PNG_RUINS, 'r', "10_192", &imgr10_192[1], &imgr10_192[2]);
		PreLoadSet (PNG_RUINS, 'r', "10_224", &imgr10_224[1], &imgr10_224[2]);
		PreLoadSet (PNG_RUINS, 'r', "11", &imgr11[1], &imgr11[2]);
		PreLoadSet (PNG_RUINS, 'r', "12_0", &imgr12_0[1], &imgr12_0[2]);
		PreLoadSet (PNG_RUINS, 'r', "12_128", &imgr12_128[1], &imgr12_128[2]);
		PreLoadSet (PNG_RUINS, 'r', "12_160", &imgr12_160[1], &imgr12_160[2]);
		PreLoadSet (PNG_RUINS, 'r', "12_32", &imgr12_32[1], &imgr12_32[2]);
		PreLoadSet (PNG_RUINS, 'r', "13_0", &imgr13_0[1], &imgr13_0[2]);
		PreLoadSet (PNG_RUINS, 'r', "13_32", &imgr13_32[1], &imgr13_32[2]);
		PreLoadSet (PNG_RUINS, 'r', "14_16", &imgr14_16[1], &imgr14_16[2]);
		PreLoadSet (PNG_RUINS, 'r', "14_32", &imgr14_32[1], &imgr14_32[2]);
		PreLoadSet (PNG_RUINS, 'r', "14_64", &imgr14_64[1], &imgr14_64[2]);
		PreLoadSet (PNG_RUINS, 'r', "15_0", &imgr15_0[1], &imgr15_0[2]);
		PreLoadSet (PNG_RUINS, 'r', "15_1", &imgr15_1[1], &imgr15_1[2]);
		PreLoadSet (PNG_RUINS, 'r', "15_2", &imgr15_2[1], &imgr15_2[2]);
		PreLoadSet (PNG_RUINS, 'r', "15_3", &imgr15_3[1], &imgr15_3[2]);
		PreLoadSet (PNG_RUINS, 'r', "16", &imgr16[1], &imgr16[2]);
		PreLoadSet (PNG_RUINS, 'r', "17_0", &imgr17_0[1], &imgr17_0[2]);
		PreLoadSet (PNG_RUINS, 'r', "17_21", &imgr17_21[1], &imgr17_21[2]);
		PreLoadSet (PNG_RUINS, 'r', "17_42", &imgr17_42[1], &imgr17_42[2]);
		PreLoadSet (PNG_RUINS, 'r', "18", &imgr18[1], &imgr18[2]);
		PreLoadSet (PNG_RUINS, 'r', "20_0", &imgr20_0[1], &imgr20_0[2]);
		PreLoadSet (PNG_RUINS, 'r', "20_1", &imgr20_1[1], &imgr20_1[2]);
		PreLoadSet (PNG_RUINS, 'r', "20_2", &imgr20_2[1], &imgr20_2[2]);
		PreLoadSet (PNG_RUINS, 'r', "20_4", &imgr20_4[1], &imgr20_4[2]);
		PreLoadSet (PNG_RUINS, 'r', "20_5", &imgr20_5[1], &imgr20_5[2]);
		PreLoadSet (PNG_RUINS, 'r', "20_6", &imgr20_6[1], &imgr20_6[2]);
		PreLoadSet (PNG_RUINS, 'r', "20_8", &imgr20_8[1], &imgr20_8[2]);
		PreLoadSet (PNG_RUINS, 'r', "20_9", &imgr20_9[1], &imgr20_9[2]);
		PreLoadSet (PNG_RUINS, 'r', "20_10", &imgr20_10[1], &imgr20_10[2]);
		PreLoadSet (PNG_RUINS, 'r', "20_12", &imgr20_12[1], &imgr20_12[2]);
		PreLoadSet (PNG_RUINS, 'r', "20_13", &imgr20_13[1], &imgr20_13[2]);
		PreLoadSet (PNG_RUINS, 'r', "20_14", &imgr20_14[1], &imgr20_14[2]);
		PreLoadSet (PNG_RUINS, 'r', "21", &imgr21[1], &imgr21[2]);
		PreLoadSet (PNG_RUINS, 'r', "22_1", &imgr22_1[1], &imgr22_1[2]);
		PreLoadSet (PNG_RUINS, 'r', "22_2", &imgr22_2[1], &imgr22_2[2]);
	}

	/*** locations; temple ***/
	if (iEditPoP == 2)
	{
		PreLoadSet (PNG_TEMPLE, 't', "0", &imgt0[1], &imgt0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "1_0", &imgt1_0[1], &imgt1_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "1_1", &imgt1_1[1], &imgt1_1[2]);
		PreLoadSet (PNG_TEMPLE, 't', "2_0", &imgt2_0[1], &imgt2_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "2_5", &imgt2_5[1], &imgt2_5[2]);
		PreLoadSet (PNG_TEMPLE, 't', "3_0", &imgt3_0[1], &imgt3_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "3_1", &imgt3_1[1], &imgt3_1[2]);
		PreLoadSet (PNG_TEMPLE, 't', "4_0", &imgt4_0[1], &imgt4_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "4_100", &imgt4_100[1], &imgt4_100[2]);
		PreLoadSet (PNG_TEMPLE, 't', "4_200", &imgt4_200[1], &imgt4_200[2]);
		PreLoadSet (PNG_TEMPLE, 't', "5_E_0", &imgt5_E_0[1], &imgt5_E_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "5_E_8", &imgt5_E_8[1], &imgt5_E_8[2]);
		PreLoadSet (PNG_TEMPLE, 't', "6_E_0", &imgt6_E_0[1], &imgt6_E_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "6_E_8", &imgt6_E_8[1], &imgt6_E_8[2]);
		PreLoadSet (PNG_TEMPLE, 't', "8", &imgt8[1], &imgt8[2]);
		PreLoadSet (PNG_TEMPLE, 't', "9", &imgt9[1], &imgt9[2]);
		PreLoadSet (PNG_TEMPLE, 't', "10_0", &imgt10_0[1], &imgt10_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "10_32", &imgt10_32[1], &imgt10_32[2]);
		PreLoadSet (PNG_TEMPLE, 't', "10_64", &imgt10_64[1], &imgt10_64[2]);
		PreLoadSet (PNG_TEMPLE, 't', "10_96", &imgt10_96[1], &imgt10_96[2]);
		PreLoadSet (PNG_TEMPLE, 't', "10_128", &imgt10_128[1], &imgt10_128[2]);
		PreLoadSet (PNG_TEMPLE, 't', "10_160", &imgt10_160[1], &imgt10_160[2]);
		PreLoadSet (PNG_TEMPLE, 't', "10_192", &imgt10_192[1], &imgt10_192[2]);
		PreLoadSet (PNG_TEMPLE, 't', "10_224", &imgt10_224[1], &imgt10_224[2]);
		PreLoadSet (PNG_TEMPLE, 't', "11", &imgt11[1], &imgt11[2]);
		PreLoadSet (PNG_TEMPLE, 't', "12_0", &imgt12_0[1], &imgt12_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "12_128", &imgt12_128[1], &imgt12_128[2]);
		PreLoadSet (PNG_TEMPLE, 't', "12_160", &imgt12_160[1], &imgt12_160[2]);
		PreLoadSet (PNG_TEMPLE, 't', "12_32", &imgt12_32[1], &imgt12_32[2]);
		PreLoadSet (PNG_TEMPLE, 't', "13_0", &imgt13_0[1], &imgt13_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "13_32", &imgt13_32[1], &imgt13_32[2]);
		PreLoadSet (PNG_TEMPLE, 't', "14_0", &imgt14_0[1], &imgt14_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "14_1", &imgt14_1[1], &imgt14_1[2]);
		PreLoadSet (PNG_TEMPLE, 't', "16", &imgt16[1], &imgt16[2]);
		PreLoadSet (PNG_TEMPLE, 't', "17_0", &imgt17_0[1], &imgt17_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "17_21", &imgt17_21[1], &imgt17_21[2]);
		PreLoadSet (PNG_TEMPLE, 't', "17_42", &imgt17_42[1], &imgt17_42[2]);
		PreLoadSet (PNG_TEMPLE, 't', "18", &imgt18[1], &imgt18[2]);
		PreLoadSet (PNG_TEMPLE, 't', "19", &imgt19[1], &imgt19[2]);
		PreLoadSet (PNG_TEMPLE, 't', "20_0", &imgt20_0[1], &imgt20_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "20_1", &imgt20_1[1], &imgt20_1[2]);
		PreLoadSet (PNG_TEMPLE, 't', "20_2", &imgt20_2[1], &imgt20_2[2]);
		PreLoadSet (PNG_TEMPLE, 't', "25_0", &imgt25_0[1], &imgt25_0[2]);
		PreLoadSet (PNG_TEMPLE, 't', "25_1", &imgt25_1[1], &imgt25_1[2]);
		PreLoadSet (PNG_TEMPLE, 't', "25_2", &imgt25_2[1], &imgt25_2[2]);
		PreLoadSet (PNG_TEMPLE, 't', "26", &imgt26[1], &imgt26[2]);
		PreLoadSet (PNG_TEMPLE, 't', "32", &imgt32[1], &imgt32[2]);
		PreLoad (PNG_TEMPLE, "t_19_sprite.png", &spriteflamet1);
		PreLoad (PNG_TEMPLE, "t_32_sprite.png", &spriteflamet2);
	}

	if (iEditPoP == 1)
	{
		/*** PoP1 guards ***/
		PreLoad (PNG_LIVING1, "guard_darkblue_beige_l.png", &imggdb[1]);
		PreLoad (PNG_LIVING1, "guard_darkblue_beige_r.png", &imggdb[2]);
		PreLoad (PNG_LIVING1, "guard_green_yellow_l.png", &imgggy[1]);
		PreLoad (PNG_LIVING1, "guard_green_yellow_r.png", &imgggy[2]);
		PreLoad (PNG_LIVING1, "guard_lightblue_pink_l.png", &imgglp[1]);
		PreLoad (PNG_LIVING1, "guard_lightblue_pink_r.png", &imgglp[2]);
		PreLoad (PNG_LIVING1, "guard_orange_yellow_l.png", &imggoy[1]);
		PreLoad (PNG_LIVING1, "guard_orange_yellow_r.png", &imggoy[2]);
		PreLoad (PNG_LIVING1, "guard_purple_beige_l.png", &imggpb[1]);
		PreLoad (PNG_LIVING1, "guard_purple_beige_r.png", &imggpb[2]);
		PreLoad (PNG_LIVING1, "guard_red_purple_l.png", &imggrp[1]);
		PreLoad (PNG_LIVING1, "guard_red_purple_r.png", &imggrp[2]);
		PreLoad (PNG_LIVING1, "guard_yellow_orange_l.png", &imggyo[1]);
		PreLoad (PNG_LIVING1, "guard_yellow_orange_r.png", &imggyo[2]);
		PreLoad (PNG_LIVING1, "skeleton_left.png", &imgskel[1]);
		PreLoad (PNG_LIVING1, "skeleton_right.png", &imgskel[2]);
		PreLoad (PNG_LIVING1, "jaffar_left.png", &imgjaffar[1]);
		PreLoad (PNG_LIVING1, "jaffar_right.png", &imgjaffar[2]);
		PreLoad (PNG_LIVING1, "guard_fat_l.png", &imgfat[1]);
		PreLoad (PNG_LIVING1, "guard_fat_r.png", &imgfat[2]);
		PreLoad (PNG_LIVING1, "shadow_left.png", &imgshadow[1]);
		PreLoad (PNG_LIVING1, "shadow_right.png", &imgshadow[2]);

		/*** PoP1 guards, selected ***/
		PreLoad (PNG_LIVING1, "sel_guard_left.png", &imggsel[1]);
		PreLoad (PNG_LIVING1, "sel_guard_right.png", &imggsel[2]);
		PreLoad (PNG_LIVING1, "sel_skeleton_left.png", &imgskelsel[1]);
		PreLoad (PNG_LIVING1, "sel_skeleton_right.png", &imgskelsel[2]);
		PreLoad (PNG_LIVING1, "sel_jaffar_left.png", &imgjaffarsel[1]);
		PreLoad (PNG_LIVING1, "sel_jaffar_right.png", &imgjaffarsel[2]);
		PreLoad (PNG_LIVING1, "sel_guard_fat_l.png", &imgfatsel[1]);
		PreLoad (PNG_LIVING1, "sel_guard_fat_r.png", &imgfatsel[2]);
		PreLoad (PNG_LIVING1, "sel_shadow_left.png", &imgshadowsel[1]);
		PreLoad (PNG_LIVING1, "sel_shadow_right.png", &imgshadowsel[2]);
	} else {
		/*** PoP2 guards ***/
		PreLoad (PNG_LIVING2, "birdman_l.png", &imgbirdman[1]);
		PreLoad (PNG_LIVING2, "birdman_r.png", &imgbirdman[2]);
		PreLoad (PNG_LIVING2, "fake_prince_l.png", &imgfakeprince[1]);
		PreLoad (PNG_LIVING2, "fake_prince_r.png", &imgfakeprince[2]);
		PreLoad (PNG_LIVING2, "guard_black_l.png", &imgguardblack[1]);
		PreLoad (PNG_LIVING2, "guard_black_r.png", &imgguardblack[2]);
		PreLoad (PNG_LIVING2, "guard_blue_l.png", &imgguardblue[1]);
		PreLoad (PNG_LIVING2, "guard_blue_r.png", &imgguardblue[2]);
		PreLoad (PNG_LIVING2, "guard_red_l.png", &imgguardred[1]);
		PreLoad (PNG_LIVING2, "guard_red_r.png", &imgguardred[2]);
		PreLoad (PNG_LIVING2, "head_long_hair_l.png", &imgheadlong[1]);
		PreLoad (PNG_LIVING2, "head_long_hair_r.png", &imgheadlong[2]);
		PreLoad (PNG_LIVING2, "head_short_hair_l.png", &imgheadshort[1]);
		PreLoad (PNG_LIVING2, "head_short_hair_r.png", &imgheadshort[2]);
		PreLoad (PNG_LIVING2, "skeleton_awake_l.png", &imgskelawake[1]);
		PreLoad (PNG_LIVING2, "skeleton_awake_r.png", &imgskelawake[2]);
		PreLoad (PNG_LIVING2, "skeleton_awakes_l.png", &imgskelawakes[1]);
		PreLoad (PNG_LIVING2, "skeleton_awakes_r.png", &imgskelawakes[2]);
		PreLoad (PNG_LIVING2, "skeleton_awakes_sws_l.png", &imgskelawakessws[1]);
		PreLoad (PNG_LIVING2, "skeleton_awakes_sws_r.png", &imgskelawakessws[2]);
		PreLoad (PNG_LIVING2, "skeleton_sleeps_l.png", &imgskelsleeps[1]);
		PreLoad (PNG_LIVING2, "skeleton_sleeps_r.png", &imgskelsleeps[2]);
		PreLoad (PNG_LIVING2, "snake_l.png", &imgsnake[1]);
		PreLoad (PNG_LIVING2, "snake_r.png", &imgsnake[2]);
		PreLoad (PNG_LIVING2, "sword_l.png", &imgsword[1]);
		PreLoad (PNG_LIVING2, "sword_r.png", &imgsword[2]);
	}

	/*** various ***/
	PreLoad (PNG_VARIOUS, "no_other.png", &imgdother);
	PreLoad (PNG_VARIOUS, "no_jaffar.png", &imgdjaffar);
	PreLoad (PNG_VARIOUS, "no_guards.png", &imgdguards);
	PreLoad (PNG_VARIOUS, "prince_warning.png", &imgprincew);
	PreLoad (PNG_VARIOUS, "unknown.png", &imgunk[1]);
	PreLoad (PNG_VARIOUS, "sel_unknown.png", &imgunk[2]);
	PreLoad (PNG_BUTTONS, "up_0.png", &imgup_0);
	PreLoad (PNG_BUTTONS, "up_1.png", &imgup_1);
	PreLoad (PNG_BUTTONS, "down_0.png", &imgdown_0);
	PreLoad (PNG_BUTTONS, "down_1.png", &imgdown_1);
	PreLoad (PNG_BUTTONS, "left_0.png", &imgleft_0);
	PreLoad (PNG_BUTTONS, "left_1.png", &imgleft_1);
	PreLoad (PNG_BUTTONS, "right_0.png", &imgright_0);
	PreLoad (PNG_BUTTONS, "right_1.png", &imgright_1);
	PreLoad (PNG_BUTTONS, "up_down_no.png", &imgudno);
	PreLoad (PNG_BUTTONS, "left_right_no.png", &imglrno);
	PreLoad (PNG_BUTTONS, "up_down_no_nfo.png", &imgudnonfo);
	PreLoad (PNG_BUTTONS, "previous_on_0.png", &imgprevon_0);
	PreLoad (PNG_BUTTONS, "previous_on_1.png", &imgprevon_1);
	PreLoad (PNG_BUTTONS, "next_on_0.png", &imgnexton_0);
	PreLoad (PNG_BUTTONS, "next_on_1.png", &imgnexton_1);
	PreLoad (PNG_BUTTONS, "previous_off.png", &imgprevoff);
	PreLoad (PNG_BUTTONS, "next_off.png", &imgnextoff);
	PreLoad (PNG_VARIOUS, "distort.png", &imgdistort);
	if (iEditPoP == 1)
	{
		PreLoad (PNG_VARIOUS, "level_bar.png", &imgbar);
	} else {
		PreLoad (PNG_VARIOUS, "level_bar_pop2.png", &imgbar);
		PreLoad (PNG_VARIOUS, "guard_row.png", &imgguard_row);
		PreLoad (PNG_VARIOUS, "guard_entry.png", &imgguard_entry);
		PreLoad (PNG_VARIOUS, "faded_dynamic_guard.png", &imgfadeddg);
		PreLoad (PNG_VARIOUS, "faded_static_guard_left.png", &imgfadedsgl);
		PreLoad (PNG_VARIOUS, "faded_static_guard_right.png", &imgfadedsgr);
		PreLoad (PNG_VARIOUS, "X-coordinate_tooltip.png", &imgxtooltip);
		PreLoad (PNG_VARIOUS, "level_10_warning.png", &imgl10warn);
		PreLoad (PNG_VARIOUS, "guardwarn_rooftops.png", &imggw_rooftops);
		PreLoad (PNG_VARIOUS, "guardwarn_desert.png", &imggw_desert);
		PreLoad (PNG_VARIOUS, "guardwarn_caverns.png", &imggw_caverns);
		PreLoad (PNG_VARIOUS, "guardwarn_ruins.png", &imggw_ruins);
		PreLoad (PNG_VARIOUS, "guardwarn_temple.png", &imggw_temple);
		PreLoad (PNG_VARIOUS, "guardwarn_final.png", &imggw_final);
		PreLoad (PNG_VARIOUS, "guardwarn_dynamic.png", &imggw_dynamic);
		PreLoad (PNG_VARIOUS, "sel_back.png", &imgsb);
		PreLoad (PNG_VARIOUS, "hi_back.png", &imghb);

		PreLoad (PNG_INCOMING, "incomingl0.png", &imgincomingl[0]);
		PreLoad (PNG_INCOMING, "incomingl1.png", &imgincomingl[1]);
		PreLoad (PNG_INCOMING, "incomingl2.png", &imgincomingl[2]);
		PreLoad (PNG_INCOMING, "incomingl3.png", &imgincomingl[3]);
		PreLoad (PNG_INCOMING, "incomingl4.png", &imgincomingl[4]);
		PreLoad (PNG_INCOMING, "incomingl5.png", &imgincomingl[5]);
		PreLoad (PNG_INCOMING, "incomingl6.png", &imgincomingl[6]);
		PreLoad (PNG_INCOMING, "incomingl7.png", &imgincomingl[7]);
		PreLoad (PNG_INCOMING, "incomingl8.png", &imgincomingl[8]);
		PreLoad (PNG_INCOMING, "incomingl9.png", &imgincomingl[9]);
		PreLoad (PNG_INCOMING, "incomingr0.png", &imgincomingr[0]);
		PreLoad (PNG_INCOMING, "incomingr1.png", &imgincomingr[1]);
		PreLoad (PNG_INCOMING, "incomingr2.png", &imgincomingr[2]);
		PreLoad (PNG_INCOMING, "incomingr3.png", &imgincomingr[3]);
		PreLoad (PNG_INCOMING, "incomingr4.png", &imgincomingr[4]);
		PreLoad (PNG_INCOMING, "incomingr5.png", &imgincomingr[5]);
		PreLoad (PNG_INCOMING, "incomingr6.png", &imgincomingr[6]);
		PreLoad (PNG_INCOMING, "incomingr7.png", &imgincomingr[7]);
		PreLoad (PNG_INCOMING, "incomingr8.png", &imgincomingr[8]);
		PreLoad (PNG_INCOMING, "incomingr9.png", &imgincomingr[9]);
	}

	/*** extras ***/
	PreLoad (PNG_EXTRAS, "extras_0.png", &extras_0);
	PreLoad (PNG_EXTRAS, "extras_1.png", &extras_1);
	PreLoad (PNG_EXTRAS, "extras_2.png", &extras_2);
	PreLoad (PNG_EXTRAS, "extras_3.png", &extras_3);
	PreLoad (PNG_EXTRAS, "extras_4.png", &extras_4);
	PreLoad (PNG_EXTRAS, "extras_5.png", &extras_5);
	PreLoad (PNG_EXTRAS, "extras_6.png", &extras_6);
	PreLoad (PNG_EXTRAS, "extras_7.png", &extras_7);
	PreLoad (PNG_EXTRAS, "extras_8.png", &extras_8);

	PreLoad (PNG_BUTTONS, "rooms_on_0.png", &imgroomson_0);
	PreLoad (PNG_BUTTONS, "rooms_on_1.png", &imgroomson_1);
	PreLoad (PNG_BUTTONS, "rooms_off.png", &imgroomsoff);
	PreLoad (PNG_BUTTONS, "broken_rooms_on_0.png", &imgbroomson_0);
	PreLoad (PNG_BUTTONS, "broken_rooms_on_1.png", &imgbroomson_1);
	PreLoad (PNG_BUTTONS, "broken_rooms_off.png", &imgbroomsoff);
	/***/
	PreLoad (PNG_BUTTONS, "events_on_0.png", &imgeventson_0);
	PreLoad (PNG_BUTTONS, "events_on_1.png", &imgeventson_1);
	PreLoad (PNG_BUTTONS, "events_off.png", &imgeventsoff);
	PreLoad (PNG_BUTTONS, "save_on_0.png", &imgsaveon_0);
	PreLoad (PNG_BUTTONS, "save_on_1.png", &imgsaveon_1);
	PreLoad (PNG_BUTTONS, "save_off.png", &imgsaveoff);
	PreLoad (PNG_BUTTONS, "quit_0.png", &imgquit_0);
	PreLoad (PNG_BUTTONS, "quit_1.png", &imgquit_1);
	PreLoad (PNG_VARIOUS, "room_links.png", &imgrl);
	PreLoad (PNG_VARIOUS, "room_links_25-32.png", &imgrl2);
	PreLoad (PNG_VARIOUS, "broken_room_links.png", &imgbrl);
	PreLoad (PNG_VARIOUS, "broken_room_links_25-32.png", &imgbrl2);
	PreLoad (PNG_VARIOUS, "sel_room_current.png", &imgsrc);
	PreLoad (PNG_VARIOUS, "sel_room_start.png", &imgsrs);
	PreLoad (PNG_VARIOUS, "sel_room_moving.png", &imgsrm);
	PreLoad (PNG_VARIOUS, "sel_room_cross.png", &imgsrp);
	PreLoad (PNG_VARIOUS, "sel_room_broken.png", &imgsrb);

	if (iEditPoP == 1)
	{
		PreLoad (PNG_LIVING1, "kid_right.png", &imgkidr[1]);
		PreLoad (PNG_LIVING1, "sel_kid_right.png", &imgkidr[2]);
		PreLoad (PNG_LIVING1, "kid_left.png", &imgkidl[1]);
		PreLoad (PNG_LIVING1, "sel_kid_left.png", &imgkidl[2]);
	} else {
		PreLoad (PNG_LIVING2, "kid_right.png", &imgkidr[1]);
		PreLoad (PNG_LIVING2, "sel_kid_right.png", &imgkidr[2]);
		PreLoad (PNG_LIVING2, "kid_left.png", &imgkidl[1]);
		PreLoad (PNG_LIVING2, "sel_kid_left.png", &imgkidl[2]);
	}

	if (iEditPoP == 1)
	{
		PreLoad (PNG_VARIOUS, "events.png", &imgevents);
	} else {
		PreLoad (PNG_VARIOUS, "events_pop2.png", &imgevents);
	}
	PreLoad (PNG_VARIOUS, "sel_event.png", &imgsele);
	PreLoad (PNG_VARIOUS, "sel_level.png", &imgsell);

	/*** tiles screens ***/
	if (iEditPoP == 1)
	{
		PreLoad (PNG_VARIOUS, "dungeon.png", &imgdungeon);
		PreLoad (PNG_VARIOUS, "palace.png", &imgpalace);
	} else {
		PreLoad (PNG_VARIOUS, "oth.png", &imgoth);
		PreLoad (PNG_VARIOUS, "caverns.png", &imgcaverns);
		PreLoad (PNG_VARIOUS, "ruins.png", &imgruins);
		PreLoad (PNG_VARIOUS, "ruins_back.png", &imgruins_back);
		PreLoad (PNG_VARIOUS, "temple.png", &imgtemple);
		PreLoad (PNG_VARIOUS, "temple_back.png", &imgtemple_back);
		PreLoad (PNG_VARIOUS, "guards.png", &imgguards);
		PreLoad (PNG_VARIOUS, "music.png", &imgmusic);
		PreLoad (PNG_MUSIC, "music_note.png", &imgmusic_note);
		PreLoad (PNG_MUSIC, "music_all_0.png", &imgm_all_0);
		PreLoad (PNG_MUSIC, "music_all_1.png", &imgm_all_1);
		PreLoad (PNG_MUSIC, "music_none_0.png", &imgm_none_0);
		PreLoad (PNG_MUSIC, "music_none_1.png", &imgm_none_1);
		PreLoad (PNG_MUSIC, "music_left_0.png", &imgm_left_0);
		PreLoad (PNG_MUSIC, "music_left_1.png", &imgm_left_1);
		PreLoad (PNG_MUSIC, "music_right_0.png", &imgm_right_0);
		PreLoad (PNG_MUSIC, "music_right_1.png", &imgm_right_1);
		PreLoad (PNG_MUSIC, "music_top_0.png", &imgm_top_0);
		PreLoad (PNG_MUSIC, "music_top_1.png", &imgm_top_1);
		PreLoad (PNG_MUSIC, "music_middle_0.png", &imgm_middle_0);
		PreLoad (PNG_MUSIC, "music_middle_1.png", &imgm_middle_1);
		PreLoad (PNG_MUSIC, "music_bottom_0.png", &imgm_bottom_0);
		PreLoad (PNG_MUSIC, "music_bottom_1.png", &imgm_bottom_1);
		PreLoad (PNG_MUSIC, "music_invert_0.png", &imgm_invert_0);
		PreLoad (PNG_MUSIC, "music_invert_1.png", &imgm_invert_1);
	}

	if (iEditPoP == 2)
	{
		PreLoad (PNG_BACK, "PoP2_l1r1.png", &backl1r1);
		PreLoad (PNG_BACK, "PoP2_l1r2.png", &backl1r2);
		PreLoad (PNG_BACK, "PoP2_l1r3.png", &backl1r3);
		PreLoad (PNG_BACK, "PoP2_l1r4.png", &backl1r4);
		PreLoad (PNG_BACK, "PoP2_l1r5.png", &backl1r5);
		PreLoad (PNG_BACK, "PoP2_l1r10.png", &backl1r10);
		PreLoad (PNG_BACK, "PoP2_l1r11.png", &backl1r11);
		PreLoad (PNG_BACK, "PoP2_l1r12.png", &backl1r12);
		PreLoad (PNG_BACK, "PoP2_l1r15.png", &backl1r15);
		PreLoad (PNG_BACK, "PoP2_l1r16.png", &backl1r16);
		PreLoad (PNG_BACK, "PoP2_l1r19.png", &backl1r19);
		PreLoad (PNG_BACK, "PoP2_l2r1.png", &backl2r1);
		PreLoad (PNG_BACK, "PoP2_l2r2.png", &backl2r2);
		PreLoad (PNG_BACK, "PoP2_l2r3.png", &backl2r3);
		PreLoad (PNG_BACK, "PoP2_l14r1.png", &backl14r1);
		PreLoad (PNG_BACK, "PoP2_l14r2.png", &backl14r2);
		PreLoad (PNG_BACK, "PoP2_l14r3.png", &backl14r3);
		PreLoad (PNG_BACK, "PoP2_l14r4.png", &backl14r4);
		PreLoad (PNG_BACK, "PoP2_l14r5.png", &backl14r5);
		PreLoad (PNG_BACK, "PoP2_l14r6.png", &backl14r6);
		PreLoad (PNG_BACK, "PoP2_l14r7.png", &backl14r7);
		PreLoad (PNG_BACK, "PoP2_l14r8.png", &backl14r8);
		PreLoad (PNG_BACK, "PoP2_l6r27.png", &backl6r27);
		PreLoad (PNG_BACK, "PoP2_l9r2.png", &backl9r2);
		PreLoad (PNG_BACK, "PoP2_l9r11.png", &backl9r11);
		PreLoad (PNG_BACK, "PoP2_l9r12.png", &backl9r12);
		PreLoad (PNG_BACK, "PoP2_l9r13.png", &backl9r13);
		PreLoad (PNG_BACK, "PoP2_l9r14.png", &backl9r14);
		PreLoad (PNG_BACK, "PoP2_l9r15.png", &backl9r15);
		PreLoad (PNG_BACK, "PoP2_l9r16.png", &backl9r16);
		PreLoad (PNG_BACK, "PoP2_l13r4.png", &backl13r4);
		PreLoad (PNG_BACK, "PoP2_l8r9.png", &backl8r9);

		PreLoad (PNG_RUINSD, "crack1.png", &crack1[1]);
		PreLoad (PNG_RUINSD, "crack3.png", &crack3[1]);
		PreLoad (PNG_RUINSD, "crack4.png", &crack4[1]);
		PreLoad (PNG_RUINSD, "crack5.png", &crack5[1]);
		PreLoad (PNG_RUINSD, "crack6.png", &crack6[1]);
		PreLoad (PNG_RUINSD, "crack7.png", &crack7[1]);
		PreLoad (PNG_RUINSD, "crack8.png", &crack8[1]);
		PreLoad (PNG_RUINSD, "crack9.png", &crack9[1]);
		PreLoad (PNG_RUINSD, "crack10.png", &crack10[1]);
		PreLoad (PNG_RUINSD, "crack11.png", &crack11[1]);
		PreLoad (PNG_RUINSD, "crack12.png", &crack12[1]);
		PreLoad (PNG_RUINSD, "symbol16.png", &symbol16[1]);
		PreLoad (PNG_RUINSD, "symbol32.png", &symbol32[1]);
		PreLoad (PNG_RUINSD, "symbol48.png", &symbol48[1]);
		PreLoad (PNG_RUINSD, "sel_crack1.png", &crack1[2]);
		PreLoad (PNG_RUINSD, "sel_crack3.png", &crack3[2]);
		PreLoad (PNG_RUINSD, "sel_crack4.png", &crack4[2]);
		PreLoad (PNG_RUINSD, "sel_crack5.png", &crack5[2]);
		PreLoad (PNG_RUINSD, "sel_crack6.png", &crack6[2]);
		PreLoad (PNG_RUINSD, "sel_crack7.png", &crack7[2]);
		PreLoad (PNG_RUINSD, "sel_crack8.png", &crack8[2]);
		PreLoad (PNG_RUINSD, "sel_crack9.png", &crack9[2]);
		PreLoad (PNG_RUINSD, "sel_crack10.png", &crack10[2]);
		PreLoad (PNG_RUINSD, "sel_crack11.png", &crack11[2]);
		PreLoad (PNG_RUINSD, "sel_crack12.png", &crack12[2]);
		PreLoad (PNG_RUINSD, "sel_symbol16.png", &symbol16[2]);
		PreLoad (PNG_RUINSD, "sel_symbol32.png", &symbol32[2]);
		PreLoad (PNG_RUINSD, "sel_symbol48.png", &symbol48[2]);

		PreLoad (PNG_CAVERNSD, "bottom1.png", &bottom1[1]);
		PreLoad (PNG_CAVERNSD, "bottom2.png", &bottom2[1]);
		PreLoad (PNG_CAVERNSD, "bottom3.png", &bottom3[1]);
		PreLoad (PNG_CAVERNSD, "bottom4.png", &bottom4[1]);
		PreLoad (PNG_CAVERNSD, "bottom5.png", &bottom5[1]);
		PreLoad (PNG_CAVERNSD, "bottom6.png", &bottom6[1]);
		PreLoad (PNG_CAVERNSD, "bottom7.png", &bottom7[1]);
		PreLoad (PNG_CAVERNSD, "stalactite8.png", &stalactite8[1]);
		PreLoad (PNG_CAVERNSD, "stalactite16.png", &stalactite16[1]);
		PreLoad (PNG_CAVERNSD, "stalactite24.png", &stalactite24[1]);
		PreLoad (PNG_CAVERNSD, "stalactite32.png", &stalactite32[1]);
		PreLoad (PNG_CAVERNSD, "stalactite40.png", &stalactite40[1]);
		PreLoad (PNG_CAVERNSD, "stalactite48.png", &stalactite48[1]);
		PreLoad (PNG_CAVERNSD, "stalactite56.png", &stalactite56[1]);
		PreLoad (PNG_CAVERNSD, "sel_bottom1.png", &bottom1[2]);
		PreLoad (PNG_CAVERNSD, "sel_bottom2.png", &bottom2[2]);
		PreLoad (PNG_CAVERNSD, "sel_bottom3.png", &bottom3[2]);
		PreLoad (PNG_CAVERNSD, "sel_bottom4.png", &bottom4[2]);
		PreLoad (PNG_CAVERNSD, "sel_bottom5.png", &bottom5[2]);
		PreLoad (PNG_CAVERNSD, "sel_bottom6.png", &bottom6[2]);
		PreLoad (PNG_CAVERNSD, "sel_bottom7.png", &bottom7[2]);
		PreLoad (PNG_CAVERNSD, "sel_stalactite8.png", &stalactite8[2]);
		PreLoad (PNG_CAVERNSD, "sel_stalactite16.png", &stalactite16[2]);
		PreLoad (PNG_CAVERNSD, "sel_stalactite24.png", &stalactite24[2]);
		PreLoad (PNG_CAVERNSD, "sel_stalactite32.png", &stalactite32[2]);
		PreLoad (PNG_CAVERNSD, "sel_stalactite40.png", &stalactite40[2]);
		PreLoad (PNG_CAVERNSD, "sel_stalactite48.png", &stalactite48[2]);
		PreLoad (PNG_CAVERNSD, "sel_stalactite56.png", &stalactite56[2]);

		PreLoad (PNG_TEMPLEB, "0.png", &templeb[0]);
		PreLoad (PNG_TEMPLEB, "1.png", &templeb[1]);
		PreLoad (PNG_TEMPLEB, "2.png", &templeb[2]);
		PreLoad (PNG_TEMPLEB, "3.png", &templeb[3]);
		PreLoad (PNG_TEMPLEB, "4.png", &templeb[4]);
		PreLoad (PNG_TEMPLEB, "5.png", &templeb[5]);
		PreLoad (PNG_TEMPLEB, "6.png", &templeb[6]);
		PreLoad (PNG_TEMPLEB, "7.png", &templeb[7]);
		PreLoad (PNG_TEMPLEB, "8.png", &templeb[8]);
		PreLoad (PNG_TEMPLEB, "9.png", &templeb[9]);
		PreLoad (PNG_TEMPLEB, "10.png", &templeb[10]);
		PreLoad (PNG_TEMPLEB, "11.png", &templeb[11]);
		PreLoad (PNG_TEMPLEB, "12.png", &templeb[12]);
		PreLoad (PNG_TEMPLEB, "13.png", &templeb[13]);
		PreLoad (PNG_TEMPLEB, "14.png", &templeb[14]);
		PreLoad (PNG_TEMPLEB, "15.png", &templeb[15]);
		PreLoad (PNG_TEMPLEB, "16.png", &templeb[16]);
		PreLoad (PNG_TEMPLEB, "17.png", &templeb[17]);
		PreLoad (PNG_TEMPLEB, "18.png", &templeb[18]);
		PreLoad (PNG_TEMPLEB, "19.png", &templeb[19]);
		PreLoad (PNG_TEMPLEB, "20.png", &templeb[20]);
		PreLoad (PNG_TEMPLEB, "21.png", &templeb[21]);
		PreLoad (PNG_TEMPLEB, "22.png", &templeb[22]);
		PreLoad (PNG_TEMPLEB, "23.png", &templeb[23]);
		PreLoad (PNG_TEMPLEB, "24.png", &templeb[24]);
		PreLoad (PNG_TEMPLEB, "25.png", &templeb[25]);
		PreLoad (PNG_TEMPLEB, "26.png", &templeb[26]);
		PreLoad (PNG_TEMPLEB, "27.png", &templeb[27]);
		PreLoad (PNG_TEMPLEB, "28.png", &templeb[28]);
		PreLoad (PNG_TEMPLEB, "29.png", &templeb[29]);
		PreLoad (PNG_TEMPLEB, "30.png", &templeb[30]);
		PreLoad (PNG_TEMPLEB, "31.png", &templeb[31]);

		iTempleBHeight[0] = 1;
		iTempleBHeight[1] = 1;
		iTempleBHeight[2] = 1;
		iTempleBHeight[3] = 1;
		iTempleBHeight[4] = 1;
		iTempleBHeight[5] = 1;
		iTempleBHeight[6] = 1;
		iTempleBHeight[7] = 1;
		iTempleBHeight[8] = 1;
		iTempleBHeight[9] = 1;
		iTempleBHeight[10] = 1;
		iTempleBHeight[11] = 1;
		iTempleBHeight[12] = 1;
		iTempleBHeight[13] = 1;
		iTempleBHeight[14] = 1;
		iTempleBHeight[15] = 1;
		iTempleBHeight[16] = 1;
		iTempleBHeight[17] = 1;
		iTempleBHeight[18] = 1;
		iTempleBHeight[19] = 2;
		iTempleBHeight[20] = 2;
		iTempleBHeight[21] = 1;
		iTempleBHeight[22] = 1;
		iTempleBHeight[23] = 1;
		iTempleBHeight[24] = 1;
		iTempleBHeight[25] = 1;
		iTempleBHeight[26] = 1;
		iTempleBHeight[27] = 1;
		iTempleBHeight[28] = 1;
		iTempleBHeight[29] = 1;
		iTempleBHeight[30] = 1;
		iTempleBHeight[31] = 1;

		PreLoad (PNG_RUINSB, "0.png", &ruinsb[0]);
		PreLoad (PNG_RUINSB, "1.png", &ruinsb[1]);
		PreLoad (PNG_RUINSB, "2.png", &ruinsb[2]);
		PreLoad (PNG_RUINSB, "3.png", &ruinsb[3]);
		PreLoad (PNG_RUINSB, "4.png", &ruinsb[4]);
		PreLoad (PNG_RUINSB, "5.png", &ruinsb[5]);
		PreLoad (PNG_RUINSB, "6.png", &ruinsb[6]);
		PreLoad (PNG_RUINSB, "7.png", &ruinsb[7]);
		PreLoad (PNG_RUINSB, "8.png", &ruinsb[8]);
		PreLoad (PNG_RUINSB, "9.png", &ruinsb[9]);
		PreLoad (PNG_RUINSB, "10.png", &ruinsb[10]);
		PreLoad (PNG_RUINSB, "12.png", &ruinsb[12]);
		PreLoad (PNG_RUINSB, "13.png", &ruinsb[13]);
		PreLoad (PNG_RUINSB, "14.png", &ruinsb[14]);
		PreLoad (PNG_RUINSB, "15.png", &ruinsb[15]);
		PreLoad (PNG_RUINSB, "16.png", &ruinsb[16]);
		PreLoad (PNG_RUINSB, "17.png", &ruinsb[17]);
		PreLoad (PNG_RUINSB, "18.png", &ruinsb[18]);
		PreLoad (PNG_RUINSB, "19.png", &ruinsb[19]);
		PreLoad (PNG_RUINSB, "20.png", &ruinsb[20]);
		PreLoad (PNG_RUINSB, "22.png", &ruinsb[22]);
		PreLoad (PNG_RUINSB, "23.png", &ruinsb[23]);
		PreLoad (PNG_RUINSB, "24.png", &ruinsb[24]);
		PreLoad (PNG_RUINSB, "25.png", &ruinsb[25]);
		PreLoad (PNG_RUINSB, "26.png", &ruinsb[26]);
		PreLoad (PNG_RUINSB, "30.png", &ruinsb[30]);
		PreLoad (PNG_RUINSB, "31.png", &ruinsb[31]);

		iRuinsBHeight[0] = 1;
		iRuinsBHeight[1] = 2;
		iRuinsBHeight[2] = 2;
		iRuinsBHeight[3] = 2;
		iRuinsBHeight[4] = 2;
		iRuinsBHeight[5] = 2;
		iRuinsBHeight[6] = 3;
		iRuinsBHeight[7] = 2;
		iRuinsBHeight[8] = 2;
		iRuinsBHeight[9] = 1;
		iRuinsBHeight[10] = 1;
		iRuinsBHeight[12] = 1;
		iRuinsBHeight[13] = 1;
		iRuinsBHeight[14] = 1;
		iRuinsBHeight[15] = 1;
		iRuinsBHeight[16] = 1;
		iRuinsBHeight[17] = 1;
		iRuinsBHeight[18] = 1;
		iRuinsBHeight[19] = 1;
		iRuinsBHeight[20] = 1;
		iRuinsBHeight[22] = 1;
		iRuinsBHeight[23] = 1;
		iRuinsBHeight[24] = 1;
		iRuinsBHeight[25] = 1;
		iRuinsBHeight[26] = 1;
		iRuinsBHeight[30] = 1;
		iRuinsBHeight[31] = 1;

		PreLoad (PNG_VARIOUS, "level_10_horse.png", &imghorse);
		PreLoad (PNG_VARIOUS, "level_6_arrow.png", &imgarrow);
		PreLoad (PNG_VARIOUS, "invisible.png", &imginvisible);
	}

	PreLoad (PNG_BUTTONS, "close_0.png", &imgclose_0);
	PreLoad (PNG_BUTTONS, "close_1.png", &imgclose_1);
	if (iEditPoP == 2)
	{
		PreLoad (PNG_BUTTONS, "close_big_0.png", &imgclosebig_0);
		PreLoad (PNG_BUTTONS, "close_big_1.png", &imgclosebig_1);
		PreLoad (PNG_BUTTONS, "background_0.png", &imgback_0);
		PreLoad (PNG_BUTTONS, "background_1.png", &imgback_1);
		PreLoad (PNG_BUTTONS, "guards_0.png", &imgguards_0);
		PreLoad (PNG_BUTTONS, "guards_1.png", &imgguards_1);
		PreLoad (PNG_BUTTONS, "music_0.png", &imgmusic_0);
		PreLoad (PNG_BUTTONS, "music_1.png", &imgmusic_1);
		PreLoad (PNG_BUTTONS, "edit_event_0.png", &imgee_0);
		PreLoad (PNG_BUTTONS, "edit_event_1.png", &imgee_1);
		PreLoad (PNG_BUTTONS, "end_chain_0.png", &imgec_0);
		PreLoad (PNG_BUTTONS, "end_chain_1.png", &imgec_1);
	}
	PreLoad (PNG_VARIOUS, "border_big.png", &imgborderb);
	PreLoad (PNG_VARIOUS, "border_small.png", &imgborders);
	PreLoad (PNG_VARIOUS, "border_small_live.png", &imgbordersl);
	PreLoad (PNG_VARIOUS, "border_big_live.png", &imgborderbl);

	/*** custom ***/
	PreLoad (PNG_BUTTONS, "custom_active_0.png", &imgcustoma0);
	PreLoad (PNG_BUTTONS, "custom_inactive_0.png", &imgcustomi0);
	PreLoad (PNG_BUTTONS, "custom_active_1.png", &imgcustoma1);
	PreLoad (PNG_BUTTONS, "custom_inactive_1.png", &imgcustomi1);
	PreLoad (PNG_VARIOUS, "custom.png", &imgcustom);
	PreLoad (PNG_BUTTONS, "custom_0.png", &imgcustom_0);
	PreLoad (PNG_BUTTONS, "custom_1.png", &imgcustom_1);

	/*** popup things ***/
	PreLoad (PNG_VARIOUS, "faded_l.png", &imgfadedl);
	PreLoad (PNG_VARIOUS, "popup.png", &imgpopup);
	PreLoad (PNG_BUTTONS, "OK.png", &imgok[1]);
	PreLoad (PNG_BUTTONS, "sel_OK.png", &imgok[2]);
	PreLoad (PNG_VARIOUS, "popup_yn.png", &imgpopup_yn);
	PreLoad (PNG_BUTTONS, "Yes.png", &imgyes[1]);
	PreLoad (PNG_BUTTONS, "sel_Yes.png", &imgyes[2]);
	PreLoad (PNG_BUTTONS, "No.png", &imgno[1]);
	PreLoad (PNG_BUTTONS, "sel_No.png", &imgno[2]);
	PreLoad (PNG_VARIOUS, "help.png", &imghelp);

	/*** faded tiles ***/
	PreLoad (PNG_VARIOUS, "faded_s.png", &imgfadeds);

	/*** room thingies, 14x14 ***/
	PreLoad (PNG_ROOMS, "room1.png", &imgroom1);
	PreLoad (PNG_ROOMS, "room2.png", &imgroom2);
	PreLoad (PNG_ROOMS, "room3.png", &imgroom3);
	PreLoad (PNG_ROOMS, "room4.png", &imgroom4);
	PreLoad (PNG_ROOMS, "room5.png", &imgroom5);
	PreLoad (PNG_ROOMS, "room6.png", &imgroom6);
	PreLoad (PNG_ROOMS, "room7.png", &imgroom7);
	PreLoad (PNG_ROOMS, "room8.png", &imgroom8);
	PreLoad (PNG_ROOMS, "room9.png", &imgroom9);
	PreLoad (PNG_ROOMS, "room10.png", &imgroom10);
	PreLoad (PNG_ROOMS, "room11.png", &imgroom11);
	PreLoad (PNG_ROOMS, "room12.png", &imgroom12);
	PreLoad (PNG_ROOMS, "room13.png", &imgroom13);
	PreLoad (PNG_ROOMS, "room14.png", &imgroom14);
	PreLoad (PNG_ROOMS, "room15.png", &imgroom15);
	PreLoad (PNG_ROOMS, "room16.png", &imgroom16);
	PreLoad (PNG_ROOMS, "room17.png", &imgroom17);
	PreLoad (PNG_ROOMS, "room18.png", &imgroom18);
	PreLoad (PNG_ROOMS, "room19.png", &imgroom19);
	PreLoad (PNG_ROOMS, "room20.png", &imgroom20);
	PreLoad (PNG_ROOMS, "room21.png", &imgroom21);
	PreLoad (PNG_ROOMS, "room22.png", &imgroom22);
	PreLoad (PNG_ROOMS, "room23.png", &imgroom23);
	PreLoad (PNG_ROOMS, "room24.png", &imgroom24);
	PreLoad (PNG_ROOMS, "room25.png", &imgroom25);
	PreLoad (PNG_ROOMS, "room26.png", &imgroom26);
	PreLoad (PNG_ROOMS, "room27.png", &imgroom27);
	PreLoad (PNG_ROOMS, "room28.png", &imgroom28);
	PreLoad (PNG_ROOMS, "room29.png", &imgroom29);
	PreLoad (PNG_ROOMS, "room30.png", &imgroom30);
	PreLoad (PNG_ROOMS, "room31.png", &imgroom31);
	PreLoad (PNG_ROOMS, "room32.png", &imgroom32);

	if (iDebug == 1)
		{ printf ("[ INFO ] Preloaded images: %i\n", iPreLoaded); }

	iDownAt = 0;
	iSelected = 1; /*** Start with the upper left selected. ***/
	iScreen = 1;

	if (iEditPoP == 1)
	{
		PoP1Basics();
	} else {
		PoP2Basics();
	}
	if (iDebug == 1)
		printf ("[ INFO ] Starting in level: %i\n", iStartLevel);

	LoadLevel (iStartLevel);
	iChangeEvent = 0;
	iChangeJumps = 3;
	iChangeFrame = 0;
	iChangePercent = 50;
	iChangeWall = 1;
	iFlameFrameDP = 1;
	iChomperFrameDP = 1;
	iChomped = 0;
	iSwordFrameDP = 1;
	iFlameFrameC = 1;
	iFlameFrameT = 1;
	oldticks = 0;
	iGuardTooltip = 0;
	iLvl10Warning = 0;

	ShowScreen (iScreen);
	InitPopUp (1, 1);
	while (1)
	{
		if (iNoAnim == 0)
		{
			/*** This is for the animation; 12 fps (1000/83) is fine for PoP. ***/
			newticks = SDL_GetTicks();
			if (newticks > oldticks + 83)
			{
				iFlameFrameDP++;
				if (iFlameFrameDP == 10) { iFlameFrameDP = 1; }
				iChomperFrameDP++;
				if (iChomperFrameDP == 14) { iChomperFrameDP = 1; }
				iSwordFrameDP++;
				if (iSwordFrameDP == 49) { iSwordFrameDP = 1; }
				iFlameFrameC++;
				if (iFlameFrameC == 9) { iFlameFrameC = 1; }
				iFlameFrameT++;
				if (iFlameFrameT == 5) { iFlameFrameT = 1; }
				ShowScreen (iScreen);
				oldticks = newticks;
			}
		}

		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN: /*** http://wiki.libsdl.org/SDLKeycodeLookup ***/
					switch (event.key.keysym.sym)
					{
						case SDLK_F1: Help (iScreen); break;
						case SDLK_LEFTBRACKET:
							switch (iScreen)
							{
								case 2:
									if (iBrokenRoomLinks == 0)
									{
										iMovingNewBusy = 0;
										switch (iMovingRoom)
										{
											case 0: iMovingRoom = iRooms; break;
											case 1: iMovingRoom = iRooms; break;
											default: iMovingRoom--; break;
										}
									}
									break;
								case 3:
									if (iEditPoP == 1)
									{
										iEventRoom = EventInfo (iChangeEvent, 1);
										if (iEventRoom > 1)
										{
											EventRoom (iEventRoom - 1);
										} else {
											EventRoom (24);
										}
									} else {
										if (iPoP2DoorRoom[iChangeEvent] > 1)
										{
											iPoP2DoorRoom[iChangeEvent]--;
										} else {
											iPoP2DoorRoom[iChangeEvent] = 32;
										}
										PlaySound ("wav/check_box.wav");
										iChanged++;
									}
									break;
							}
							break;
						case SDLK_RIGHTBRACKET:
							switch (iScreen)
							{
								case 2:
									if (iBrokenRoomLinks == 0)
									{
										iMovingNewBusy = 0;
										switch (iMovingRoom)
										{
											case 0: iMovingRoom = 1; break;
											case 24:
												if (iEditPoP == 1) { iMovingRoom = 1; }
													else { iMovingRoom++; } break;
											case 32: iMovingRoom = 1; break;
											default: iMovingRoom++; break;
										}
									}
									break;
								case 3:
									if (iEditPoP == 1)
									{
										iEventRoom = EventInfo (iChangeEvent, 1);
										if (iEventRoom < 24)
										{
											EventRoom (iEventRoom + 1);
										} else {
											EventRoom (1);
										}
									} else {
										if (iPoP2DoorRoom[iChangeEvent] < 32)
										{
											iPoP2DoorRoom[iChangeEvent]++;
										} else {
											iPoP2DoorRoom[iChangeEvent] = 1;
										}
										PlaySound ("wav/check_box.wav");
										iChanged++;
									}
									break;
							}
							break;
						case SDLK_d:
							RunLevel ((int)luLevelNr);
							break;
						case SDLK_SLASH:
						case SDLK_QUESTION:
							if (iScreen == 1)
							{
								for (iTemp = 1; iTemp <= 30; iTemp++)
								{
									if (cCurType == 'o')
										{ SetLocation (iCurRoom, iTemp, 0, 0, -1, -1, -1); }
											else { SetLocation (iCurRoom, iTemp, 0, 0, 0, 0, 0); }
								}
								if (iEditPoP == 1)
								{
									sGuardLocations[iCurRoom - 1] = 30;
								} else {
									for (iTile = 0; iTile < 30; iTile++)
										{ sMusic[((iCurRoom - 1) * 30) + iTile] = 0; }
									iStaticGuards_Amount[iCurRoom - 1] = 0;
									iDynamicGuards_Sets[iCurRoom - 1] = 0;
								}
								iChanged++;
								PlaySound ("wav/ok_close.wav");
							}
							break;
						case SDLK_BACKSLASH:
							if (iScreen == 1)
							{
								for (iTemp2 = 1; iTemp2 <= iRooms; iTemp2++)
								{
									for (iTemp = 1; iTemp <= 30; iTemp++)
									{
										UseTile (-1, iTemp, iTemp2);
									}
								}
								iChanged++;
								PlaySound ("wav/ok_close.wav");
							}
							break;
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
							switch (iScreen)
							{
								case 1:
									ChangePos (iSelected, screen);
									break;
								case 2:
									if (iBrokenRoomLinks == 0)
									{
										if (iMovingRoom != 0)
										{
											if (iRoomArray[iMovingNewX][iMovingNewY] == 0)
											{
												RemoveOldRoom();
												AddNewRoom (iMovingNewX, iMovingNewY, iMovingRoom);
												iChanged++;
											}
											iMovingRoom = 0; iMovingNewBusy = 0;
										}
									} else {
										iRoomConnections[iChangingBrokenRoom]
											[iChangingBrokenSide]++;
										if (iRoomConnections[iChangingBrokenRoom]
											[iChangingBrokenSide] == iRooms + 1)
										{
											iRoomConnections[iChangingBrokenRoom]
												[iChangingBrokenSide] = 0;
										}
										iChanged++;
										iBrokenRoomLinks = BrokenRoomLinks (iRoomConnections, 0);
										PlaySound ("wav/hum_adj.wav");
									}
									break;
							}
							break;
						case SDLK_BACKSPACE:
							if ((iScreen == 2) && (iBrokenRoomLinks == 1))
							{
								iRoomConnections[iChangingBrokenRoom]
									[iChangingBrokenSide]--;
								if (iRoomConnections[iChangingBrokenRoom]
									[iChangingBrokenSide] == -1)
								{
									iRoomConnections[iChangingBrokenRoom]
										[iChangingBrokenSide] = iRooms;
								}
								iChanged++;
								iBrokenRoomLinks = BrokenRoomLinks (iRoomConnections, 0);
								PlaySound ("wav/hum_adj.wav");
							}
							break;
						case SDLK_ESCAPE:
						case SDLK_q:
							switch (iScreen)
							{
								case 1:
									Quit(); break;
								case 2:
									iBrokenRoomLinks = BrokenRoomLinks (iRoomConnections, 0);
									/*** no break ***/
								case 3:
									iScreen = 1; break;
							}
							break;
						case SDLK_LEFT:
							if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								switch (iScreen)
								{
									case 1:
										if (iRoomConnections[iCurRoom][1] != 0)
										{
											iCurRoom = iRoomConnections[iCurRoom][1];
											PlaySound ("wav/scroll.wav");
										}
										break;
									case 3:
										ChangeEvent (-1, 0);
										break;
								}
							} else if ((event.key.keysym.mod & KMOD_LCTRL) ||
								(event.key.keysym.mod & KMOD_RCTRL))
							{
								if (iScreen == 3)
								{
									ChangeEvent (-10, 0);
								}
							} else {
								switch (iScreen)
								{
									case 1:
										iSelected--;
										switch (iSelected)
										{
											case 0: iSelected = 10; break;
											case 10: iSelected = 20; break;
											case 20: iSelected = 30; break;
										}
										break;
									case 2:
										if (iBrokenRoomLinks == 0)
										{
											if (iMovingRoom != 0)
											{
												if (iMovingNewX != 1) { iMovingNewX--; }
													else { iMovingNewX = 25; }
												PlaySound ("wav/cross.wav");
											}
										} else {
											if (iChangingBrokenSide != 1)
											{
												iChangingBrokenSide = 1;
											} else {
												if (iEditPoP == 1)
												{
													switch (iChangingBrokenRoom)
													{
														case 1: iChangingBrokenRoom = 4; break;
														case 5: iChangingBrokenRoom = 8; break;
														case 9: iChangingBrokenRoom = 12; break;
														case 13: iChangingBrokenRoom = 16; break;
														case 17: iChangingBrokenRoom = 20; break;
														case 21: iChangingBrokenRoom = 24; break;
														default: iChangingBrokenRoom--; break;
													}
												} else {
													switch (iChangingBrokenRoom)
													{
														case 1: iChangingBrokenRoom = 4; break;
														case 5: iChangingBrokenRoom = 8; break;
														case 9: iChangingBrokenRoom = 12; break;
														case 13: iChangingBrokenRoom = 16; break;
														case 17: iChangingBrokenRoom = 28; break;
														case 21: iChangingBrokenRoom = 32; break;
														case 25: iChangingBrokenRoom = 20; break;
														case 29: iChangingBrokenRoom = 24; break;
														default: iChangingBrokenRoom--; break;
													}
												}
											}
										}
										break;
									case 3:
										if (iEditPoP == 1)
										{
											iEventDoor = EventInfo (iChangeEvent, 2);
											switch (iEventDoor)
											{
												case 1: iEventDoor = 10; break;
												case 11: iEventDoor = 20; break;
												case 21: iEventDoor = 30; break;
												default: iEventDoor--; break;
											}
											if (iEventDoor > 20) { EventDoor (iEventDoor - 20, 3); }
												else if (iEventDoor > 10)
													{ EventDoor (iEventDoor - 10, 2); }
												else { EventDoor (iEventDoor, 1); }
										} else {
											iEventDoor = iPoP2DoorLocation[iChangeEvent];
											switch (iEventDoor)
											{
												case 0: iEventDoor = 9; break;
												case 10: iEventDoor = 19; break;
												case 20: iEventDoor = 29; break;
												default: iEventDoor--; break;
											}
											iPoP2DoorLocation[iChangeEvent] = iEventDoor;
											PlaySound ("wav/check_box.wav");
											iChanged++;
										}
										break;
								}
							}
							break;
						case SDLK_RIGHT:
							if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								switch (iScreen)
								{
									case 1:
										if (iRoomConnections[iCurRoom][2] != 0)
										{
											iCurRoom = iRoomConnections[iCurRoom][2];
											PlaySound ("wav/scroll.wav");
										}
										break;
									case 3:
										ChangeEvent (1, 0);
										break;
								}
							} else if ((event.key.keysym.mod & KMOD_LCTRL) ||
								(event.key.keysym.mod & KMOD_RCTRL))
							{
								if (iScreen == 3)
								{
									ChangeEvent (10, 0);
								}
							} else {
								switch (iScreen)
								{
									case 1:
										iSelected++;
										switch (iSelected)
										{
											case 11: iSelected = 1; break;
											case 21: iSelected = 11; break;
											case 31: iSelected = 21; break;
										}
										break;
									case 2:
										if (iBrokenRoomLinks == 0)
										{
											if (iMovingRoom != 0)
											{
												if (iMovingNewX != 25) { iMovingNewX++; }
													else { iMovingNewX = 1; }
												PlaySound ("wav/cross.wav");
											}
										} else {
											if (iChangingBrokenSide != 2)
											{
												iChangingBrokenSide = 2;
											} else {
												if (iEditPoP == 1)
												{
													switch (iChangingBrokenRoom)
													{
														case 4: iChangingBrokenRoom = 1; break;
														case 8: iChangingBrokenRoom = 5; break;
														case 12: iChangingBrokenRoom = 9; break;
														case 16: iChangingBrokenRoom = 13; break;
														case 20: iChangingBrokenRoom = 17; break;
														case 24: iChangingBrokenRoom = 21; break;
														default: iChangingBrokenRoom++; break;
													}
												} else {
													switch (iChangingBrokenRoom)
													{
														case 4: iChangingBrokenRoom = 1; break;
														case 8: iChangingBrokenRoom = 5; break;
														case 12: iChangingBrokenRoom = 9; break;
														case 16: iChangingBrokenRoom = 13; break;
														case 20: iChangingBrokenRoom = 25; break;
														case 24: iChangingBrokenRoom = 29; break;
														case 28: iChangingBrokenRoom = 17; break;
														case 32: iChangingBrokenRoom = 21; break;
														default: iChangingBrokenRoom++; break;
													}
												}
											}
										}
										break;
									case 3:
										if (iEditPoP == 1)
										{
											iEventDoor = EventInfo (iChangeEvent, 2);
											switch (iEventDoor)
											{
												case 10: iEventDoor = 1; break;
												case 20: iEventDoor = 11; break;
												case 30: iEventDoor = 21; break;
												default: iEventDoor++; break;
											}
											if (iEventDoor > 20) { EventDoor (iEventDoor - 20, 3); }
												else if (iEventDoor > 10)
													{ EventDoor (iEventDoor - 10, 2); }
												else { EventDoor (iEventDoor, 1); }
										} else {
											iEventDoor = iPoP2DoorLocation[iChangeEvent];
											switch (iEventDoor)
											{
												case 9: iEventDoor = 0; break;
												case 19: iEventDoor = 10; break;
												case 29: iEventDoor = 20; break;
												default: iEventDoor++; break;
											}
											iPoP2DoorLocation[iChangeEvent] = iEventDoor;
											PlaySound ("wav/check_box.wav");
											iChanged++;
										}
										break;
								}
							}
							break;
						case SDLK_UP:
							if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								if (iScreen == 1)
								{
									if (iRoomConnections[iCurRoom][3] != 0)
									{
										iCurRoom = iRoomConnections[iCurRoom][3];
										PlaySound ("wav/scroll.wav");
									}
								}
							} else {
								switch (iScreen)
								{
									case 1:
										if (iSelected > 10) { iSelected-=10; }
											else { iSelected+=20; }
										break;
									case 2:
										if (iBrokenRoomLinks == 0)
										{
											if (iMovingRoom != 0)
											{
												if (iMovingNewY != 1) { iMovingNewY--; }
													else { iMovingNewY = 24; }
												PlaySound ("wav/cross.wav");
											}
										} else {
											if (iChangingBrokenSide != 3)
											{
												iChangingBrokenSide = 3;
											} else {
												switch (iChangingBrokenRoom)
												{
													case 1: iChangingBrokenRoom = 21; break;
													case 2: iChangingBrokenRoom = 22; break;
													case 3: iChangingBrokenRoom = 23; break;
													case 4: iChangingBrokenRoom = 24; break;
													case 25: iChangingBrokenRoom = 29; break;
													case 26: iChangingBrokenRoom = 30; break;
													case 27: iChangingBrokenRoom = 31; break;
													case 28: iChangingBrokenRoom = 32; break;
													default: iChangingBrokenRoom -= 4; break;
												}
											}
										}
										break;
									case 3:
										if (iEditPoP == 1)
										{
											iEventDoor = EventInfo (iChangeEvent, 2);
											if (iEventDoor > 10) { iEventDoor-=10; }
												else { iEventDoor+=20; }
											if (iEventDoor > 20) { EventDoor (iEventDoor - 20, 3); }
												else if (iEventDoor > 10)
													{ EventDoor (iEventDoor - 10, 2); }
												else { EventDoor (iEventDoor, 1); }
										} else {
											if (iPoP2DoorLocation[iChangeEvent] > 9)
												{ iPoP2DoorLocation[iChangeEvent]-=10; }
													else { iPoP2DoorLocation[iChangeEvent]+=20; }
											PlaySound ("wav/check_box.wav");
											iChanged++;
										}
										break;
								}
							}
							break;
						case SDLK_DOWN:
							if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								if (iScreen == 1)
								{
									if (iRoomConnections[iCurRoom][4] != 0)
									{
										iCurRoom = iRoomConnections[iCurRoom][4];
										PlaySound ("wav/scroll.wav");
									}
								}
							} else {
								switch (iScreen)
								{
									case 1:
										if (iSelected <= 20) { iSelected+=10; }
											else { iSelected-=20; }
										break;
									case 2:
										if (iBrokenRoomLinks == 0)
										{
											if (iMovingRoom != 0)
											{
												if (iMovingNewY != 24) { iMovingNewY++; }
													else { iMovingNewY = 1; }
												PlaySound ("wav/cross.wav");
											}
										} else {
											if (iChangingBrokenSide != 4)
											{
												iChangingBrokenSide = 4;
											} else {
												switch (iChangingBrokenRoom)
												{
													case 21: iChangingBrokenRoom = 1; break;
													case 22: iChangingBrokenRoom = 2; break;
													case 23: iChangingBrokenRoom = 3; break;
													case 24: iChangingBrokenRoom = 4; break;
													case 29: iChangingBrokenRoom = 25; break;
													case 30: iChangingBrokenRoom = 26; break;
													case 31: iChangingBrokenRoom = 27; break;
													case 32: iChangingBrokenRoom = 28; break;
													default: iChangingBrokenRoom += 4; break;
												}
											}
										}
										break;
									case 3:
										if (iEditPoP == 1)
										{
											iEventDoor = EventInfo (iChangeEvent, 2);
											if (iEventDoor <= 20) { iEventDoor+=10; }
												else { iEventDoor-=20; }
											if (iEventDoor > 20) { EventDoor (iEventDoor - 20, 3); }
												else if (iEventDoor > 10)
													{ EventDoor (iEventDoor - 10, 2); }
												else { EventDoor (iEventDoor, 1); }
										} else {
											if (iPoP2DoorLocation[iChangeEvent] <= 19)
												{ iPoP2DoorLocation[iChangeEvent]+=10; }
													else { iPoP2DoorLocation[iChangeEvent]-=20; }
											PlaySound ("wav/check_box.wav");
											iChanged++;
										}
										break;
								}
							}
							break;
						case SDLK_y:
							if ((iEditPoP == 1) && (iScreen == 3)) { EventNext (0); }
							break;
						case SDLK_n:
							if ((iEditPoP == 1) && (iScreen == 3)) { EventNext (1); }
							break;
						case SDLK_t:
							if (iScreen == 1)
							{
								if (iEditPoP == 1)
								{
									switch (cCurType)
									{
										case 'd': cCurType = 'p'; break;
										case 'p': cCurType = 'd'; break;
									}
									PlaySound ("wav/extras.wav");
								} else {
/***
									switch (cCurType)
									{
										case 'o': cCurType = 'c'; break;
										case 'c': cCurType = 'r'; break;
										case 'r': cCurType = 't'; break;
										case 't': cCurType = 'o'; break;
									}
									PlaySound ("wav/extras.wav");
***/
								}
							}
							break;
						case SDLK_MINUS:
						case SDLK_KP_MINUS:
						case SDLK_UNDERSCORE:
							if ((int)luLevelNr != 1)
							{
								if (iChanged != 0) { InitPopUpSave (iScreen); }
								Prev ((int)luLevelNr);
							}
							break;
						case SDLK_PLUS:
						case SDLK_KP_PLUS:
						case SDLK_EQUALS:
							if (((int)luLevelNr != 0) && ((int)luLevelNr != 28))
							{
								if (iChanged != 0) { InitPopUpSave (iScreen); }
								Next ((int)luLevelNr);
							}
							break;
						case SDLK_r:
							if (iScreen != 2)
							{
								iScreen = 2;
								iMovingRoom = 0;
								iMovingNewBusy = 0;
								iChangingBrokenRoom = iCurRoom;
								iChangingBrokenSide = 1;
								PlaySound ("wav/screen2or3.wav");
							} else if (iBrokenRoomLinks == 0) {
								iBrokenRoomLinks = 1;
								PlaySound ("wav/screen2or3.wav");
							}
							break;
						case SDLK_e:
							if (iScreen == 2)
							{
								iBrokenRoomLinks = BrokenRoomLinks (iRoomConnections, 0);
							}
							if (iScreen != 3)
							{
								iScreen = 3;
								PlaySound ("wav/screen2or3.wav");
							}
							break;
						case SDLK_s:
							if (iChanged != 0) { CallSave (0); } break;
						case SDLK_z:
							if (iScreen == 1)
							{
								Zoom();
								iExtras = 0;
								PlaySound ("wav/extras.wav");
							}
							break;
						case SDLK_QUOTE:
							if (iScreen == 1)
							{
								if ((event.key.keysym.mod & KMOD_LSHIFT) ||
									(event.key.keysym.mod & KMOD_RSHIFT))
								{
									Sprinkle();
									PlaySound ("wav/extras.wav");
									iChanged++;
								} else {
									SetLocation (iCurRoom, iSelected, iLastThing,
										iLastModifier[1], iLastModifier[2],
										iLastModifier[3], iLastModifier[4]);
									PlaySound ("wav/ok_close.wav");
									iChanged++;
								}
							}
							break;
						case SDLK_h:
							if (iScreen == 1)
							{
								FlipRoom (iCurRoom, 1);
								PlaySound ("wav/extras.wav");
								iChanged++;
							}
							break;
						case SDLK_v:
							if (iScreen == 1)
							{
								if ((event.key.keysym.mod & KMOD_LCTRL) ||
									(event.key.keysym.mod & KMOD_RCTRL))
								{
									CopyPaste (iCurRoom, 2);
									PlaySound ("wav/extras.wav");
									iChanged++;
								} else {
									FlipRoom (iCurRoom, 2);
									PlaySound ("wav/extras.wav");
									iChanged++;
								}
							}
							break;
						case SDLK_c:
							if (iScreen == 1)
							{
								if ((event.key.keysym.mod & KMOD_LCTRL) ||
									(event.key.keysym.mod & KMOD_RCTRL))
								{
									CopyPaste (iCurRoom, 1);
									PlaySound ("wav/extras.wav");
								}
							}
							break;
						default: break;
					}
					ShowScreen (iScreen);
					break;
				case SDL_MOUSEMOTION:
					iOldXPos = iXPos;
					iOldYPos = iYPos;
					SDL_GetMouseState (&iXPos, &iYPos);
					if ((iOldXPos == iXPos) && (iOldYPos == iYPos)) { break; }

					if (iScreen == 2)
					{
						if (iMovingRoom != 0) { ShowScreen (2); }
					}

					/*** Level 10 warning. ***/
					if ((iEditPoP == 2) && ((luLevelNr == 10) ||
						(luLevelNr == 24)) && ((int)luKidRoom == 22))
					{
						if (InArea (event, 25, 0, 614, 25) == 1)
						{
							if (iLvl10Warning != 1)
								{ iLvl10Warning = 1; ShowScreen (iScreen); }
						} else {
							if (iLvl10Warning == 1)
								{ iLvl10Warning = 0; ShowScreen (iScreen); }
						}
					}

					if (iScreen == 1)
					{
						/*** User hovers over tiles in the upper row. ***/
						if ((InArea (event, HOR1, VER1 + TTP, HOR2, VER2 + TTP)
							== 1) && (iSelected != 1)) { iSelected = 1; ShowScreen (1); }
						else if ((InArea (event, HOR2, VER1 + TTP, HOR3, VER2 + TTP)
							== 1) && (iSelected != 2)) { iSelected = 2; ShowScreen (1); }
						else if ((InArea (event, HOR3, VER1 + TTP, HOR4, VER2 + TTP)
							== 1) && (iSelected != 3)) { iSelected = 3; ShowScreen (1); }
						else if ((InArea (event, HOR4, VER1 + TTP, HOR5, VER2 + TTP)
							== 1) && (iSelected != 4)) { iSelected = 4; ShowScreen (1); }
						else if ((InArea (event, HOR5, VER1 + TTP, HOR6, VER2 + TTP)
							== 1) && (iSelected != 5)) { iSelected = 5; ShowScreen (1); }
						else if ((InArea (event, HOR6, VER1 + TTP, HOR7, VER2 + TTP)
							== 1) && (iSelected != 6)) { iSelected = 6; ShowScreen (1); }
						else if ((InArea (event, HOR7, VER1 + TTP, HOR8, VER2 + TTP)
							== 1) && (iSelected != 7)) { iSelected = 7; ShowScreen (1); }
						else if ((InArea (event, HOR8, VER1 + TTP, HOR9, VER2 + TTP)
							== 1) && (iSelected != 8)) { iSelected = 8; ShowScreen (1); }
						else if ((InArea (event, HOR9, VER1 + TTP, HOR10, VER2 + TTP)
							== 1) && (iSelected != 9)) { iSelected = 9; ShowScreen (1); }
						else if ((InArea (event, HOR10, VER1 + TTP, HOR10 + 64,
							VER2 + TTP) == 1) && (iSelected != 10))
						{ iSelected = 10; ShowScreen (1); }

						/*** User hovers over tiles in the middle row. ***/
						else if ((InArea (event, HOR1, VER2 + TTP, HOR2, VER3 + TTP)
							== 1) && (iSelected != 11)) { iSelected = 11; ShowScreen (1); }
						else if ((InArea (event, HOR2, VER2 + TTP, HOR3, VER3 + TTP)
							== 1) && (iSelected != 12)) { iSelected = 12; ShowScreen (1); }
						else if ((InArea (event, HOR3, VER2 + TTP, HOR4, VER3 + TTP)
							== 1) && (iSelected != 13)) { iSelected = 13; ShowScreen (1); }
						else if ((InArea (event, HOR4, VER2 + TTP, HOR5, VER3 + TTP)
							== 1) && (iSelected != 14)) { iSelected = 14; ShowScreen (1); }
						else if ((InArea (event, HOR5, VER2 + TTP, HOR6, VER3 + TTP)
							== 1) && (iSelected != 15)) { iSelected = 15; ShowScreen (1); }
						else if ((InArea (event, HOR6, VER2 + TTP, HOR7, VER3 + TTP)
							== 1) && (iSelected != 16)) { iSelected = 16; ShowScreen (1); }
						else if ((InArea (event, HOR7, VER2 + TTP, HOR8, VER3 + TTP)
							== 1) && (iSelected != 17)) { iSelected = 17; ShowScreen (1); }
						else if ((InArea (event, HOR8, VER2 + TTP, HOR9, VER3 + TTP)
							== 1) && (iSelected != 18)) { iSelected = 18; ShowScreen (1); }
						else if ((InArea (event, HOR9, VER2 + TTP, HOR10, VER3 + TTP)
							== 1) && (iSelected != 19)) { iSelected = 19; ShowScreen (1); }
						else if ((InArea (event, HOR10, VER2 + TTP, HOR10 + 64,
							VER3 + TTP) == 1) && (iSelected != 20))
						{ iSelected = 20; ShowScreen (1); }

						/*** User hovers over tiles in the bottom row. ***/
						else if ((InArea (event, HOR1, VER3 + TTP, HOR2, VER3 + 126 + TTP)
							== 1) && (iSelected != 21)) { iSelected = 21; ShowScreen (1); }
						else if ((InArea (event, HOR2, VER3 + TTP, HOR3, VER3 + 126 + TTP)
							== 1) && (iSelected != 22)) { iSelected = 22; ShowScreen (1); }
						else if ((InArea (event, HOR3, VER3 + TTP, HOR4, VER3 + 126 + TTP)
							== 1) && (iSelected != 23)) { iSelected = 23; ShowScreen (1); }
						else if ((InArea (event, HOR4, VER3 + TTP, HOR5, VER3 + 126 + TTP)
							== 1) && (iSelected != 24)) { iSelected = 24; ShowScreen (1); }
						else if ((InArea (event, HOR5, VER3 + TTP, HOR6, VER3 + 126 + TTP)
							== 1) && (iSelected != 25)) { iSelected = 25; ShowScreen (1); }
						else if ((InArea (event, HOR6, VER3 + TTP, HOR7, VER3 + 126 + TTP)
							== 1) && (iSelected != 26)) { iSelected = 26; ShowScreen (1); }
						else if ((InArea (event, HOR7, VER3 + TTP, HOR8, VER3 + 126 + TTP)
							== 1) && (iSelected != 27)) { iSelected = 27; ShowScreen (1); }
						else if ((InArea (event, HOR8, VER3 + TTP, HOR9, VER3 + 126 + TTP)
							== 1) && (iSelected != 28)) { iSelected = 28; ShowScreen (1); }
						else if ((InArea (event, HOR9, VER3 + TTP, HOR10, VER3 + 126 + TTP)
							== 1) && (iSelected != 29)) { iSelected = 29; ShowScreen (1); }
						else if ((InArea (event, HOR10, VER3 + TTP, HOR10 + 64,
							VER3 + 126 + TTP) == 1) && (iSelected != 30))
						{ iSelected = 30; ShowScreen (1); }

						/*** extras ***/
						if ((InArea (event, 620, 3, 629, 12) == 1) && (iExtras != 1))
							{ iExtras = 1; ShowScreen (1); }
						else if ((InArea (event, 630, 3, 639, 12) == 1) && (iExtras != 2))
							{ iExtras = 2; ShowScreen (1); }
						else if ((InArea (event, 640, 3, 649, 12) == 1) && (iExtras != 3))
							{ iExtras = 3; ShowScreen (1); }
						else if ((InArea (event, 650, 3, 659, 12) == 1) && (iExtras != 4))
							{ iExtras = 4; ShowScreen (1); }
						else if ((InArea (event, 620, 13, 629, 22) == 1) && (iExtras != 5))
							{ iExtras = 5; ShowScreen (1); }
						else if ((InArea (event, 630, 13, 639, 22) == 1) && (iExtras != 6))
							{ iExtras = 6; ShowScreen (1); }
						else if ((InArea (event, 640, 13, 649, 22) == 1) && (iExtras != 7))
							{ iExtras = 7; ShowScreen (1); }
						else if ((InArea (event, 650, 13, 659, 22) == 1) && (iExtras != 8))
							{ iExtras = 8; ShowScreen (1); }
						else if ((InArea (event, 620, 3, 659, 22) == 0) && (iExtras != 0))
							{ iExtras = 0; ShowScreen (1); }
					}

					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (event, 0, 50, 25, 430) == 1) /*** left arrow ***/
						{
							if (iRoomConnections[iCurRoom][1] != 0)
								iDownAt = 1;
						}
						if (InArea (event, 667, 50, 692, 430) == 1) /*** right arrow ***/
						{
							if (iRoomConnections[iCurRoom][2] != 0)
								iDownAt = 2;
						}
						if (InArea (event, 25, 25, 667, 50) == 1) /*** up arrow ***/
						{
							if (iRoomConnections[iCurRoom][3] != 0)
								iDownAt = 3;
						}
						if (InArea (event, 25, 430, 667, 455) == 1) /*** down arrow ***/
						{
							if (iRoomConnections[iCurRoom][4] != 0)
								iDownAt = 4;
						}
						if (InArea (event, 0, 25, 25, 50) == 1) /*** rooms ***/
						{
							if (iBrokenRoomLinks == 0)
							{
								iDownAt = 5;
							} else {
								iDownAt = 11;
							}
						}
						if (InArea (event, 667, 25, 692, 50) == 1) /*** events ***/
						{
							iDownAt = 6;
						}
						if (InArea (event, 0, 430, 25, 455) == 1) /*** save ***/
						{
							iDownAt = 7;
						}
						if (InArea (event, 667, 430, 692, 455) == 1) /*** quit ***/
						{
							iDownAt = 8;
						}
						if (InArea (event, 0, 0, 25, 25) == 1) /*** previous ***/
						{
							iDownAt = 9;
						}
						if (InArea (event, 667, 0, 692, 25) == 1) /*** next ***/
						{
							iDownAt = 10;
						}

						if (iScreen == 2)
						{
							if (iBrokenRoomLinks == 0)
							{
								for (iTemp = 0; iTemp < iRooms; iTemp++) /*** x ***/
								{
									for (iTemp2 = 0; iTemp2 < iRooms; iTemp2++) /*** y ***/
									{
										if (InArea (event, 297 + (iTemp * 15), 61 + (iTemp2 * 15),
											310 + (iTemp * 15), 74 + (iTemp2 * 15)) == 1)
										{
											if (iRoomArray[iTemp + 1][iTemp2 + 1] != 0)
											{
												iMovingNewBusy = 0;
												iMovingRoom = iRoomArray[iTemp + 1][iTemp2 + 1];
											}
										}
									}
								}
								for (iTemp2 = 0; iTemp2 < iRooms; iTemp2++) /*** y ***/
								{
									if (iTemp2 < 24) /*** right pane ***/
									{
										if (InArea (event, 272, 61 + (iTemp2 * 15),
											272 + 14, 61 + 14 + (iTemp2 * 15)) == 1)
										{
											if (iRoomArray[25][iTemp2 + 1] != 0)
											{
												iMovingNewBusy = 0;
												iMovingRoom = iRoomArray[25][iTemp2 + 1];
											}
										}
									} else { /*** left pane ***/
										if ((InArea (event, 247, 301 + ((iTemp2 - 24) * 15),
											247 + 14, 301 + 14 + ((iTemp2 - 24) * 15)) == 1) &&
											(iEditPoP == 2))
										{
											if (iRoomArray[25][iTemp2 + 1] != 0)
											{
												iMovingNewBusy = 0;
												iMovingRoom = iRoomArray[25][iTemp2 + 1];
											}
										}
									}
								}

								/*** rooms broken ***/
								if (InArea (event, 629, 63, 654, 88) == 1)
								{
									iDownAt = 11;
								}
							} else {
								MouseSelectAdj (event);
							}
						}

						if (iScreen == 3) /*** events ***/
						{
							if (InArea (event, 251, 254, 336, 286) == 1) /*** edit left ***/
								{ if (iEditLeftOn != 1) { iEditLeftOn = 1; } }
							if (InArea (event, 346, 254, 431, 286) == 1) /*** end left ***/
								{ if (iEndLeftOn != 1) { iEndLeftOn = 1; } }
							if (InArea (event, 476, 254, 561, 286) == 1) /*** edit right ***/
								{ if (iEditRightOn != 1) { iEditRightOn = 1; } }
							if (InArea (event, 571, 254, 656, 286) == 1) /*** end right ***/
								{ if (iEndRightOn != 1) { iEndRightOn = 1; } }
						}
					}
					ShowScreen (iScreen);
					break;
				case SDL_MOUSEBUTTONUP:
					iDownAt = 0;
					iEditLeftOn = 0;
					iEndLeftOn = 0;
					iEditRightOn = 0;
					iEndRightOn = 0;
					if (event.button.button == 1) /*** left mouse button, change ***/
					{
						if (InArea (event, 0, 50, 25, 430) == 1) /*** left arrow ***/
						{
							if (iRoomConnections[iCurRoom][1] != 0) {
								iCurRoom = iRoomConnections[iCurRoom][1];
								PlaySound ("wav/scroll.wav"); }
						}
						if (InArea (event, 667, 50, 692, 430) == 1) /*** right arrow ***/
						{
							if (iRoomConnections[iCurRoom][2] != 0) {
								iCurRoom = iRoomConnections[iCurRoom][2];
								PlaySound ("wav/scroll.wav"); }
						}
						if (InArea (event, 25, 25, 667, 50) == 1) /*** up arrow ***/
						{
							if (iRoomConnections[iCurRoom][3] != 0) {
								iCurRoom = iRoomConnections[iCurRoom][3];
								PlaySound ("wav/scroll.wav"); }
						}
						if (InArea (event, 25, 430, 667, 455) == 1) /*** down arrow ***/
						{
							if (iRoomConnections[iCurRoom][4] != 0) {
								iCurRoom = iRoomConnections[iCurRoom][4];
								PlaySound ("wav/scroll.wav"); }
						}
						if (InArea (event, 0, 25, 25, 50) == 1) /*** rooms ***/
						{
							if (iScreen != 2)
							{
								iScreen = 2; iMovingRoom = 0; iMovingNewBusy = 0;
								iChangingBrokenRoom = iCurRoom;
								iChangingBrokenSide = 1;
								PlaySound ("wav/screen2or3.wav");
							}
						}
						if (InArea (event, 667, 25, 692, 50) == 1) /*** events ***/
						{
							if (iScreen == 2)
							{
								iBrokenRoomLinks = BrokenRoomLinks (iRoomConnections, 0);
							}
							if (iScreen != 3)
							{
								iScreen = 3;
								PlaySound ("wav/screen2or3.wav");
							}
						}
						if (InArea (event, 0, 430, 25, 455) == 1) /*** save ***/
						{
							if (iChanged != 0) { CallSave (0); }
						}
						if (InArea (event, 667, 430, 692, 455) == 1) /*** quit ***/
						{
							switch (iScreen)
							{
								case 1:
									Quit(); break;
								case 2:
									iBrokenRoomLinks = BrokenRoomLinks (iRoomConnections, 0);
									/*** no break ***/
								case 3:
									iScreen = 1; break;
							}
						}
						if (InArea (event, 0, 0, 25, 25) == 1) /*** previous ***/
						{
							if (iChanged != 0) { InitPopUpSave (iScreen); }
							Prev ((int)luLevelNr);
						}
						if (InArea (event, 667, 0, 692, 25) == 1) /*** next ***/
						{
							if (iChanged != 0) { InitPopUpSave (iScreen); }
							Next ((int)luLevelNr);
						}
						if (InArea (event, 25, 0, 612, 24) == 1) /*** level bar ***/
						{
							RunLevel ((int)luLevelNr);
						}

						if (iScreen == 1)
						{
							if (InArea (event, HOR1, VER1 + TTP, HOR10 + 64,
								VER3 + 126 + TTP) == 1)
							{
								keystate = SDL_GetKeyState (NULL);
								if ((keystate[SDLK_LSHIFT]) || (keystate[SDLK_RSHIFT]))
								{
									SetLocation (iCurRoom, iSelected, iLastThing,
										iLastModifier[1], iLastModifier[2], iLastModifier[3],
										iLastModifier[4]);
									PlaySound ("wav/ok_close.wav");
									iChanged++;
								} else {
									ChangePos (iSelected, screen);
								}
							}

							if (InArea (event, 620, 3, 629, 12) == 1)
							{
								Zoom();
								iExtras = 0;
								PlaySound ("wav/extras.wav");
							}
							if (InArea (event, 650, 3, 659, 12) == 1)
							{
								if (iEditPoP == 1)
								{
									switch (cCurType)
									{
										case 'd': cCurType = 'p'; break;
										case 'p': cCurType = 'd'; break;
									}
									PlaySound ("wav/extras.wav");
								} else {
/***
									switch (cCurType)
									{
										case 'o': cCurType = 'c'; break;
										case 'c': cCurType = 'r'; break;
										case 'r': cCurType = 't'; break;
										case 't': cCurType = 'o'; break;
									}
									PlaySound ("wav/extras.wav");
***/
								}
							}
							if (InArea (event, 620, 13, 629, 22) == 1)
							{
								Sprinkle();
								PlaySound ("wav/extras.wav");
								iChanged++;
							}
							if (InArea (event, 640, 13, 649, 22) == 1)
							{
								FlipRoom (iCurRoom, 1);
								PlaySound ("wav/extras.wav");
								iChanged++;
							}
							if (InArea (event, 640, 3, 649, 12) == 1)
							{
								FlipRoom (iCurRoom, 2);
								PlaySound ("wav/extras.wav");
								iChanged++;
							}
							if (InArea (event, 630, 3, 639, 12) == 1)
							{
								CopyPaste (iCurRoom, 1);
								PlaySound ("wav/extras.wav");
							}
							if (InArea (event, 630, 13, 639, 22) == 1)
							{
								CopyPaste (iCurRoom, 2);
								PlaySound ("wav/extras.wav");
								iChanged++;
							}
						}

						if (iScreen == 2) /*** room links screen ***/
						{
							if (iBrokenRoomLinks == 0)
							{
								for (iTemp = 0; iTemp < iRooms; iTemp++) /*** x ***/
								{
									for (iTemp2 = 0; iTemp2 < iRooms; iTemp2++) /*** y ***/
									{
										if (InArea (event, 297 + (iTemp * 15), 61 + (iTemp2 * 15),
											310 + (iTemp * 15), 74 + (iTemp2 * 15)) == 1)
										{
											if (iMovingRoom != 0)
											{
												if (iRoomArray[iTemp + 1][iTemp2 + 1] == 0)
												{
													RemoveOldRoom();
													AddNewRoom (iTemp + 1, iTemp2 + 1, iMovingRoom);
													iChanged++;
												}
												iMovingRoom = 0; iMovingNewBusy = 0;
											}
										}
									}
								}
								for (iTemp2 = 0; iTemp2 < iRooms; iTemp2++) /*** y ***/
								{
									if (iTemp2 < 24) /*** right pane ***/
									{
										if (InArea (event, 272, 61 + (iTemp2 * 15),
											272 + 14, 61 + 14 + (iTemp2 * 15)) == 1)
										{
											if (iMovingRoom != 0)
											{
												if (iRoomArray[25][iTemp2 + 1] == 0)
												{
													RemoveOldRoom();
													AddNewRoom (25, iTemp2 + 1, iMovingRoom);
													iChanged++;
												}
												iMovingRoom = 0; iMovingNewBusy = 0;
											}
										}
									} else { /*** left pane ***/
										if ((InArea (event, 247, 301 + ((iTemp2 - 24) * 15),
											247 + 14, 301 + 14 + ((iTemp2 - 24) * 15)) == 1) &&
											(iEditPoP == 2))
										{
											if (iMovingRoom != 0)
											{
												if (iRoomArray[25][iTemp2 + 1] == 0)
												{
													RemoveOldRoom();
													AddNewRoom (25, iTemp2 + 1, iMovingRoom);
													iChanged++;
												}
												iMovingRoom = 0; iMovingNewBusy = 0;
											}
										}
									}
								}

								/*** rooms broken ***/
								if (InArea (event, 629, 63, 654, 88) == 1)
								{
									iBrokenRoomLinks = 1;
									PlaySound ("wav/screen2or3.wav");
								}
							} else {
								if (MouseSelectAdj (event) == 1)
								{
									iRoomConnections[iChangingBrokenRoom]
										[iChangingBrokenSide]++;
									if (iRoomConnections[iChangingBrokenRoom]
										[iChangingBrokenSide] == iRooms + 1)
									{
										iRoomConnections[iChangingBrokenRoom]
											[iChangingBrokenSide] = 0;
									}
									iChanged++;
									iBrokenRoomLinks = BrokenRoomLinks (iRoomConnections, 0);
									PlaySound ("wav/hum_adj.wav");
								}
							}
						}

						if (iScreen == 3) /*** events screen ***/
						{
							/*** Change event number ***/
							if (InArea (event, 343, 60, 355, 79) == 1)
								{ ChangeEvent (-10, 0); }
							if (InArea (event, 358, 60, 370, 79) == 1)
								{ ChangeEvent (-1, 0); }
							if (InArea (event, 428, 60, 440, 79) == 1)
								{ ChangeEvent (1, 0); }
							if (InArea (event, 443, 60, 455, 79) == 1)
								{ ChangeEvent (10, 0); }

							/*** room (1-24) ***/
							if ((event.motion.y >= 115 * iScale) &&
								(event.motion.y <= 129 * iScale))
							{
								for (iTemp = 1; iTemp <= 24; iTemp++)
								{
									if ((event.motion.x >= (297 + ((iTemp - 1) * 15)) * iScale)
										&& (event.motion.x <= (311 + ((iTemp - 1) * 15)) * iScale))
									{
										if (iEditPoP == 1)
										{
											EventRoom (iTemp);
										} else {
											iPoP2DoorRoom[iChangeEvent] = iTemp;
											PlaySound ("wav/check_box.wav");
											iChanged++;
										}
									}
								}
							}
							/*** room (25-32) ***/
							if ((event.motion.y >= 150 * iScale) &&
								(event.motion.y <= 164 * iScale) && (iEditPoP == 2))
							{
								for (iTemp = 25; iTemp <= 32; iTemp++)
								{
									if ((event.motion.x >= (297 + ((iTemp - 25) * 15)) * iScale)
										&& (event.motion.x <=
										(311 + ((iTemp - 25) * 15)) * iScale))
									{
										iPoP2DoorRoom[iChangeEvent] = iTemp;
										PlaySound ("wav/check_box.wav");
										iChanged++;
									}
								}
							}

							/*** location ***/
							for (iTemp2 = 1; iTemp2 <= 3; iTemp2++)
							{
								for (iTemp = 1; iTemp <= 10; iTemp++)
								{
									if ((event.motion.x >= (462 + ((iTemp - 1) * 15)) * iScale)
										&& (event.motion.x <= (475 + ((iTemp - 1) * 15)) * iScale))
									{
										if ((event.motion.y >=
											(155 + ((iTemp2 - 1) * 15)) * iScale)
											&& (event.motion.y <=
											(168 + ((iTemp2 - 1) * 15)) * iScale))
										{
											if (iEditPoP == 1)
											{
												EventDoor (iTemp, iTemp2);
											} else {
												iPoP2DoorLocation[iChangeEvent] =
													((iTemp2 - 1) * 10) + iTemp - 1;
												PlaySound ("wav/check_box.wav");
												iChanged++;
											}
										}
									}
								}
							}

							if (iEditPoP == 1)
							{
								/*** next ***/
								if (InArea (event, 582, 225, 595, 238) == 1) { EventNext (1); }
								if (InArea (event, 597, 225, 610, 238) == 1) { EventNext (0); }
							} else {
								/*** left chain ***/
								PlusMinus (event, &iPoP2DoorLeft[iChangeEvent],
									319, 224, 0, 255, -10, 1);
								PlusMinus (event, &iPoP2DoorLeft[iChangeEvent],
									334, 224, 0, 255, -1, 1);
								PlusMinus (event, &iPoP2DoorLeft[iChangeEvent],
									404, 224, 0, 255, 1, 1);
								PlusMinus (event, &iPoP2DoorLeft[iChangeEvent],
									419, 224, 0, 255, 10, 1);
								/*** right chain ***/
								PlusMinus (event, &iPoP2DoorRight[iChangeEvent],
									544, 224, 0, 255, -10, 1);
								PlusMinus (event, &iPoP2DoorRight[iChangeEvent],
									559, 224, 0, 255, -1, 1);
								PlusMinus (event, &iPoP2DoorRight[iChangeEvent],
									629, 224, 0, 255, 1, 1);
								PlusMinus (event, &iPoP2DoorRight[iChangeEvent],
									644, 224, 0, 255, 10, 1);

								if (InArea (event, 251, 254, 336, 286) == 1)
								{ /*** edit left ***/
									iChangeEvent = iPoP2DoorLeft[iChangeEvent];
									PlaySound ("wav/screen2or3.wav");
								}
								if (InArea (event, 346, 254, 431, 286) == 1)
								{ /*** end left ***/
									iPoP2DoorLeft[iChangeEvent] = 253;
									PlaySound ("wav/screen2or3.wav");
									iChanged++;
								}
								if (InArea (event, 476, 254, 561, 286) == 1)
								{ /*** edit right ***/
									iChangeEvent = iPoP2DoorRight[iChangeEvent];
									PlaySound ("wav/screen2or3.wav");
								}
								if (InArea (event, 571, 254, 656, 286) == 1)
								{ /*** end right ***/
									iPoP2DoorRight[iChangeEvent] = 253;
									PlaySound ("wav/screen2or3.wav");
									iChanged++;
								}

								/*** checkpoints ***/
								PlusMinus (event, &iCheckPoints[0], 306, 301, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[0], 376, 301, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[1], 391, 301, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[1], 461, 301, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[2], 306, 325, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[2], 376, 325, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[3], 391, 325, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[3], 461, 325, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[4], 306, 349, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[4], 376, 349, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[5], 391, 349, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[5], 461, 349, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[6], 306, 373, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[6], 376, 373, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[7], 391, 373, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[7], 461, 373, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[8], 306, 397, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[8], 376, 397, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[9], 391, 397, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[9], 461, 397, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[10], 484, 325, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[10], 554, 325, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[11], 569, 325, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[11], 639, 325, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[12], 484, 349, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[12], 554, 349, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[13], 569, 349, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[13], 639, 349, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[14], 484, 373, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[14], 554, 373, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[15], 569, 373, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[15], 639, 373, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[16], 484, 397, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[16], 554, 397, 0, 99, 1, 1);
								PlusMinus (event, &iCheckPoints[17], 569, 397, 0, 99, -1, 1);
								PlusMinus (event, &iCheckPoints[17], 639, 397, 0, 99, 1, 1);
							}
						}
					}
					if (event.button.button == 2) /*** middle mouse button, clear ***/
					{
						if (iScreen == 1)
						{
							for (iTemp = 1; iTemp <= 30; iTemp++)
							{
								if (cCurType == 'o')
									{ SetLocation (iCurRoom, iTemp, 0, 0, -1, -1, -1); }
										else { SetLocation (iCurRoom, iTemp, 0, 0, 0, 0, 0); }
							}
							if (iEditPoP == 1)
							{
								sGuardLocations[iCurRoom - 1] = 30;
							} else {
								for (iTile = 0; iTile < 30; iTile++)
									{ sMusic[((iCurRoom - 1) * 30) + iTile] = 0; }
								iStaticGuards_Amount[iCurRoom - 1] = 0;
								iDynamicGuards_Sets[iCurRoom - 1] = 0;
							}
							PlaySound ("wav/ok_close.wav");
							iChanged++;
						}
					}
					if (event.button.button == 3) /*** right mouse button, randomize ***/
					{
						if (iScreen == 1)
						{
							for (iTemp2 = 1; iTemp2 <= iRooms; iTemp2++)
							{
								for (iTemp = 1; iTemp <= 30; iTemp++)
								{
									UseTile (-1, iTemp, iTemp2);
								}
							}
							PlaySound ("wav/ok_close.wav");
							iChanged++;
						}
						if (iScreen == 2)
						{
							if (iBrokenRoomLinks == 1)
							{
								if (MouseSelectAdj (event) == 1)
								{
									iRoomConnections[iChangingBrokenRoom]
										[iChangingBrokenSide]--;
									if (iRoomConnections[iChangingBrokenRoom]
										[iChangingBrokenSide] == -1)
									{
										iRoomConnections[iChangingBrokenRoom]
											[iChangingBrokenSide] = iRooms;
									}
									iChanged++;
									iBrokenRoomLinks = BrokenRoomLinks (iRoomConnections, 0);
									PlaySound ("wav/hum_adj.wav");
								}
							}
						}
					}
					if (event.button.button == 4) /*** scroll wheel up ***/
					{
						if (InArea (event, HOR1, VER1 + TTP, HOR10 + 64,
							VER3 + 126 + TTP) == 1) /*** middle field ***/
						{
							keystate = SDL_GetKeyState (NULL);
							if ((keystate[SDLK_LSHIFT]) || (keystate[SDLK_RSHIFT]))
							{ /*** right ***/
								if (iRoomConnections[iCurRoom][2] != 0) {
									iCurRoom = iRoomConnections[iCurRoom][2];
									PlaySound ("wav/scroll.wav"); }
							} else { /*** up ***/
								if (iRoomConnections[iCurRoom][3] != 0) {
									iCurRoom = iRoomConnections[iCurRoom][3];
									PlaySound ("wav/scroll.wav"); }
							}
						}
					}
					if (event.button.button == 5) /*** scroll wheel down ***/
					{
						if (InArea (event, HOR1, VER1 + TTP, HOR10 + 64,
							VER3 + 126 + TTP) == 1) /*** middle field ***/
						{
							keystate = SDL_GetKeyState (NULL);
							if ((keystate[SDLK_LSHIFT]) || (keystate[SDLK_RSHIFT]))
							{ /*** left ***/
								if (iRoomConnections[iCurRoom][1] != 0) {
									iCurRoom = iRoomConnections[iCurRoom][1];
									PlaySound ("wav/scroll.wav"); }
							} else { /*** down ***/
								if (iRoomConnections[iCurRoom][4] != 0) {
									iCurRoom = iRoomConnections[iCurRoom][4];
									PlaySound ("wav/scroll.wav"); }
							}
						}
					}
					ShowScreen (iScreen);
					break;
				case SDL_QUIT:
					Quit(); break;
				default:
					break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
}
/*****************************************************************************/
void EventDoor (int iX, int iY)
/*****************************************************************************/
{
	char sBinaryFDoors[9 + 2]; /*** 8 chars, plus \0 ***/
	char sTempBinary[9 + 2]; /*** 8 chars, plus \0 ***/

	/* This part is kind of complicated. Only some bits have
	 * to be changed, so we first convert the event to bits,
	 * change some of them, and then convert them all back.
	 */
	GetAsEightBits (sFirstDoorEvents[iChangeEvent],
		sBinaryFDoors);
	GetAsEightBits ((((iY - 1) * 10) + iX - 1),
		sTempBinary);
	sBinaryFDoors[3] = sTempBinary[3];
	sBinaryFDoors[4] = sTempBinary[4];
	sBinaryFDoors[5] = sTempBinary[5];
	sBinaryFDoors[6] = sTempBinary[6];
	sBinaryFDoors[7] = sTempBinary[7];
	sFirstDoorEvents[iChangeEvent] =
		BitsToInt (sBinaryFDoors);
	PlaySound ("wav/check_box.wav");
	iChanged++;
}
/*****************************************************************************/
void EventNext (int iNoNext)
/*****************************************************************************/
{
	char sBinaryFDoors[9 + 2]; /*** 8 chars, plus \0 ***/

	/* This part is kind of complicated. Only one bit has
	 * to be changed, so we first convert the event to bits,
	 * change one of them, and then convert them all back.
	 */
	GetAsEightBits (sFirstDoorEvents[iChangeEvent],
		sBinaryFDoors);
	switch (iNoNext)
	{
		case 1: sBinaryFDoors[0] = '1'; break;
		case 0: sBinaryFDoors[0] = '0'; break;
	}
	sFirstDoorEvents[iChangeEvent] =
		BitsToInt (sBinaryFDoors);
	PlaySound ("wav/check_box.wav");
	iChanged++;
}
/*****************************************************************************/
void RunLevel (int iLevel)
/*****************************************************************************/
{
	/*** The following are used for the batch file. ***/
	int iFd;
	char sToWrite[MAX_TOWRITE + 2];
	SDL_Thread *princethread;

	if (((iLevel != 15) && (iLevel != 0) && (iEditPoP == 1)) ||
		(iEditPoP == 2))
	{
		/*** Create batch file. ***/
		if (iEditPoP == 1)
		{
			iFd = open (BATCH_FILE, O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, 0600);
		} else {
			iFd = open (BATCH_FILE_POP2, O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, 0600);
		}
		if (iFd == -1)
		{
			printf ("[FAILED] Could not create batch file: %s!\n",
				strerror (errno));
			exit (EXIT_ERROR);
		}
		if (iEditPoP == 1)
		{
			snprintf (sToWrite, MAX_TOWRITE, "PRINCE %s %i\n", sCheat1, iLevel);
		} else {
			if (iLevel <= 14)
			{
				snprintf (sToWrite, MAX_TOWRITE, "PRINCE %s level%i\n",
					sCheat2, iLevel);
			} else {
				snprintf (sToWrite, MAX_TOWRITE, "PRINCE gameplay %s level%i\n",
					sCheat2, iLevel - 14);
			}
		}
		write (iFd, sToWrite, strlen (sToWrite));
		close (iFd);
		princethread = SDL_CreateThread (StartGame, NULL);
		if (princethread == NULL)
		{
			printf ("[FAILED] Could not create thread!\n");
			exit (EXIT_ERROR);
		}
	}
}
/*****************************************************************************/
void EventRoom (int iRoom)
/*****************************************************************************/
{
	char sBinarySDoors[9 + 2]; /*** 8 chars, plus \0 ***/
	char sBinaryFDoors[9 + 2]; /*** 8 chars, plus \0 ***/
	char sTempBinary[9 + 2]; /*** 8 chars, plus \0 ***/

	/* This part is kind of complicated. Only some bits have
	 * to be changed, so we first convert the event to bits,
	 * change some of them, and then convert all of them back.
	 */
	GetAsEightBits (sFirstDoorEvents[iChangeEvent],
		sBinaryFDoors);
	GetAsEightBits (sSecondDoorEvents[iChangeEvent],
		sBinarySDoors);
	GetAsEightBits (iRoom, sTempBinary);
	sBinarySDoors[0] = sTempBinary[3];
	sBinarySDoors[1] = sTempBinary[4];
	sBinarySDoors[2] = sTempBinary[5];
	sBinaryFDoors[1] = sTempBinary[6];
	sBinaryFDoors[2] = sTempBinary[7];
	sFirstDoorEvents[iChangeEvent] =
		BitsToInt (sBinaryFDoors);
	sSecondDoorEvents[iChangeEvent] =
		BitsToInt (sBinarySDoors);
	PlaySound ("wav/check_box.wav");
	iChanged++;
}
/*****************************************************************************/
void RemoveOldRoom (void)
/*****************************************************************************/
{
	iRoomArray[iMovingOldX][iMovingOldY] = 0;

	/* Change the links of the rooms around
	 * the removed room.
	 */

	/*** left of removed ***/
	if ((iMovingOldX >= 2) && (iMovingOldX <= 24))
	{
		if (iRoomArray[iMovingOldX - 1][iMovingOldY] != 0)
		{
			iRoomConnections[iRoomArray[iMovingOldX - 1]
				[iMovingOldY]][2] = 0; /*** remove right ***/
		}
	}

	/*** right of removed ***/
	if ((iMovingOldX >= 1) && (iMovingOldX <= 23))
	{
		if (iRoomArray[iMovingOldX + 1][iMovingOldY] != 0)
		{
			iRoomConnections[iRoomArray[iMovingOldX + 1]
				[iMovingOldY]][1] = 0; /*** remove left ***/
		}
	}

	/*** above removed ***/
	if ((iMovingOldY >= 2) && (iMovingOldY <= 24))
	{
		if (iRoomArray[iMovingOldX][iMovingOldY - 1] != 0)
		{
			iRoomConnections[iRoomArray[iMovingOldX]
				[iMovingOldY - 1]][4] = 0; /*** remove below ***/
		}
	}

	/*** below removed ***/
	if ((iMovingOldY >= 1) && (iMovingOldY <= 23))
	{
		if (iRoomArray[iMovingOldX][iMovingOldY + 1] != 0)
		{
			iRoomConnections[iRoomArray[iMovingOldX]
				[iMovingOldY + 1]][3] = 0; /*** remove above ***/
		}
	}
}
/*****************************************************************************/
void AddNewRoom (int iX, int iY, int iRoom)
/*****************************************************************************/
{
	iRoomArray[iX][iY] = iRoom;

	/* Change the links of the rooms around
	 * the new room and the room itself.
	 */

	iRoomConnections[iRoom][1] = 0;
	iRoomConnections[iRoom][2] = 0;
	iRoomConnections[iRoom][3] = 0;
	iRoomConnections[iRoom][4] = 0;
	if ((iX >= 2) && (iX <= 24)) /*** left of added ***/
	{
		if (iRoomArray[iX - 1][iY] != 0)
		{
			iRoomConnections[iRoomArray[iX - 1]
				[iY]][2] = iRoom; /*** add room right ***/
			iRoomConnections[iRoom][1] = iRoomArray[iX - 1][iY];
		}
	}
	if ((iX >= 1) && (iX <= 23)) /*** right of added ***/
	{
		if (iRoomArray[iX + 1][iY] != 0)
		{
			iRoomConnections[iRoomArray[iX + 1]
				[iY]][1] = iRoom; /*** add room left ***/
			iRoomConnections[iRoom][2] = iRoomArray[iX + 1][iY];
		}
	}
	if ((iY >= 2) && (iY <= 24)) /*** above added ***/
	{
		if (iRoomArray[iX][iY - 1] != 0)
		{
			iRoomConnections[iRoomArray[iX]
				[iY - 1]][4] = iRoom; /*** add room below ***/
			iRoomConnections[iRoom][3] = iRoomArray[iX][iY - 1];
		}
	}
	if ((iY >= 1) && (iY <= 23)) /*** below added ***/
	{
		if (iRoomArray[iX][iY + 1] != 0)
		{
			iRoomConnections[iRoomArray[iX]
				[iY + 1]][3] = iRoom; /*** add room above ***/
			iRoomConnections[iRoom][4] = iRoomArray[iX][iY + 1];
		}
	}
	PlaySound ("wav/move_room.wav");
}
/*****************************************************************************/
void InitRooms (void)
/*****************************************************************************/
{
	int iTemp;
	int iTemp2;

	for (iTemp = 0; iTemp <= iRooms + 1; iTemp++) /*** x ***/
	{
		for (iTemp2 = 0; iTemp2 <= iRooms; iTemp2++) /*** y ***/
		{
			iRoomArray[iTemp][iTemp2] = 0;
		}
	}
}
/*****************************************************************************/
int StartGame (void *unused)
/*****************************************************************************/
{
	PlaySound ("wav/dosbox.wav");
	if (iEditPoP == 1)
	{
		if (system ("dosbox " BATCH_FILE " > " DEVNULL) == -1)
			{ printf ("[FAILED] Could not execute PoP1 batch file!\n"); }
	} else {
		if (system ("dosbox " BATCH_FILE_POP2 " > " DEVNULL) == -1)
			{ printf ("[FAILED] Could not execute PoP2 batch file!\n"); }
	}
	return (EXIT_NORMAL);
}
/*****************************************************************************/
void Quit (void)
/*****************************************************************************/
{
	if (iChanged != 0) { InitPopUpSave (iScreen); }
	TTF_CloseFont (font1);
	TTF_CloseFont (font2);
	TTF_CloseFont (font3);
	TTF_Quit();
	SDL_Quit();
	exit (EXIT_NORMAL);
}
/*****************************************************************************/
void ShowScreen (int iScreen)
/*****************************************************************************/
{
	char sText[MAX_TEXT + 2];
	char sTemp[MAX_TEXT + 2];
	char sGamePlay[MAX_TEXT + 2];
	char sLevelType[MAX_TEXT + 2];
	int iTemp;
	int iLoc;
	int iUnusedRooms;
	int iRoom;
	int iSide;
	int iTargetRoom;
	int iTargetTile;
	int iGuard;
	int iGWidth, iGHeight;
	int iTargetW, iTargetH;

	/*** black background ***/
	ShowImage (-4, (int[]){1, 0, 0, 0}, screen, 31, 0, 0, 692, 430);

	if (iScreen == 1)
	{
		/*** PoP2 backgrounds ***/
		if (iEditPoP == 2)
		{
			if (((luLevelNr == 1) || (luLevelNr == 15)) && (iCurRoom == 1))
				ShowImage (-14, (int[]){1, 1, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 1) || (luLevelNr == 15)) && (iCurRoom == 2))
				ShowImage (-14, (int[]){1, 2, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 1) || (luLevelNr == 15)) && (iCurRoom == 3))
				ShowImage (-14, (int[]){1, 3, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 1) || (luLevelNr == 15)) && (iCurRoom == 4))
				ShowImage (-14, (int[]){1, 4, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 1) || (luLevelNr == 15)) && (iCurRoom == 5))
				ShowImage (-14, (int[]){1, 5, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 1) || (luLevelNr == 15)) && (iCurRoom == 10))
				ShowImage (-14, (int[]){1, 10, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 1) || (luLevelNr == 15)) && (iCurRoom == 11))
				ShowImage (-14, (int[]){1, 11, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 1) || (luLevelNr == 15)) && (iCurRoom == 12))
				ShowImage (-14, (int[]){1, 12, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 1) || (luLevelNr == 15)) && (iCurRoom == 15))
				ShowImage (-14, (int[]){1, 15, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 1) || (luLevelNr == 15)) && (iCurRoom == 16))
				ShowImage (-14, (int[]){1, 16, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 1) || (luLevelNr == 15)) && (iCurRoom == 19))
				ShowImage (-14, (int[]){1, 19, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 2) || (luLevelNr == 16)) && (iCurRoom == 1))
				ShowImage (-14, (int[]){2, 1, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 2) || (luLevelNr == 16)) && (iCurRoom == 2))
				ShowImage (-14, (int[]){2, 2, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 2) || (luLevelNr == 16)) && (iCurRoom == 3))
				ShowImage (-14, (int[]){2, 3, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 14) || (luLevelNr == 28)) && (iCurRoom == 1))
				ShowImage (-14, (int[]){14, 1, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 14) || (luLevelNr == 28)) && (iCurRoom == 2))
				ShowImage (-14, (int[]){14, 2, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 14) || (luLevelNr == 28)) && (iCurRoom == 3))
				ShowImage (-14, (int[]){14, 3, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 14) || (luLevelNr == 28)) && (iCurRoom == 4))
				ShowImage (-14, (int[]){14, 4, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 14) || (luLevelNr == 28)) && (iCurRoom == 5))
				ShowImage (-14, (int[]){14, 5, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 14) || (luLevelNr == 28)) && (iCurRoom == 6))
				ShowImage (-14, (int[]){14, 6, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 14) || (luLevelNr == 28)) && (iCurRoom == 7))
				ShowImage (-14, (int[]){14, 7, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 14) || (luLevelNr == 28)) && (iCurRoom == 8))
				ShowImage (-14, (int[]){14, 8, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 6) || (luLevelNr == 20)) && (iCurRoom == 27))
				ShowImage (-14, (int[]){6, 27, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 9) || (luLevelNr == 23)) && (iCurRoom == 2))
				ShowImage (-14, (int[]){9, 2, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 9) || (luLevelNr == 23)) && (iCurRoom == 11))
				ShowImage (-14, (int[]){9, 11, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 9) || (luLevelNr == 23)) && (iCurRoom == 12))
				ShowImage (-14, (int[]){9, 12, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 9) || (luLevelNr == 23)) && (iCurRoom == 13))
				ShowImage (-14, (int[]){9, 13, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 9) || (luLevelNr == 23)) && (iCurRoom == 14))
				ShowImage (-14, (int[]){9, 14, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 9) || (luLevelNr == 23)) && (iCurRoom == 15))
				ShowImage (-14, (int[]){9, 15, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 9) || (luLevelNr == 23)) && (iCurRoom == 16))
				ShowImage (-14, (int[]){9, 16, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 13) || (luLevelNr == 27)) && (iCurRoom == 4))
				ShowImage (-14, (int[]){13, 4, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (((luLevelNr == 8) || (luLevelNr == 22)) && (iCurRoom == 9))
				ShowImage (-14, (int[]){8, 9, 0, 0}, screen, 42, 0, 0, 642, 380);
		}

		if ((cCurType == 'r') || (cCurType == 't')) { Backgrounds(); }

		/*** under this room ***/
		if (iRoomConnections[iCurRoom][4] != 0)
		{
			for (iTemp = 1; iTemp <= 10; iTemp++)
			{
				ShowImage (iThingA[iRoomConnections[iCurRoom][4]][iTemp - 1], (int[]){
					iModifierA[iRoomConnections[iCurRoom][4]][iTemp - 1][1],
					iModifierA[iRoomConnections[iCurRoom][4]][iTemp - 1][2],
					iModifierA[iRoomConnections[iCurRoom][4]][iTemp - 1][3],
					iModifierA[iRoomConnections[iCurRoom][4]][iTemp - 1][4]},
					screen, -4 - iTemp, 0, 0, 120, 155);
				/*** ShowImage (-11, (int[]){0, 0, 0, 0}, screen,
					-4 - iTemp, 0, 0, 120, 155); ***/
			}
		}

		for (iTemp = 1; iTemp <= 30; iTemp++)
		{
			/*** locations ***/
			if ((iTemp >= 1) && (iTemp <= 10)) { iLoc = 20 + iTemp; }
			if ((iTemp >= 11) && (iTemp <= 20)) { iLoc = iTemp; }
			if ((iTemp >= 21) && (iTemp <= 30)) { iLoc = -20 + iTemp; }
			ShowImage (iThingA[iCurRoom][iLoc - 1], (int[]){
				iModifierA[iCurRoom][iLoc - 1][1],
				iModifierA[iCurRoom][iLoc - 1][2],
				iModifierA[iCurRoom][iLoc - 1][3],
				iModifierA[iCurRoom][iLoc - 1][4]},
				screen, iLoc, 0, 0, 120, 155);
			/*** kid ***/
			if ((iCurRoom == (int)luKidRoom) && (iLoc == (int)luKidPos))
			{
				if (iEditPoP == 1)
				{
					ShowImage (-8, (int[]){1, 0, 0, 0}, screen, iLoc, 0, 0, 22, 84);
				} else {
					ShowImage (-8, (int[]){1, 0, 0, 0}, screen, iLoc, 0, 0, 24, 84);
				}
			}
			if (iEditPoP == 1)
			{
				/*** PoP1 guard ***/
				if (sGuardLocations[iCurRoom - 1] + 1 == iLoc)
				{
					ShowImage (-8, (int[]){2, 0, 0, 0}, screen, iLoc, 0, 0, 70, 86);
				}
			} else {
				/*** PoP2 guards ***/
				for (iGuard = 0; iGuard < iStaticGuards_Amount[iCurRoom - 1]; iGuard++)
				{
					switch (WhichStaticGuard (iGuard))
					{
						case 1: iGWidth = 68; iGHeight = 86; break;
						case 2: iGWidth = 68; iGHeight = 86; break;
						case 3: iGWidth = 68; iGHeight = 86; break;
						case 4: iGWidth = 76; iGHeight = 44; break;
						case 5: iGWidth = 84; iGHeight = 74; break;
						case 6: iGWidth = 76; iGHeight = 18; break;
						case 7: iGWidth = 78; iGHeight = 44; break;
						case 8: iGWidth = 30; iGHeight = 52; break;
						case 9: iGWidth = 30; iGHeight = 36; break;
						case 10: iGWidth = 98; iGHeight = 20; break;
						case 11: iGWidth = 76; iGHeight = 86; break;
						case 12: iGWidth = 46; iGHeight = 12; break;
						case 13: iGWidth = 22; iGHeight = 82; break;
					}
					ShowImage (-8, (int[]){4, iGuard, 0, 0},
						screen, 117, 0, 0, iGWidth, iGHeight);
				}
			}

			if (sMusic[((iCurRoom - 1) * 30) + iLoc - 1] == 1)
			{
				ShowImage (-17, (int[]){0, 0, 0, 0}, screen,
					iLoc, 0, 0, 38, 151);
			}
		}

		/*** foreground elements ***/
		if (((luLevelNr == 10) || (luLevelNr == 24)) && (iCurRoom == 22))
		{ /*** horse ***/
			ShowImage (-16, (int[]){1, 0, 0, 0}, screen, 118, 0, 0, 154, 168);
		}
		if (((luLevelNr == 6) || (luLevelNr == 20)) && (iCurRoom == 27))
		{ /*** arrow ***/
			ShowImage (-16, (int[]){2, 0, 0, 0}, screen, 118, 0, 0, 52, 41);
		}
		if (cCurType == 'o')
		{ /*** invisible ***/
			ShowImage (-16, (int[]){3, 0, 0, 0}, screen, 118, 0, 0, 245, 16);
		}

		/*** incoming arrows ***/
		if (iDynamicGuards_Sets[iCurRoom - 1] > 0)
		{
			ShowImage (-8, (int[]){3, 0, 0, 0}, screen, 116, 0, 0, 13, 26);
		}
		if (iDynamicGuards_Sets[iCurRoom - 1] > 1)
		{
			ShowImage (-8, (int[]){3, 1, 0, 0}, screen, 116, 0, 0, 13, 26);
		}
	}
	if (iScreen == 2) /*** R ***/
	{
		if (iBrokenRoomLinks == 0)
		{
			InitRooms();
			/*** room links ***/
			ShowImage (-6, (int[]){0, 0, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (iEditPoP == 2)
			{
				ShowImage (-6, (int[]){35, 0, 0, 0}, screen, 128, 0, 0, 112, 131);
			}
			/*** rooms broken on ***/
			ShowImage (-2, (int[]){8, 0, 0, 0}, screen, 87, 0, 0, 25, 25);
			WhereToStart ();
			for (iTemp = 1; iTemp <= iRooms; iTemp++)
			{
				iDone[iTemp] = 0;
			}
			ShowRooms ((int)luKidRoom, iStartRoomsX, iStartRoomsY, 1);
			iUnusedRooms = 0;
			for (iTemp = 1; iTemp <= iRooms; iTemp++)
			{
				if (iDone[iTemp] != 1)
				{
					iUnusedRooms++;
					ShowRooms (iTemp, 25, iUnusedRooms, 0);
				}
			}
			if (iMovingRoom != 0)
			{
				ShowImage (-6, (int[]){iMovingRoom, 0, 0, 0}, screen,
					83, 0, 0, 14, 14);
				if (iCurRoom == iMovingRoom)
				{
					ShowImage (-7, (int[]){0, 0, 0, 0}, screen, 83, 0, 0, 14, 14);
				}
				if ((int)luKidRoom == iMovingRoom)
				{
					ShowImage (-7, (int[]){1, 0, 0, 0}, screen, 83, 0, 0, 14, 14);
				}
				ShowImage (-7, (int[]){2, 0, 0, 0}, screen, 83, 0, 0, 14, 14);
				ShowRooms (-1, iMovingNewX, iMovingNewY, 0);
			}
		} else {
			/*** broken room links ***/
			ShowImage (-6, (int[]){33, 0, 0, 0}, screen, 42, 0, 0, 642, 380);
			if (iEditPoP == 2)
			{
				ShowImage (-6, (int[]){34, 0, 0, 0}, screen, 127, 0, 0, 378, 139);
			}
			for (iRoom = 1; iRoom <= iRooms; iRoom++)
			{
				if (iCurRoom == iRoom)
				{
					ShowImage (-7, (int[]){0, 0, 0, 0}, screen,
						BrokenRoomChange (iRoom, 0), 0, 0, 14, 14);
				}
				if ((int)luKidRoom == iRoom)
				{
					ShowImage (-7, (int[]){1, 0, 0, 0}, screen,
						BrokenRoomChange (iRoom, 0), 0, 0, 14, 14);
				}
				for (iSide = 1; iSide <= 4; iSide++)
				{
					if (iRoomConnections[iRoom][iSide] != 0)
					{
						ShowImage (-6, (int[]){iRoomConnections[iRoom][iSide], 0, 0, 0},
							screen, BrokenRoomChange (iRoom, iSide), 0, 0, 14, 14);
						if (iRoomConnectionsBroken[iRoom][iSide] == 1)
						{
							ShowImage (-7, (int[]){4, 0, 0, 0}, screen,
								BrokenRoomChange (iRoom, iSide), 0, 0, 14, 14);
						}
					}
					if ((iChangingBrokenRoom == iRoom) && (iChangingBrokenSide == iSide))
					{
						ShowImage (-7, (int[]){2, 0, 0, 0}, screen,
							BrokenRoomChange (iRoom, iSide), 0, 0, 14, 14);
					}
				}
			}
		}
	}
	if (iScreen == 3) /*** E ***/
	{
		/*** events screen ***/
		ShowImage (-10, (int[]){0, 0, 0, 0}, screen, 42, 0, 0, 642, 380);

		/*** current event number ***/
		CenterNumber (screen, iChangeEvent + 1, 371, 60, color_wh, color_bl);

		/*** sel event, room ***/
		if (((EventInfo (iChangeEvent, 1) >= 1) &&
			(EventInfo (iChangeEvent, 1) <= 24)) || (iEditPoP == 2))
		{
			ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 49, 0, 0, 14, 14);
		}

		/*** sel event, location ***/
		if (((EventInfo (iChangeEvent, 2) >= 1) &&
			(EventInfo (iChangeEvent, 2) <= 30)) || (iEditPoP == 2))
		{
			ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 50, 0, 0, 14, 14);
		}

		/*** sel event, next ***/
		if (iEditPoP == 1)
		{
			if ((EventInfo (iChangeEvent, 3) == 0) ||
				(EventInfo (iChangeEvent, 3) == 1))
			{
				ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 51, 0, 0, 14, 14);
			}
		}

		/*** Show target. ***/
		if (iEditPoP == 1)
		{
			iTargetRoom = EventInfo (iChangeEvent, 1);
			iTargetTile = EventInfo (iChangeEvent, 2) - 1;
			iTargetW = 120;
			iTargetH = 155;
		} else {
			iTargetRoom = iPoP2DoorRoom[iChangeEvent];
			iTargetTile = iPoP2DoorLocation[iChangeEvent];
			iTargetW = 116;
			iTargetH = 154;
		}
		if ((cCurType == 'r') || (cCurType == 't'))
		{
			ShowBackground ((int[]){iModifierA[iTargetRoom][iTargetTile][1],
				iModifierA[iTargetRoom][iTargetTile][2],
				iModifierA[iTargetRoom][iTargetTile][3],
				iModifierA[iTargetRoom][iTargetTile][4]}, 101);
		}
		ShowImage (iThingA[iTargetRoom][iTargetTile], (int[]){
			iModifierA[iTargetRoom][iTargetTile][1],
			iModifierA[iTargetRoom][iTargetTile][2],
			iModifierA[iTargetRoom][iTargetTile][3],
			iModifierA[iTargetRoom][iTargetTile][4]},
			screen, 92, 0, 0, iTargetW, iTargetH);
		if (iEditPoP == 2)
		{
			if (sMusic[((iTargetRoom - 1) * 30) + iTargetTile] == 1)
			{
				ShowImage (-17, (int[]){0, 0, 0, 0}, screen,
					92, 0, 0, 38, 151);
			}
		}

		if (iEditPoP == 2)
		{
			/*** left ***/
			CenterNumber (screen, iPoP2DoorLeft[iChangeEvent] + 1, 347, 224,
				color_wh, color_bl);

			/*** right ***/
			CenterNumber (screen, iPoP2DoorRight[iChangeEvent] + 1, 572, 224,
				color_wh, color_bl);

			/*** edit left ***/
			if (iEditLeftOn == 1)
			{ /*** on ***/
				ShowImage (-12, (int[]){43, 1, 0, 0}, screen, 121, 0, 0, 85, 32);
			} else { /*** off ***/
				ShowImage (-12, (int[]){42, 1, 0, 0}, screen, 121, 0, 0, 85, 32);
			}

			/*** end left ***/
			if (iEndLeftOn == 1)
			{ /*** on ***/
				ShowImage (-12, (int[]){45, 2, 0, 0}, screen, 121, 0, 0, 85, 32);
			} else { /*** off ***/
				ShowImage (-12, (int[]){44, 2, 0, 0}, screen, 121, 0, 0, 85, 32);
			}

			/*** edit right ***/
			if (iEditRightOn == 1)
			{ /*** on ***/
				ShowImage (-12, (int[]){43, 3, 0, 0}, screen, 121, 0, 0, 85, 32);
			} else { /*** off ***/
				ShowImage (-12, (int[]){42, 3, 0, 0}, screen, 121, 0, 0, 85, 32);
			}

			/*** end right ***/
			if (iEndRightOn == 1)
			{ /*** on ***/
				ShowImage (-12, (int[]){45, 4, 0, 0}, screen, 121, 0, 0, 85, 32);
			} else { /*** off ***/
				ShowImage (-12, (int[]){44, 4, 0, 0}, screen, 121, 0, 0, 85, 32);
			}

			/*** checkpoints ***/
			CenterNumber (screen, iCheckPoints[0], 319, 301, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[1], 404, 301, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[2], 319, 325, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[3], 404, 325, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[4], 319, 349, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[5], 404, 349, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[6], 319, 373, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[7], 404, 373, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[8], 319, 397, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[9], 404, 397, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[10], 497, 325, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[11], 582, 325, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[12], 497, 349, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[13], 582, 349, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[14], 497, 373, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[15], 582, 373, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[16], 497, 397, color_wh, color_bl);
			CenterNumber (screen, iCheckPoints[17], 582, 397, color_wh, color_bl);
		}
	}

	if (iRoomConnections[iCurRoom][1] != 0)
	{
		/*** left ***/
		ShowImage (-1, (int[]){1, 0, 0, 0}, screen, 32, 0, 0, 25, 380);
	} else {
		/*** no left ***/
		ShowImage (-1, (int[]){6, 0, 0, 0}, screen, 32, 0, 0, 25, 380);
	}
	if (iRoomConnections[iCurRoom][2] != 0)
	{
		/*** right ***/
		ShowImage (-1, (int[]){2, 0, 0, 0}, screen, 33, 0, 0, 25, 380);
	} else {
		/*** no right ***/
		ShowImage (-1, (int[]){6, 0, 0, 0}, screen, 33, 0, 0, 25, 380);
	}
	if (iRoomConnections[iCurRoom][3] != 0)
	{
		/*** up ***/
		ShowImage (-1, (int[]){3, 0, 0, 0}, screen, 34, 0, 0, 642, 25);
	} else {
		if (iScreen != 1)
		{
			/*** no up ***/
			ShowImage (-1, (int[]){5, 0, 0, 0}, screen, 34, 0, 0, 642, 25);
		} else {
			/*** no up, with info ***/
			ShowImage (-1, (int[]){7, 0, 0, 0}, screen, 34, 0, 0, 642, 25);
		}
	}
	if (iRoomConnections[iCurRoom][4] != 0)
	{
		/*** down ***/
		ShowImage (-1, (int[]){4, 0, 0, 0}, screen, 35, 0, 0, 642, 25);
	} else {
		/*** no down ***/
		ShowImage (-1, (int[]){5, 0, 0, 0}, screen, 35, 0, 0, 642, 25);
	}
	switch (iScreen)
	{
		case 1:
			if (iBrokenRoomLinks == 1)
			{
				/*** rooms broken on ***/
				ShowImage (-2, (int[]){8, 0, 0, 0}, screen, 36, 0, 0, 25, 25);
			} else {
				/*** rooms on ***/
				ShowImage (-2, (int[]){1, 0, 0, 0}, screen, 36, 0, 0, 25, 25);
			}
			/*** events on ***/
			ShowImage (-2, (int[]){2, 0, 0, 0}, screen, 37, 0, 0, 25, 25);
			break;
		case 2:
			if (iBrokenRoomLinks == 1)
			{
				/*** rooms broken off ***/
				ShowImage (-2, (int[]){9, 0, 0, 0}, screen, 36, 0, 0, 25, 25);
			} else {
				/*** rooms off ***/
				ShowImage (-2, (int[]){6, 0, 0, 0}, screen, 36, 0, 0, 25, 25);
			}
			/*** events on ***/
			ShowImage (-2, (int[]){2, 0, 0, 0}, screen, 37, 0, 0, 25, 25);
			break;
		case 3:
			if (iBrokenRoomLinks == 1)
			{
				/*** rooms broken on ***/
				ShowImage (-2, (int[]){8, 0, 0, 0}, screen, 36, 0, 0, 25, 25);
			} else {
				/*** rooms on ***/
				ShowImage (-2, (int[]){1, 0, 0, 0}, screen, 36, 0, 0, 25, 25);
			}
			/*** events off ***/
			ShowImage (-2, (int[]){7, 0, 0, 0}, screen, 37, 0, 0, 25, 25);
			break;
	}
	if (iChanged != 0)
	{
		/*** save on ***/
		ShowImage (-2, (int[]){3, 0, 0, 0}, screen, 38, 0, 0, 25, 25);
	} else {
		/*** save off ***/
		ShowImage (-2, (int[]){5, 0, 0, 0}, screen, 38, 0, 0, 25, 25);
	}
	/*** quit ***/
	ShowImage (-2, (int[]){4, 0, 0, 0}, screen, 39, 0, 0, 25, 25);
	if ((int)luLevelNr != 1)
	{
		/*** previous on ***/
		ShowImage (-5, (int[]){1, 0, 0, 0}, screen, 31, 0, 0, 25, 25);
	} else {
		/*** previous off ***/
		ShowImage (-5, (int[]){4, 0, 0, 0}, screen, 31, 0, 0, 25, 25);
	}
	if (((int)luLevelNr != 0) && ((int)luLevelNr != 28))
	{
		/*** next on ***/
		ShowImage (-5, (int[]){2, 0, 0, 0}, screen, 40, 0, 0, 25, 25);
	} else {
		/*** next off ***/
		ShowImage (-5, (int[]){5, 0, 0, 0}, screen, 40, 0, 0, 25, 25);
	}
	/*** level bar ***/
	if (iLvl10Warning == 1)
	{
		ShowImage (-5, (int[]){19, 0, 0, 0}, screen, 41, 0, 0, 642, 25);
	} else {
		ShowImage (-5, (int[]){3, 0, 0, 0}, screen, 41, 0, 0, 642, 25);
	}

	if (iEditPoP == 1)
	{
		switch (luLevelNr)
		{
			case 1:
				snprintf (sTemp, MAX_TEXT, "%s", "level 1 (dungeon; prison),"); break;
			case 2:
				snprintf (sTemp, MAX_TEXT, "%s", "level 2 (dungeon; guards),"); break;
			case 3:
				snprintf (sTemp, MAX_TEXT, "level 3 (dungeon; skeleton),"); break;
			case 4:
				snprintf (sTemp, MAX_TEXT, "level 4 (palace; mirror),"); break;
			case 5:
				snprintf (sTemp, MAX_TEXT, "level 5 (palace; thief),"); break;
			case 6:
				snprintf (sTemp, MAX_TEXT, "level 6 (palace; plunge),"); break;
			case 7:
				snprintf (sTemp, MAX_TEXT, "level 7 (dungeon; weightless),"); break;
			case 8:
				snprintf (sTemp, MAX_TEXT, "level 8 (dungeon; mouse),"); break;
			case 9:
				snprintf (sTemp, MAX_TEXT, "level 9 (dungeon; twisty),"); break;
			case 10:
				snprintf (sTemp, MAX_TEXT, "level 10 (palace; quad),"); break;
			case 11:
				snprintf (sTemp, MAX_TEXT, "level 11 (palace; fragile),"); break;
			case 12:
				snprintf (sTemp, MAX_TEXT, "level 12 (12a; dungeon; tower),"); break;
			case 13:
				snprintf (sTemp, MAX_TEXT, "level 13 (12b; dungeon; jaffar),"); break;
			case 14:
				snprintf (sTemp, MAX_TEXT, "level 14 (palace; rescue),"); break;
			case 15:
				snprintf (sTemp, MAX_TEXT, "level 15 (dungeon; potions),"); break;
			case 0:
				snprintf (sTemp, MAX_TEXT, "level 0 (dungeon; demo),"); break;
		}
	} else {
		if (luLevelNr > 14)
			{ snprintf (sGamePlay, MAX_TEXT, "%s", "; gameplay"); }
				else { snprintf (sGamePlay, MAX_TEXT, "%s", ""); }
		switch (luLevelNr)
		{
			case 1: case 15:
				snprintf (sLevelType, MAX_TEXT, "%s", "rooftops"); break;
			case 2: case 16:
				snprintf (sLevelType, MAX_TEXT, "%s", "desert"); break;
			case 3: case 4: case 5: case 17: case 18: case 19:
				snprintf (sLevelType, MAX_TEXT, "%s", "caverns"); break;
			case 6: case 7: case 8: case 9: case 20: case 21: case 22: case 23:
				snprintf (sLevelType, MAX_TEXT, "%s", "ruins"); break;
			case 10: case 11: case 12: case 13: case 24: case 25: case 26: case 27:
				snprintf (sLevelType, MAX_TEXT, "%s", "temple"); break;
			case 14: case 28:
				snprintf (sLevelType, MAX_TEXT, "%s", "final"); break;
		}
		snprintf (sTemp, MAX_TEXT, "level %lu (%s%s),",
			luLevelNr, sLevelType, sGamePlay);
	}
	switch (iScreen)
	{
		case 1:
			snprintf (sText, MAX_TEXT, "%s room %i", sTemp, iCurRoom);
			ShowImage (-5, (int[]){iExtras + 10, 0, 0, 0}, screen, 93, 0, 0, 39, 19);
			break;
		case 2:
			snprintf (sText, MAX_TEXT, "%s room links", sTemp); break;
		case 3:
			snprintf (sText, MAX_TEXT, "%s events", sTemp); break;
	}

	if (iLvl10Warning != 1)
	{
		message = TTF_RenderText_Shaded (font1, sText, color_bl, color_wh);
		offset.x = 31;
		offset.y = 5;
		CustomBlitSurface (message, "message", NULL, screen, &offset, arFont);
	}

	/*** refresh screen ***/
	if (SDL_Flip (screen) == -1)
		{ printf ("[FAILED] SDL_Flip error!\n"); exit (EXIT_ERROR); }
}
/*****************************************************************************/
void Prev (int iCurLevel)
/*****************************************************************************/
{
	int iToLoad;

	iToLoad = -1;
	if (iEditPoP == 1)
	{
		if (iCurLevel != 1)
		{
			iToLoad = iCurLevel - 1;
			if (iToLoad == -1) { iToLoad = 15; }
		}
	} else {
		if (iCurLevel != 1) { iToLoad = iCurLevel - 1; }
	}
	if (iToLoad != -1)
	{
		LoadLevel (iToLoad);
		PlaySound ("wav/level_change.wav");
	}
}
/*****************************************************************************/
void Next (int iCurLevel)
/*****************************************************************************/
{
	int iToLoad;

	iToLoad = -1;
	if (iEditPoP == 1)
	{
		if (iCurLevel != 0)
		{
			iToLoad = iCurLevel + 1;
			if (iToLoad == 16) { iToLoad = 0; }
		}
	} else {
		if (iCurLevel != 28) { iToLoad = iCurLevel + 1; }
	}
	if (iToLoad != -1)
	{
		LoadLevel (iToLoad);
		PlaySound ("wav/level_change.wav");
	}
}
/*****************************************************************************/
void WhereToStart (void)
/*****************************************************************************/
{
	int iTemp;
	iMinX = 0;
	iMaxX = 0;
	iMinY = 0;
	iMaxY = 0;

	for (iTemp = 1; iTemp <= iRooms; iTemp++)
	{
		iDone[iTemp] = 0;
	}
	CheckSides ((int)luKidRoom, 0, 0);

	iStartRoomsX = 12 - round ((iMinX + iMaxX) / 2);
	iStartRoomsY = 12 - round ((iMinY + iMaxY) / 2);
}
/*****************************************************************************/
void CheckSides (int iRoom, int iX, int iY)
/*****************************************************************************/
{
	if (iX < iMinX) { iMinX = iX; }
	if (iY < iMinY) { iMinY = iY; }
	if (iX > iMaxX) { iMaxX = iX; }
	if (iY > iMaxY) { iMaxY = iY; }

	iDone[iRoom] = 1;

	if ((iRoomConnections[iRoom][1] != 0) && (iDone[iRoomConnections[iRoom][1]]
		!= 1)) { CheckSides (iRoomConnections[iRoom][1], iX - 1, iY); }
	if ((iRoomConnections[iRoom][2] != 0) && (iDone[iRoomConnections[iRoom][2]]
		!= 1)) { CheckSides (iRoomConnections[iRoom][2], iX + 1, iY); }
	if ((iRoomConnections[iRoom][3] != 0) && (iDone[iRoomConnections[iRoom][3]]
		!= 1)) { CheckSides (iRoomConnections[iRoom][3], iX, iY - 1); }
	if ((iRoomConnections[iRoom][4] != 0) && (iDone[iRoomConnections[iRoom][4]]
		!= 1)) { CheckSides (iRoomConnections[iRoom][4], iX, iY + 1); }
}
/*****************************************************************************/
void ShowRooms (int iRoom, int iX, int iY, int iNext)
/*****************************************************************************/
{
	char sOnGrid[MAX_GRID + 2];
	char sTemp[MAX_GRID + 2];

	if (iX < 10)
	{
		snprintf (sTemp, MAX_GRID, "10%i", iX);
	} else {
		snprintf (sTemp, MAX_GRID, "1%i", iX);
	}
	if (iY < 10)
	{
		snprintf (sOnGrid, MAX_GRID, "%s0%i", sTemp, iY);
	} else {
		snprintf (sOnGrid, MAX_GRID, "%s%i", sTemp, iY);
	}
	if (iRoom != -1)
	{
		ShowImage (-6, (int[]){iRoom, 0, 0, 0}, screen,
			atoi (sOnGrid), 0, 0, 14, 14);
		if (iCurRoom == iRoom)
		{
			ShowImage (-7, (int[]){0, 0, 0, 0}, screen,
				atoi (sOnGrid), 0, 0, 14, 14);
		}
		if ((int)luKidRoom == iRoom)
		{
			ShowImage (-7, (int[]){1, 0, 0, 0}, screen,
				atoi (sOnGrid), 0, 0, 14, 14);
		}
		if (iRoom == iMovingRoom)
		{
			ShowImage (-7, (int[]){2, 0, 0, 0}, screen,
				atoi (sOnGrid), 0, 0, 14, 14);
		}
	} else {
		ShowImage (-7, (int[]){3, 0, 0, 0}, screen, atoi (sOnGrid), 0, 0, 14, 14);
	}
	if (iRoom == iMovingRoom)
	{
		iMovingOldX = iX;
		iMovingOldY = iY;
		if (iMovingNewBusy == 0)
		{
			iMovingNewX = iMovingOldX;
			iMovingNewY = iMovingOldY;
			iMovingNewBusy = 1;
		}
	}

	iDone[iRoom] = 1;

	if (iNext == 1)
	{
		if ((iRoomConnections[iRoom][1] != 0) && (iDone[iRoomConnections[iRoom][1]]
			!= 1)) { ShowRooms (iRoomConnections[iRoom][1], iX - 1, iY, 1); }
		if ((iRoomConnections[iRoom][2] != 0) && (iDone[iRoomConnections[iRoom][2]]
			!= 1)) { ShowRooms (iRoomConnections[iRoom][2], iX + 1, iY, 1); }
		if ((iRoomConnections[iRoom][3] != 0) && (iDone[iRoomConnections[iRoom][3]]
			!= 1)) { ShowRooms (iRoomConnections[iRoom][3], iX, iY - 1, 1); }
		if ((iRoomConnections[iRoom][4] != 0) && (iDone[iRoomConnections[iRoom][4]]
			!= 1)) { ShowRooms (iRoomConnections[iRoom][4], iX, iY + 1, 1); }
	}
}
/*****************************************************************************/
void InitPopUp (int iAtScreen, int iStart)
/*****************************************************************************/
{
	SDL_Event event;
	int iPopUp;

	iPopUp = 1;

	if (iStart == 1) { PlaySound ("wav/popup.wav"); }
	ShowPopUp();
	while (iPopUp == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
						case SDLK_o:
							iPopUp = 0;
						default: break;
					}
					break;
				case SDL_MOUSEMOTION:
					SDL_GetMouseState (&iXPos, &iYPos);
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (event, 440, 317, 440 + 85, 317 + 32) == 1) /*** OK ***/
						{
							if (iOKOn != 1) { iOKOn = 1; }
							ShowPopUp();
						}
					}
					break;
				case SDL_MOUSEBUTTONUP:
					iOKOn = 0;
					if (event.button.button == 1)
					{
						if (InArea (event, 440, 317, 440 + 85, 317 + 32) == 1) /*** OK ***/
						{
							iPopUp = 0;
						}
					}
					ShowPopUp(); break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/popup_close.wav");
	ShowScreen (iAtScreen);
}
/*****************************************************************************/
void ShowPopUp (void)
/*****************************************************************************/
{
	char sText[9 + 2][MAX_TEXT + 2];

	/*** faded background ***/
	ShowImage (-9, (int[]){1, 0, 0, 0}, screen, 31, 0, 0, 692, 455);
	/*** popup ***/
	ShowImage (-9, (int[]){2, 0, 0, 0}, screen, 44, 0, 0, 524, 361);
	switch (iOKOn)
	{
		case 0:
			/*** OK off ***/
			ShowImage (-9, (int[]){3, 0, 0, 0}, screen, 45, 0, 0, 85, 32); break;
		case 1:
			/*** OK on ***/
			ShowImage (-9, (int[]){4, 0, 0, 0}, screen, 45, 0, 0, 85, 32); break;
	}

	snprintf (sText[0], MAX_TEXT, "%s %s", EDITOR_NAME, EDITOR_VERSION);
	snprintf (sText[1], MAX_TEXT, "%s", COPYRIGHT);
	snprintf (sText[2], MAX_TEXT, "%s", "");
	snprintf (sText[3], MAX_TEXT, "%s", "single tile (change or select)");
	snprintf (sText[4], MAX_TEXT, "%s", "entire room (clear or fill)");
	snprintf (sText[5], MAX_TEXT, "%s", "entire level (randomize or fill)");
	snprintf (sText[6], MAX_TEXT, "%s", "");
	if (iEditPoP == 1)
	{
		snprintf (sText[7], MAX_TEXT, "%s", "You may use one guard per room.");
	} else {
		snprintf (sText[7], MAX_TEXT, "%s", "You may use five guards per room.");
	}
	snprintf (sText[8], MAX_TEXT, "%s", "The tile behavior may differ per"
		" level.");

	DisplayText (180, 118, FONT_SIZE_1, sText, 9);

	/*** refresh screen ***/
	if (SDL_Flip (screen) == -1)
		{ printf ("[FAILED] SDL_Flip error!\n"); exit (EXIT_ERROR); }
}
/*****************************************************************************/
void InitPopUpSave (int iAtScreen)
/*****************************************************************************/
{
	SDL_Event event;
	int iPopUp;

	iPopUp = 1;

	PlaySound ("wav/popup_yn.wav");
	ShowPopUpSave();
	while (iPopUp == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						case SDLK_n:
							iPopUp = 0;
							break;
						case SDLK_y:
							CallSave (0);
							iPopUp = 0;
							break;
						default: break;
					}
					break;
				case SDL_MOUSEMOTION:
					SDL_GetMouseState (&iXPos, &iYPos);
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (event, 440, 317, 440 + 85, 317 + 32) == 1)
						{ /*** Yes ***/
							if (iYesOn != 1) { iYesOn = 1; }
							ShowPopUpSave();
						}
						if (InArea (event, 167, 317, 167 + 85, 317 + 32) == 1)
						{ /*** No ***/
							if (iNoOn != 1) { iNoOn = 1; }
							ShowPopUpSave();
						}
					}
					break;
				case SDL_MOUSEBUTTONUP:
					iYesOn = 0;
					iNoOn = 0;
					if (event.button.button == 1)
					{
						if (InArea (event, 440, 317, 440 + 85, 317 + 32) == 1)
						{ /*** Yes ***/
							CallSave (0);
							iPopUp = 0;
						}
						if (InArea (event, 167, 317, 167 + 85, 317 + 32) == 1)
						{ /*** No ***/
							iPopUp = 0;
						}
					}
					ShowPopUpSave(); break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/popup_close.wav");
	ShowScreen (iAtScreen);
}
/*****************************************************************************/
void ShowPopUpSave (void)
/*****************************************************************************/
{
	char sText[2 + 2][MAX_TEXT + 2];

	/*** faded background ***/
	ShowImage (-9, (int[]){1, 0, 0, 0}, screen, 31, 0, 0, 692, 455);
	/*** popup_yn ***/
	ShowImage (-9, (int[]){5, 0, 0, 0}, screen, 84, 0, 0, 392, 277);
	switch (iYesOn)
	{
		case 0:
			/*** Yes off ***/
			ShowImage (-9, (int[]){6, 0, 0, 0}, screen, 85, 0, 0, 85, 32); break;
		case 1:
			/*** Yes on ***/
			ShowImage (-9, (int[]){7, 0, 0, 0}, screen, 85, 0, 0, 85, 32); break;
	}
	switch (iNoOn)
	{
		case 0:
			/*** No off ***/
			ShowImage (-9, (int[]){8, 0, 0, 0}, screen, 86, 0, 0, 85, 32); break;
		case 1:
			/*** No on ***/
			ShowImage (-9, (int[]){9, 0, 0, 0}, screen, 86, 0, 0, 85, 32); break;
	}

	if (iChanged == 1)
	{
		snprintf (sText[0], MAX_TEXT, "%s", "You made an unsaved change.");
		snprintf (sText[1], MAX_TEXT, "%s", "Do you want to save it?");
	} else {
		snprintf (sText[0], MAX_TEXT, "There are unsaved changes.");
		snprintf (sText[1], MAX_TEXT, "%s", "Do you wish to save these?");
	}

	DisplayText (180, 118, FONT_SIZE_1, sText, 2);

	/*** refresh screen ***/
	if (SDL_Flip (screen) == -1)
		{ printf ("[FAILED] SDL_Flip error!\n"); exit (EXIT_ERROR); }
}
/*****************************************************************************/
void DisplayText (int iStartX, int iStartY, int iFontSize,
	char sText[9 + 2][MAX_TEXT + 2], int iLines)
/*****************************************************************************/
{
	int iTemp;

	for (iTemp = 0; iTemp <= (iLines - 1); iTemp++)
	{
		if (strcmp (sText[iTemp], "") != 0)
		{
			message = TTF_RenderText_Shaded (font1,
				sText[iTemp], color_bl, color_wh);
			if ((iTemp == 3) || (iTemp == 4) || (iTemp == 5))
			{
				offset.x = iStartX + 20;
			} else {
				offset.x = iStartX;
			}
			offset.y = iStartY + (iTemp * (iFontSize + 4));
			CustomBlitSurface (message, "message", NULL, screen, &offset, arFont);
		}
	}
}
/*****************************************************************************/
void ShowPos (SDL_Rect loc, SDL_Rect dest, int iLocation,
	SDL_Surface *img_first[2 + 2], SDL_Surface *img_second[2 + 2], char *sLbl)
/*****************************************************************************/
{
	if (cCurType == 'd')
	{
		CustomBlitSurface (img_first[1], "img_first[1]", &loc,
			screen, &dest, arImage);
	} else {
		CustomBlitSurface (img_second[1], "img_second[1]", &loc,
			screen, &dest, arImage);
	}

	if (strcmp (sLbl, "") != 0)
	{
		message = TTF_RenderText_Shaded (font2, sLbl, color_bl, color_wh);
		offset.x = dest.x + 7;
		offset.y = dest.y + 145 - FONT_SIZE_2;
		CustomBlitSurface (message, "message", NULL, screen, &offset, arFont);
	}

	if (iSelected == iLocation)
	{
		if (cCurType == 'd')
		{
			CustomBlitSurface (img_first[2], "img_first[2]", &loc,
				screen, &dest, arImage);
		} else {
			CustomBlitSurface (img_second[2], "img_second[2]", &loc,
				screen, &dest, arImage);
		}
	}

	iPosShown = 1;
}
/*****************************************************************************/
void ShowPosPoP2 (SDL_Rect loc, SDL_Rect dest, int iLocation,
	SDL_Surface *img[2 + 2], char *sLbl)
/*****************************************************************************/
{
	CustomBlitSurface (img[1], "img[1]", &loc, screen, &dest, arImage);

	if (strcmp (sLbl, "") != 0)
	{
		message = TTF_RenderText_Shaded (font2, sLbl, color_bl, color_wh);
		offset.x = dest.x + 7;
		offset.y = dest.y + 145 - FONT_SIZE_2;
		CustomBlitSurface (message, "message", NULL, screen, &offset, arFont);
	}

	if (iSelected == iLocation)
	{
		CustomBlitSurface (img[2], "img[2]", &loc, screen, &dest, arImage);
	}

	iPosShown = 1;
}
/*****************************************************************************/
void ShowDetail (SDL_Surface *img[2 + 2], SDL_Rect loc,
	SDL_Surface *screen, SDL_Rect dest, int iLocation)
/*****************************************************************************/
{
	CustomBlitSurface (img[1], "img[1]", &loc, screen, &dest, arImage);
	if (iSelected == iLocation)
	{
		CustomBlitSurface (img[2], "img[2]", &loc, screen, &dest, arImage);
	}
}
/*****************************************************************************/
void ChangePos (int iLocation, SDL_Surface *screen)
/*****************************************************************************/
{
	SDL_Event event;
	int iChanging;
	int iTemp;
	int iTemp2;
	int iNowOn;
	int iOldXPos;
	int iOldYPos;
	int iUseTile;
	int iWallChanged;
	int iMod2Ch;

	iChanging = 1;
	iGuardSkill = 0;

	ShowChange (iLocation);
	while (iChanging == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					iTemp = 0;
					switch (event.key.keysym.sym)
					{
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
							if ((event.key.keysym.mod & KMOD_LCTRL) ||
								(event.key.keysym.mod & KMOD_RCTRL))
							{
								if ((iOnTile >= 1) && ((iOnTile <= 72) ||
									((iOnTile <= (7 * 13)) && (iEditPoP == 2))))
								{
									for (iTemp2 = 1; iTemp2 <= iRooms; iTemp2++)
									{
										for (iTemp = 1; iTemp <= 30; iTemp++)
											{ UseTile (iOnTile, iTemp, iTemp2); }
									}
									iChanging = 0;
									iChanged++;
								}
							} else if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								if ((iOnTile >= 1) && ((iOnTile <= 72) ||
									((iOnTile <= (7 * 13)) && (iEditPoP == 2))))
								{
									for (iTemp = 1; iTemp <= 30; iTemp++)
										{ UseTile (iOnTile, iTemp, iCurRoom); }
									iChanging = 0;
									iChanged++;
								}
							} else if (iOnTile != 0) {
								UseTile (iOnTile, iLocation, iCurRoom);
								if ((iOnTile <= 72) || ((iOnTile <= (7 * 13))
									&& (iEditPoP == 2))) { iChanging = 0; }
								iChanged++;
							}
							break;
						case SDLK_ESCAPE:
						case SDLK_q:
							iChanging = 0; break;
						case SDLK_c:
							if (cCurType != 'r')
							{
								iChanging = 0;
							} else {
								switch (iChangeCrk)
								{
									case 1: iChangeCrk = 3; break;
									case 12: iChangeCrk = 0; break;
									case 2: case 13: case 14: case 15:
										printf ("[ WARN ] Strange crack value: %i\n",
											iChangeCrk);
										iChangeCrk = 1; break;
									default: iChangeCrk++; break;
								}
								PlaySound ("wav/check_box.wav");
								UpdateMod2IfNecessary (iLocation);
							}
							break;
						case SDLK_0:
						case SDLK_KP0:
							if ((iEditPoP == 1) && (iGuardSkill != 0))
								{ iGuardSkill = 0; iTemp = 1; } break;
						case SDLK_1:
						case SDLK_KP1:
							if ((iEditPoP == 1) && (iGuardSkill != 1))
								{ iGuardSkill = 1; iTemp = 1; }
							if ((iEditPoP == 2) && (iChangeJumps != 1))
							{
								iChangeJumps = 1;
								PlaySound ("wav/check_box.wav");
								ApplyJumpsFrameIfNecessary (iLocation);
							}
							break;
						case SDLK_2:
						case SDLK_KP2:
							if ((iEditPoP == 1) && (iGuardSkill != 2))
								{ iGuardSkill = 2; iTemp = 1; }
							if ((iEditPoP == 2) && (iChangeJumps != 2))
							{
								iChangeJumps = 2;
								PlaySound ("wav/check_box.wav");
								ApplyJumpsFrameIfNecessary (iLocation);
							}
							break;
						case SDLK_3:
						case SDLK_KP3:
							if ((iEditPoP == 1) && (iGuardSkill != 3))
								{ iGuardSkill = 3; iTemp = 1; }
							if ((iEditPoP == 2) && (iChangeJumps != 3))
							{
								iChangeJumps = 3;
								PlaySound ("wav/check_box.wav");
								ApplyJumpsFrameIfNecessary (iLocation);
							}
							break;
						case SDLK_4:
						case SDLK_KP4:
							if ((iEditPoP == 1) && (iGuardSkill != 4))
								{ iGuardSkill = 4; iTemp = 1; }
							if ((iEditPoP == 2) && (iChangeJumps != 4))
							{
								iChangeJumps = 4;
								PlaySound ("wav/check_box.wav");
								ApplyJumpsFrameIfNecessary (iLocation);
							}
							break;
						case SDLK_5:
						case SDLK_KP5:
							if ((iEditPoP == 1) && (iGuardSkill != 5))
								{ iGuardSkill = 5; iTemp = 1; } break;
						case SDLK_6:
						case SDLK_KP6:
							if ((iEditPoP == 1) && (iGuardSkill != 6))
								{ iGuardSkill = 6; iTemp = 1; } break;
						case SDLK_7:
						case SDLK_KP7:
							if ((iEditPoP == 1) && (iGuardSkill != 7))
								{ iGuardSkill = 7; iTemp = 1; } break;
						case SDLK_8:
						case SDLK_KP8:
							if ((iEditPoP == 1) && (iGuardSkill != 8))
								{ iGuardSkill = 8; iTemp = 1; } break;
						case SDLK_9:
						case SDLK_KP9:
							if ((iEditPoP == 1) && (iGuardSkill != 9))
								{ iGuardSkill = 9; iTemp = 1; } break;
						case SDLK_a:
							if ((iEditPoP == 1) && (iGuardSkill != 10))
								{ iGuardSkill = 10; iTemp = 1; }
							if (cCurType == 'c')
							{
								iChangeWall++;
								if (iChangeWall == 3) { iChangeWall = 1; }
								PlaySound ("wav/check_box.wav");
								ApplyWallIfNecessary (iLocation);
							}
							if (cCurType == 't')
							{
								iChangeWall++;
								if (iChangeWall == 4) { iChangeWall = 1; }
								PlaySound ("wav/check_box.wav");
								ApplyWallIfNecessary (iLocation);
							}
							break;
						case SDLK_b:
							if ((iEditPoP == 1) && (iGuardSkill != 11))
								{ iGuardSkill = 11; iTemp = 1; }
							if ((iEditPoP == 2) && ((cCurType == 'r') || (cCurType == 't')))
							{
								PlaySound ("wav/screen2or3.wav");
								iChanging = ChangeBackground (iLocation, screen);
							}
							if ((iEditPoP == 2) && (cCurType == 'c'))
							{
								iChangeBot++;
								if (iChangeBot == 8) { iChangeBot = 0; }
								PlaySound ("wav/check_box.wav");
								UpdateMod2IfNecessary (iLocation);
							}
							break;
						case SDLK_f:
							if (iEditPoP == 2)
							{
								iChangeFrame++;
								if (iChangeFrame == 16) { iChangeFrame = 0; }
								PlaySound ("wav/plus_minus.wav");
								ApplyJumpsFrameIfNecessary (iLocation);
							}
							break;
						case SDLK_g:
							if (iEditPoP == 2)
							{
								PlaySound ("wav/screen2or3.wav");
								ChangeGuards (iLocation, screen);
							}
							break;
						case SDLK_j: /*** jumps ***/
							if (iEditPoP == 2)
							{
								iChangeJumps++;
								if (iChangeJumps == 5) { iChangeJumps = 1; }
								PlaySound ("wav/check_box.wav");
								ApplyJumpsFrameIfNecessary (iLocation);
							}
							break;
						case SDLK_m:
							if (iEditPoP == 2)
							{
								PlaySound ("wav/screen2or3.wav");
								ChangeMusic (iLocation, screen);
							}
							break;
						case SDLK_p:
							if (iEditPoP == 2)
							{
								iChangePercent++;
								if (iChangePercent == 101) { iChangePercent = 0; }
								PlaySound ("wav/plus_minus.wav");
								ApplyPercentIfNecessary (iLocation);
							}
							break;
						case SDLK_r:
							if (cCurType == 'r')
							{
								iChangeWall++;
								if (iChangeWall == 5) { iChangeWall = 1; }
								PlaySound ("wav/check_box.wav");
								ApplyWallIfNecessary (iLocation);
							}
							break;
						case SDLK_s:
							if (cCurType == 'c')
							{
								iChangeStal++;
								if (iChangeStal == 8) { iChangeStal = 0; }
								PlaySound ("wav/check_box.wav");
								UpdateMod2IfNecessary (iLocation);
							}
							if (cCurType == 'r')
							{
								iChangeSym++;
								if (iChangeSym == 4) { iChangeSym = 0; }
								PlaySound ("wav/check_box.wav");
								UpdateMod2IfNecessary (iLocation);
							}
							break;
						case SDLK_LEFT:
							if (event.key.keysym.mod & KMOD_LCTRL)
							{
								ChangeEvent (-10, 1);
							} else if (event.key.keysym.mod & KMOD_RCTRL) {
								if (iEditPoP == 2)
								{
									if (iChangePercent > 0)
									{
										iChangePercent-=10;
										if (iChangePercent < 0) { iChangePercent = 0; }
										PlaySound ("wav/plus_minus.wav");
										ApplyPercentIfNecessary (iLocation);
									}
								} else {
									ChangeEvent (-10, 1);
								}
							} else if (event.key.keysym.mod & KMOD_LSHIFT) {
								ChangeEvent (-1, 1);
							} else if (event.key.keysym.mod & KMOD_RSHIFT) {
								if (iEditPoP == 2)
								{
									if (iChangePercent > 0)
									{
										iChangePercent--;
										PlaySound ("wav/plus_minus.wav");
										ApplyPercentIfNecessary (iLocation);
									}
								} else {
									ChangeEvent (-1, 1);
								}
							} else {
								do {
									iOnTile--;
									if ((cCurType == 'd') || (cCurType == 'p'))
									{
										switch (iOnTile)
										{
											case 0: iOnTile = 13; break;
											case 13: iOnTile = 26; break;
											case 26: iOnTile = 39; break;
											case 39: iOnTile = 52; break;
											case 52: iOnTile = 65; break;
											case 65: iOnTile = 78; break;
											case 91: iOnTile = 109; break;
										}
									}
									if (cCurType == 'o')
									{
										switch (iOnTile)
										{
											case 16: iOnTile = 26; break;
											case 22: iOnTile = 19; break;
											case 28: iOnTile = 39; break;
											case 35: iOnTile = 32; break;
											case 39: iOnTile = 52; break;
											case 52: iOnTile = 62; break;
											case 65: iOnTile = 75; break;
											case 78: iOnTile = 88; break;
											case 105: iOnTile = 83; break;
											case 86: iOnTile = 107; break;
										}
									}
									if (cCurType == 'c')
									{
										switch (iOnTile)
										{
											case 0: iOnTile = 13; break;
											case 13: iOnTile = 26; break;
											case 26: iOnTile = 39; break;
											case 39: iOnTile = 52; break;
											case 52: iOnTile = 58; break;
											case 105: iOnTile = 107; break;
										}
									}
									if (cCurType == 'r')
									{
										switch (iOnTile)
										{
											case 0: iOnTile = 13; break;
											case 13: iOnTile = 26; break;
											case 26: iOnTile = 39; break;
											case 39: iOnTile = 52; break;
											case 52: iOnTile = 55; break;
											case 105: iOnTile = 107; break;
										}
									}
									if (cCurType == 't')
									{
										switch (iOnTile)
										{
											case 0: iOnTile = 13; break;
											case 13: iOnTile = 26; break;
											case 31: iOnTile = 39; break;
											case 44: iOnTile = 52; break;
											case 57: iOnTile = 58; break;
											case 105: iOnTile = 107; break;
										}
									}
								} while (IsDisabled (iOnTile) == 1);
							}
							break;
						case SDLK_RIGHT:
							if (event.key.keysym.mod & KMOD_LCTRL)
							{
								ChangeEvent (10, 1);
							} else if (event.key.keysym.mod & KMOD_RCTRL) {
								if (iEditPoP == 2)
								{
									if (iChangePercent < 100)
									{
										iChangePercent+=10;
										if (iChangePercent > 100) { iChangePercent = 100; }
										PlaySound ("wav/plus_minus.wav");
										ApplyPercentIfNecessary (iLocation);
									}
								} else {
									ChangeEvent (10, 1);
								}
							} else if (event.key.keysym.mod & KMOD_LSHIFT) {
								ChangeEvent (1, 1);
							} else if (event.key.keysym.mod & KMOD_RSHIFT) {
								if (iEditPoP == 2)
								{
									if (iChangePercent < 100)
									{
										iChangePercent++;
										PlaySound ("wav/plus_minus.wav");
										ApplyPercentIfNecessary (iLocation);
									}
								} else {
									ChangeEvent (1, 1);
								}
							} else {
								do {
									iOnTile++;
									if ((cCurType == 'd') || (cCurType == 'p'))
									{
										switch (iOnTile)
										{
											case 14: iOnTile = 1; break;
											case 27: iOnTile = 14; break;
											case 40: iOnTile = 27; break;
											case 53: iOnTile = 40; break;
											case 66: iOnTile = 53; break;
											case 79: iOnTile = 66; break;
											case 110: iOnTile = 92; break;
										}
									}
									if (cCurType == 'o')
									{
										switch (iOnTile)
										{
											case 20: iOnTile = 23; break;
											case 27: iOnTile = 17; break;
											case 33: iOnTile = 36; break;
											case 40: iOnTile = 29; break;
											case 53: iOnTile = 40; break;
											case 63: iOnTile = 53; break;
											case 76: iOnTile = 66; break;
											case 84: iOnTile = 106; break;
											case 108: iOnTile = 87; break;
											case 89: iOnTile = 79; break;
										}
									}
									if (cCurType == 'c')
									{
										switch (iOnTile)
										{
											case 14: iOnTile = 1; break;
											case 27: iOnTile = 14; break;
											case 40: iOnTile = 27; break;
											case 53: iOnTile = 40; break;
											case 59: iOnTile = 53; break;
											case 108: iOnTile = 106; break;
										}
									}
									if (cCurType == 'r')
									{
										switch (iOnTile)
										{
											case 14: iOnTile = 1; break;
											case 27: iOnTile = 14; break;
											case 40: iOnTile = 27; break;
											case 53: iOnTile = 40; break;
											case 56: iOnTile = 53; break;
											case 108: iOnTile = 106; break;
										}
									}
									if (cCurType == 't')
									{
										switch (iOnTile)
										{
											case 14: iOnTile = 1; break;
											case 27: iOnTile = 14; break;
											case 40: iOnTile = 32; break;
											case 53: iOnTile = 45; break;
											case 59: iOnTile = 58; break;
											case 108: iOnTile = 106; break;
										}
									}
								} while (IsDisabled (iOnTile) == 1);
							}
							break;
						case SDLK_UP:
							if ((cCurType == 'd') || (cCurType == 'p'))
							{
								switch (iOnTile)
								{
									case 1: iOnTile = 92; break;
									case 2: iOnTile = 94; break;
									case 3: iOnTile = 96; break;
									case 4: iOnTile = 98; break;
									case 5: iOnTile = 100; break;
									case 6: iOnTile = 102; break;
									case 7: iOnTile = 104; break;
									case 8: iOnTile = 106; break;
									case 9: iOnTile = 108; break;
									case 10: iOnTile = 109; break;
									case 11: iOnTile = 109; break;
									case 12: iOnTile = 109; break;
									case 13: iOnTile = 109; break;
									case 92: iOnTile = 66; break;
									case 93: iOnTile = 66; break;
									case 94: iOnTile = 67; break;
									case 95: iOnTile = 67; break;
									case 96: iOnTile = 68; break;
									case 97: iOnTile = 68; break;
									case 98: iOnTile = 69; break;
									case 99: iOnTile = 69; break;
									case 100: iOnTile = 70; break;
									case 101: iOnTile = 70; break;
									case 102: iOnTile = 71; break;
									case 103: iOnTile = 71; break;
									case 104: iOnTile = 72; break;
									case 105: iOnTile = 72; break;
									case 106: iOnTile = 73; break;
									case 107: iOnTile = 73; break;
									case 108: iOnTile = 74; break;
									case 109: iOnTile = 74; break;
									default: iOnTile-=13; break;
								}
							}
							if (cCurType == 'o')
							{
								switch (iOnTile)
								{
									case 40: iOnTile = 79; break;
									case 41: iOnTile = 80; break;
									case 29: iOnTile = 81; break;
									case 17: iOnTile = 82; break;
									case 18: iOnTile = 83; break;
									case 19: iOnTile = 71; break;
									case 46: iOnTile = 72; break;
									case 47: iOnTile = 106; break;
									case 48: iOnTile = 87; break;
									case 23: iOnTile = 88; break;
									case 24: iOnTile = 50; break;
									case 25: iOnTile = 51; break;
									case 26: iOnTile = 52; break;
									case 106: iOnTile = 73; break;
									case 107: iOnTile = 73; break;
									default: iOnTile-=13; break;
								}
							}
							if (cCurType == 'c')
							{
								switch (iOnTile)
								{
									case 1: iOnTile = 53; break;
									case 2: iOnTile = 54; break;
									case 3: iOnTile = 55; break;
									case 4: iOnTile = 56; break;
									case 5: iOnTile = 57; break;
									case 6: iOnTile = 58; break;
									case 7: iOnTile = 46; break;
									case 8: iOnTile = 106; break;
									case 9: iOnTile = 48; break;
									case 10: iOnTile = 49; break;
									case 11: iOnTile = 50; break;
									case 12: iOnTile = 51; break;
									case 13: iOnTile = 52; break;
									case 106: iOnTile = 47; break;
									case 107: iOnTile = 47; break;
									default: iOnTile-=13; break;
								}
							}
							if (cCurType == 'r')
							{
								switch (iOnTile)
								{
									case 1: iOnTile = 53; break;
									case 2: iOnTile = 54; break;
									case 3: iOnTile = 55; break;
									case 4: iOnTile = 43; break;
									case 5: iOnTile = 44; break;
									case 6: iOnTile = 45; break;
									case 7: iOnTile = 46; break;
									case 8: iOnTile = 106; break;
									case 9: iOnTile = 48; break;
									case 10: iOnTile = 49; break;
									case 11: iOnTile = 50; break;
									case 12: iOnTile = 51; break;
									case 13: iOnTile = 52; break;
									case 106: iOnTile = 47; break;
									case 107: iOnTile = 47; break;
									default: iOnTile-=13; break;
								}
							}
							if (cCurType == 't')
							{
								switch (iOnTile)
								{
									case 1: iOnTile = 14; break;
									case 2: iOnTile = 15; break;
									case 3: iOnTile = 16; break;
									case 4: iOnTile = 17; break;
									case 5: iOnTile = 18; break;
									case 6: iOnTile = 58; break;
									case 7: iOnTile = 46; break;
									case 8: iOnTile = 106; break;
									case 9: iOnTile = 48; break;
									case 10: iOnTile = 49; break;
									case 11: iOnTile = 50; break;
									case 12: iOnTile = 51; break;
									case 13: iOnTile = 52; break;
									case 106: iOnTile = 47; break;
									case 107: iOnTile = 47; break;
									default: iOnTile-=13; break;
								}
							}
							while (IsDisabled (iOnTile) == 1) { iOnTile--; }
							break;
						case SDLK_DOWN:
							if ((cCurType == 'd') || (cCurType == 'p'))
							{
								switch (iOnTile)
								{
									case 66: iOnTile = 92; break;
									case 67: iOnTile = 94; break;
									case 68: iOnTile = 96; break;
									case 69: iOnTile = 98; break;
									case 70: iOnTile = 100; break;
									case 71: iOnTile = 102; break;
									case 72: iOnTile = 104; break;
									case 73: iOnTile = 106; break;
									case 74: iOnTile = 108; break;
									case 75: iOnTile = 109; break;
									case 76: iOnTile = 109; break;
									case 77: iOnTile = 109; break;
									case 78: iOnTile = 109; break;
									case 92: iOnTile = 1; break;
									case 93: iOnTile = 1; break;
									case 94: iOnTile = 2; break;
									case 95: iOnTile = 2; break;
									case 96: iOnTile = 3; break;
									case 97: iOnTile = 3; break;
									case 98: iOnTile = 4; break;
									case 99: iOnTile = 4; break;
									case 100: iOnTile = 5; break;
									case 101: iOnTile = 5; break;
									case 102: iOnTile = 6; break;
									case 103: iOnTile = 6; break;
									case 104: iOnTile = 7; break;
									case 105: iOnTile = 7; break;
									case 106: iOnTile = 8; break;
									case 107: iOnTile = 8; break;
									case 108: iOnTile = 9; break;
									case 109: iOnTile = 9; break;
									default: iOnTile+=13; break;
								}
							}
							if (cCurType == 'o')
							{
								switch (iOnTile)
								{
									case 79: iOnTile = 40; break;
									case 80: iOnTile = 41; break;
									case 81: iOnTile = 29; break;
									case 82: iOnTile = 17; break;
									case 83: iOnTile = 18; break;
									case 71: iOnTile = 19; break;
									case 72: iOnTile = 46; break;
									case 73: iOnTile = 106; break;
									case 87: iOnTile = 48; break;
									case 88: iOnTile = 23; break;
									case 50: iOnTile = 24; break;
									case 51: iOnTile = 25; break;
									case 52: iOnTile = 26; break;
									case 106: iOnTile = 47; break;
									case 107: iOnTile = 47; break;
									default: iOnTile+=13; break;
								}
							}
							if (cCurType == 'c')
							{
								switch (iOnTile)
								{
									case 53: iOnTile = 1; break;
									case 54: iOnTile = 2; break;
									case 55: iOnTile = 3; break;
									case 56: iOnTile = 4; break;
									case 57: iOnTile = 5; break;
									case 58: iOnTile = 6; break;
									case 46: iOnTile = 7; break;
									case 47: iOnTile = 106; break;
									case 48: iOnTile = 9; break;
									case 49: iOnTile = 10; break;
									case 50: iOnTile = 11; break;
									case 51: iOnTile = 12; break;
									case 52: iOnTile = 13; break;
									case 106: iOnTile = 8; break;
									case 107: iOnTile = 8; break;
									default: iOnTile+=13; break;
								}
							}
							if (cCurType == 'r')
							{
								switch (iOnTile)
								{
									case 53: iOnTile = 1; break;
									case 54: iOnTile = 2; break;
									case 55: iOnTile = 3; break;
									case 43: iOnTile = 4; break;
									case 44: iOnTile = 5; break;
									case 45: iOnTile = 6; break;
									case 46: iOnTile = 7; break;
									case 47: iOnTile = 106; break;
									case 48: iOnTile = 9; break;
									case 49: iOnTile = 10; break;
									case 50: iOnTile = 11; break;
									case 51: iOnTile = 12; break;
									case 52: iOnTile = 13; break;
									case 106: iOnTile = 8; break;
									case 107: iOnTile = 8; break;
									default: iOnTile+=13; break;
								}
							}
							if (cCurType == 't')
							{
								switch (iOnTile)
								{
									case 14: iOnTile = 1; break;
									case 15: iOnTile = 2; break;
									case 16: iOnTile = 3; break;
									case 17: iOnTile = 4; break;
									case 18: iOnTile = 5; break;
									case 58: iOnTile = 6; break;
									case 46: iOnTile = 7; break;
									case 47: iOnTile = 106; break;
									case 48: iOnTile = 9; break;
									case 49: iOnTile = 10; break;
									case 50: iOnTile = 11; break;
									case 51: iOnTile = 12; break;
									case 52: iOnTile = 13; break;
									case 106: iOnTile = 8; break;
									case 107: iOnTile = 8; break;
									default: iOnTile+=13; break;
								}
							}
							while (IsDisabled (iOnTile) == 1) { iOnTile--; }
							break;
						case SDLK_o:
							iChanging = ChangePosCustom (iLocation, screen); break;
						default: break;
					}
					if (iTemp == 1)
					{
						if (sGuardLocations[iCurRoom - 1] + 1 == iSelected)
						{
							sGuardSkills[iCurRoom - 1] = iGuardSkill;
							iChanged++;
						}
						PlaySound ("wav/check_box.wav");
					}
					ShowChange (iLocation);
					break;
				case SDL_MOUSEMOTION:
					iOldXPos = iXPos;
					iOldYPos = iYPos;
					SDL_GetMouseState (&iXPos, &iYPos);
					if ((iOldXPos == iXPos) && (iOldYPos == iYPos)) { break; }

					iNowOn = OnTile (event);
					if ((iOnTile != iNowOn) && (iNowOn != 0))
					{
						if (IsDisabled (iNowOn) == 0)
						{
							iOnTile = iNowOn;
							ShowChange (iLocation);
						}
					}
					break;
				case SDL_MOUSEBUTTONDOWN: /*** ChangePos ***/
					if (event.button.button == 1) /*** left mouse button ***/
					{
						if (InArea (event, 656, 0, 692, 387) == 1) /*** close ***/
						{
							if (iCloseOn != 1) { iCloseOn = 1; }
							ShowChange (iLocation);
						}
						if (InArea (event, 254, 324, 402, 386) == 1) /*** background ***/
						{
							if (iBackgroundOn != 1) { iBackgroundOn = 1; }
							ShowChange (iLocation);
						}
						if (InArea (event, 254, 388, 352, 450) == 1) /*** guards ***/
						{
							if (iGuardsOn != 1) { iGuardsOn = 1; }
							ShowChange (iLocation);
						}
						if (InArea (event, 656, 387, 692, 455) == 1) /*** custom ***/
						{
							if (iCustomOn != 1) { iCustomOn = 1; }
							ShowChange (iLocation);
						}
						if (InArea (event, 637, 305, 651, 329) == 1) /*** music ***/
						{
							if (iMusicOn != 1) { iMusicOn = 1; }
							ShowChange (iLocation);
						}
					}
					break;
				case SDL_MOUSEBUTTONUP:
					iCloseOn = 0;
					iBackgroundOn = 0;
					iGuardsOn = 0;
					iCustomOn = 0;
					iMusicOn = 0;

					/*** On tile or human. ***/
					iUseTile = 0;
					if (((cCurType == 'd') || (cCurType == 'p')) &&
						((InArea (event, 2, 2, 652, 386) == 1) ||
						(InArea (event, 2, 386, 454, 452) == 1))) { iUseTile = 1; }
					if ((cCurType == 'o') &&
						((InArea (event, 154, 48, 302, 110) == 1) ||
						(InArea (event, 454, 48, 652, 110) == 1) ||
						(InArea (event, 104, 122, 302, 184) == 1) ||
						(InArea (event, 454, 122, 652, 184) == 1) ||
						(InArea (event, 4, 196, 652, 258) == 1) ||
						(InArea (event, 4, 260, 502, 386) == 1) ||
						(InArea (event, 4, 388, 252, 450) == 1) ||
						(InArea (event, 354, 388, 502, 450) == 1))) { iUseTile = 1; }
					if ((cCurType == 'c') &&
						((InArea (event, 4, 4, 652, 258) == 1) ||
						(InArea (event, 4, 260, 302, 322) == 1) ||
						(InArea (event, 354, 388, 402, 450) == 1))) { iUseTile = 1; }
					if ((cCurType == 'r') &&
						((InArea (event, 4, 4, 652, 258) == 1) ||
						(InArea (event, 4, 260, 152, 322) == 1) ||
						(InArea (event, 354, 388, 402, 450) == 1))) { iUseTile = 1; }
					if ((cCurType == 't') &&
						((InArea (event, 4, 4, 652, 130) == 1) ||
						(InArea (event, 254, 132, 652, 258) == 1) ||
						(InArea (event, 254, 260, 302, 322) == 1) ||
						(InArea (event, 354, 388, 402, 450) == 1))) { iUseTile = 1; }

					if (event.button.button == 1) /*** left mouse button, tile ***/
					{
						if (iEditPoP == 1)
						{
							iTemp = 0;
							if (InArea (event, 455, 420, 468, 433) == 1)
								if (iGuardSkill != 0) { iGuardSkill = 0; iTemp = 1; }
							if (InArea (event, 470, 420, 483, 433) == 1)
								if (iGuardSkill != 1) { iGuardSkill = 1; iTemp = 1; }
							if (InArea (event, 485, 420, 498, 433) == 1)
								if (iGuardSkill != 2) { iGuardSkill = 2; iTemp = 1; }
							if (InArea (event, 500, 420, 513, 433) == 1)
								if (iGuardSkill != 3) { iGuardSkill = 3; iTemp = 1; }
							if (InArea (event, 515, 420, 528, 433) == 1)
								if (iGuardSkill != 4) { iGuardSkill = 4; iTemp = 1; }
							if (InArea (event, 455, 435, 468, 448) == 1)
								if (iGuardSkill != 5) { iGuardSkill = 5; iTemp = 1; }
							if (InArea (event, 470, 435, 483, 448) == 1)
								if (iGuardSkill != 6) { iGuardSkill = 6; iTemp = 1; }
							if (InArea (event, 485, 435, 498, 448) == 1)
								if (iGuardSkill != 7) { iGuardSkill = 7; iTemp = 1; }
							if (InArea (event, 500, 435, 513, 448) == 1)
								if (iGuardSkill != 8) { iGuardSkill = 8; iTemp = 1; }
							if (InArea (event, 515, 435, 528, 448) == 1)
								if (iGuardSkill != 9) { iGuardSkill = 9; iTemp = 1; }
							if (InArea (event, 500, 405, 513, 418) == 1)
								if (iGuardSkill != 10) { iGuardSkill = 10; iTemp = 1; }
							if (InArea (event, 515, 405, 528, 418) == 1)
								if (iGuardSkill != 11) { iGuardSkill = 11; iTemp = 1; }
							if (iTemp == 1)
							{
								if (sGuardLocations[iCurRoom - 1] + 1 == iSelected)
								{
									sGuardSkills[iCurRoom - 1] = iGuardSkill;
									iChanged++;
								}
								PlaySound ("wav/check_box.wav");
							}
						}

						/*** Changing the event number. ***/
						if (iEditPoP == 1)
						{
							if (InArea (event, 534, 424, 546, 444) == 1)
								{ ChangeEvent (-10, 1); }
							if (InArea (event, 549, 424, 561, 444) == 1)
								{ ChangeEvent (-1, 1); }
							if (InArea (event, 619, 424, 631, 444) == 1)
								{ ChangeEvent (1, 1); }
							if (InArea (event, 634, 424, 646, 444) == 1)
								{ ChangeEvent (10, 1); }
						} else {
							if (InArea (event, 521, 424, 534, 444) == 1)
								{ ChangeEvent (-10, 1); }
							if (InArea (event, 536, 424, 549, 444) == 1)
								{ ChangeEvent (-1, 1); }
							if (InArea (event, 606, 424, 619, 444) == 1)
								{ ChangeEvent (1, 1); }
							if (InArea (event, 621, 424, 634, 444) == 1)
								{ ChangeEvent (10, 1); }
						}

						/*** On close: exit ChangePos. ***/
						if (InArea (event, 656, 0, 692, 387) == 1) { iChanging = 0; }

						/*** On custom, mouse button up. ***/
						if (InArea (event, 656, 387, 692, 455) == 1)
						{
							PlaySound ("wav/screen2or3.wav");
							iChanging = ChangePosCustom (iLocation, screen);
						}

						if (iEditPoP == 2)
						{
							if (InArea (event, 254, 388, 352, 450) == 1)
							{
								PlaySound ("wav/screen2or3.wav");
								ChangeGuards (iLocation, screen);
							}
							if (InArea (event, 637, 305, 651, 327) == 1)
							{
								PlaySound ("wav/screen2or3.wav");
								ChangeMusic (iLocation, screen);
							}

							/*** jumps ***/
							if ((InArea (event, 588, 265, 602, 279) == 1) &&
								(iChangeJumps != 1))
							{
								iChangeJumps = 1;
								PlaySound ("wav/check_box.wav");
								ApplyJumpsFrameIfNecessary (iLocation);
							}
							if ((InArea (event, 603, 265, 617, 279) == 1) &&
								(iChangeJumps != 2))
							{
								iChangeJumps = 2;
								PlaySound ("wav/check_box.wav");
								ApplyJumpsFrameIfNecessary (iLocation);
							}
							if ((InArea (event, 618, 265, 632, 279) == 1) &&
								(iChangeJumps != 3))
							{
								iChangeJumps = 3;
								PlaySound ("wav/check_box.wav");
								ApplyJumpsFrameIfNecessary (iLocation);
							}
							if ((InArea (event, 633, 265, 647, 279) == 1) &&
								(iChangeJumps != 4))
							{
								iChangeJumps = 4;
								PlaySound ("wav/check_box.wav");
								ApplyJumpsFrameIfNecessary (iLocation);
							}

							/*** alt wall ***/
							iWallChanged = 0;
							if (cCurType == 'c')
							{
								if ((InArea (event, 618, 287, 632, 301) == 1) &&
									(iChangeWall != 1)) { iChangeWall = 1; iWallChanged = 1; }
								if ((InArea (event, 633, 287, 647, 301) == 1) &&
									(iChangeWall != 2)) { iChangeWall = 2; iWallChanged = 1; }
							}
							if (cCurType == 'r')
							{
								if ((InArea (event, 588, 287, 602, 301) == 1) &&
									(iChangeWall != 1)) { iChangeWall = 1; iWallChanged = 1; }
								if ((InArea (event, 603, 287, 617, 301) == 1) &&
									(iChangeWall != 2)) { iChangeWall = 2; iWallChanged = 1; }
								if ((InArea (event, 618, 287, 632, 301) == 1) &&
									(iChangeWall != 3)) { iChangeWall = 3; iWallChanged = 1; }
								if ((InArea (event, 633, 287, 647, 301) == 1) &&
									(iChangeWall != 4)) { iChangeWall = 4; iWallChanged = 1; }
							}
							if (cCurType == 't')
							{
								if ((InArea (event, 603, 287, 617, 301) == 1) &&
									(iChangeWall != 1)) { iChangeWall = 1; iWallChanged = 1; }
								if ((InArea (event, 618, 287, 632, 301) == 1) &&
									(iChangeWall != 2)) { iChangeWall = 2; iWallChanged = 1; }
								if ((InArea (event, 633, 287, 647, 301) == 1) &&
									(iChangeWall != 3)) { iChangeWall = 3; iWallChanged = 1; }
							}
							if (iWallChanged == 1)
							{
								PlaySound ("wav/check_box.wav");
								ApplyWallIfNecessary (iLocation);
							}

							/*** frame ***/
							if (PlusMinus (event, &iChangeFrame, 574, 307, 0, 15, -1, 0))
								{ ApplyJumpsFrameIfNecessary (iLocation); }
							if (PlusMinus (event, &iChangeFrame, 621, 307, 0, 15, 1, 0))
								{ ApplyJumpsFrameIfNecessary (iLocation); }

							/*** percent ***/
							if (PlusMinus (event, &iChangePercent, 521, 364, 0, 100, -10, 0))
								{ ApplyPercentIfNecessary (iLocation); }
							if (PlusMinus (event, &iChangePercent, 536, 364, 0, 100, -1, 0))
								{ ApplyPercentIfNecessary (iLocation); }
							if (PlusMinus (event, &iChangePercent, 606, 364, 0, 100, 1, 0))
								{ ApplyPercentIfNecessary (iLocation); }
							if (PlusMinus (event, &iChangePercent, 621, 364, 0, 100, 10, 0))
								{ ApplyPercentIfNecessary (iLocation); }
						}

						if ((cCurType == 'r') || (cCurType == 't'))
						{
							if (InArea (event, 254, 324, 402, 386) == 1)
							{
								PlaySound ("wav/screen2or3.wav");
								iChanging = ChangeBackground (iLocation, screen);
							}
						}

						if (cCurType == 'c')
						{
							/*** stal ***/
							iMod2Ch = iChangeStal;
							if (InArea (event, 231, 393, 245, 407) == 1) { iChangeStal = 0; }
							if (InArea (event, 11, 347, 25, 361) == 1) { iChangeStal = 1; }
							if (InArea (event, 67, 347, 81, 361) == 1) { iChangeStal = 2; }
							if (InArea (event, 117, 347, 131, 361) == 1) { iChangeStal = 3; }
							if (InArea (event, 185, 347, 199, 361) == 1) { iChangeStal = 4; }
							if (InArea (event, 11, 393, 25, 407) == 1) { iChangeStal = 5; }
							if (InArea (event, 95, 393, 109, 407) == 1) { iChangeStal = 6; }
							if (InArea (event, 145, 393, 159, 407) == 1) { iChangeStal = 7; }
							if (iMod2Ch != iChangeStal)
							{
								PlaySound ("wav/check_box.wav");
								UpdateMod2IfNecessary (iLocation);
							}

							/*** bot ***/
							iMod2Ch = iChangeBot;
							if (InArea (event, 411, 283, 425, 297) == 1) { iChangeBot = 0; }
							if (InArea (event, 411, 304, 425, 318) == 1) { iChangeBot = 1; }
							if (InArea (event, 411, 325, 425, 339) == 1) { iChangeBot = 2; }
							if (InArea (event, 411, 346, 425, 360) == 1) { iChangeBot = 3; }
							if (InArea (event, 411, 367, 425, 381) == 1) { iChangeBot = 4; }
							if (InArea (event, 411, 388, 425, 402) == 1) { iChangeBot = 5; }
							if (InArea (event, 411, 409, 425, 423) == 1) { iChangeBot = 6; }
							if (InArea (event, 411, 430, 425, 444) == 1) { iChangeBot = 7; }
							if (iMod2Ch != iChangeBot)
							{
								PlaySound ("wav/check_box.wav");
								UpdateMod2IfNecessary (iLocation);
							}
						}

						if (cCurType == 'r')
						{
							/*** sym ***/
							iMod2Ch = iChangeSym;
							if (InArea (event, 411, 429, 425, 443) == 1) { iChangeSym = 0; }
							if (InArea (event, 411, 304, 425, 318) == 1) { iChangeSym = 1; }
							if (InArea (event, 411, 362, 425, 376) == 1) { iChangeSym = 2; }
							if (InArea (event, 411, 404, 425, 418) == 1) { iChangeSym = 3; }
							if (iMod2Ch != iChangeSym)
							{
								PlaySound ("wav/check_box.wav");
								UpdateMod2IfNecessary (iLocation);
							}

							/*** crk ***/
							iMod2Ch = iChangeCrk;
							if (InArea (event, 231, 415, 245, 429) == 1) { iChangeCrk = 0; }
							if (InArea (event, 11, 352, 25, 366) == 1) { iChangeCrk = 1; }
							if (InArea (event, 115, 331, 129, 345) == 1) { iChangeCrk = 3; }
							if (InArea (event, 161, 331, 175, 345) == 1) { iChangeCrk = 4; }
							if (InArea (event, 202, 331, 216, 345) == 1) { iChangeCrk = 5; }
							if (InArea (event, 11, 380, 25, 394) == 1) { iChangeCrk = 6; }
							if (InArea (event, 85, 380, 99, 394) == 1) { iChangeCrk = 7; }
							if (InArea (event, 85, 415, 99, 429) == 1) { iChangeCrk = 8; }
							if (InArea (event, 11, 415, 25, 429) == 1) { iChangeCrk = 9; }
							if (InArea (event, 143, 415, 157, 429) == 1) { iChangeCrk = 10; }
							if (InArea (event, 143, 380, 157, 394) == 1) { iChangeCrk = 11; }
							if (InArea (event, 231, 380, 245, 394) == 1) { iChangeCrk = 12; }
							if (iMod2Ch != iChangeCrk)
							{
								PlaySound ("wav/check_box.wav");
								UpdateMod2IfNecessary (iLocation);
							}
						}

						if (iUseTile == 1)
						{
							UseTile (iOnTile, iLocation, iCurRoom);
							if (((iEditPoP == 1) && (iOnTile <= 72)) ||
								((iEditPoP == 2) && (iOnTile <= 91)))
							{
								iChanging = 0;
							}
							iChanged++;
						}
					}

					if (event.button.button == 2)
					{
						if ((iUseTile == 1) && (iOnTile >= 1) && (iOnTile <= 91))
						{
							for (iTemp = 1; iTemp <= 30; iTemp++)
							{
								UseTile (iOnTile, iTemp, iCurRoom);
							}
							iChanging = 0;
							iChanged++;
						}
					}

					if (event.button.button == 3)
					{
						if ((iUseTile == 1) && (iOnTile >= 1) && (iOnTile <= 91))
						{
							for (iTemp2 = 1; iTemp2 <= iRooms; iTemp2++)
							{
								for (iTemp = 1; iTemp <= 30; iTemp++)
								{
									UseTile (iOnTile, iTemp, iTemp2);
								}
							}
							iChanging = 0;
							iChanged++;
						}
					}

					ShowChange (iLocation);
					break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/ok_close.wav");
}
/*****************************************************************************/
int ChangePosCustom (int iLocation, SDL_Surface *screen)
/*****************************************************************************/
{
	SDL_Event event;
	int iChanging;
	int iChangingCustom;
	int iTemp;
	int iArNr;

	iChanging = 1;
	iChangingCustom = 1;

	/*** Set iChangeForeRand, iChangeForeMod and iChangeFore. ***/
	iTemp = iThingA[iCurRoom][iLocation - 1];
	iChangeForeRand = 0;
	if (iTemp >= 192) { iTemp-=192; iChangeForeRand = 3; }
	if (iTemp >= 128) { iTemp-=128; iChangeForeRand = 2; }
	if (iTemp >= 64) { iTemp-=64; iChangeForeRand = 1; }
	if (iTemp >= 32) { iTemp-=32; iChangeForeMod = 1; }
		else { iChangeForeMod = 0; }
	iChangeFore = iTemp;

	/*** Set iChangeMod. ***/
	for (iArNr = 1; iArNr <= 4; iArNr++)
	{
		iTemp = iModifierA[iCurRoom][iLocation - 1][iArNr];
		if (iTemp == -1) { iTemp = 255; }
		iChangeMod[iArNr] = iTemp;
	}

	ShowChangeCustom (iLocation);
	while (iChangingCustom == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
							iTemp = 0;
							switch (iChangeForeRand)
							{
								case 0: break;
								case 1: iTemp+=64; break;
								case 2: iTemp+=128; break;
								case 3: iTemp+=192; break;
							}
							if (iChangeForeMod == 1) { iTemp+=32; }
							iTemp+=iChangeFore;
							SetLocation (iCurRoom, iLocation, iTemp, iChangeMod[1],
								iChangeMod[2], iChangeMod[3], iChangeMod[4]);
							iChanged++;
							iChangingCustom = 0;
							iChanging = 0;
							break;
						case SDLK_n:
							if (iChangeForeMod != 0)
							{
								iChangeForeMod = 0;
								PlaySound ("wav/check_box.wav");
							}
							break;
						case SDLK_y:
							if (iChangeForeMod != 1)
							{
								iChangeForeMod = 1;
								PlaySound ("wav/check_box.wav");
							}
							break;
						case SDLK_ESCAPE:
						case SDLK_q:
						case SDLK_c:
							iChangingCustom = 0; break;
						case SDLK_0:
						case SDLK_KP0:
							if (iChangeForeRand != 0)
								{ iChangeForeRand = 0; PlaySound ("wav/check_box.wav"); }
							break;
						case SDLK_1:
						case SDLK_KP1:
							if (iChangeForeRand != 1)
								{ iChangeForeRand = 1; PlaySound ("wav/check_box.wav"); }
							break;
						case SDLK_2:
						case SDLK_KP2:
							if (iChangeForeRand != 2)
								{ iChangeForeRand = 2; PlaySound ("wav/check_box.wav"); }
							break;
						case SDLK_3:
						case SDLK_KP3:
							if (iChangeForeRand != 3)
								{ iChangeForeRand = 3; PlaySound ("wav/check_box.wav"); }
							break;
						case SDLK_LEFT:
							if ((event.key.keysym.mod & KMOD_LCTRL) ||
								(event.key.keysym.mod & KMOD_RCTRL))
							{
								ChangeCustom (-10, 2, 1);
							} else if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								ChangeCustom (-1, 2, 1);
							} else {
								ChangeCustom (-1, 1, 1);
							}
							break;
						case SDLK_RIGHT:
							if ((event.key.keysym.mod & KMOD_LCTRL) ||
								(event.key.keysym.mod & KMOD_RCTRL))
							{
								ChangeCustom (10, 2, 1);
							} else if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								ChangeCustom (1, 2, 1);
							} else {
								ChangeCustom (1, 1, 1);
							}
							break;
						default: break;
					}
					ShowChangeCustom (iLocation);
					break;
				case SDL_MOUSEMOTION:
					SDL_GetMouseState (&iXPos, &iYPos);
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (event, 656, 0, 692, 387) == 1) /*** close ***/
						{
							if (iCloseCustomOn != 1) { iCloseCustomOn = 1; }
							ShowChangeCustom (iLocation);
						}
						if (InArea (event, 656, 387, 692, 455) == 1) /*** use ***/
						{
							if (iCustomUseOn != 1) { iCustomUseOn = 1; }
							ShowChangeCustom (iLocation);
						}
					}
					break;
				case SDL_MOUSEBUTTONUP:
					iCloseCustomOn = 0;
					iCustomUseOn = 0;
					if (event.button.button == 1) /*** left mouse button ***/
					{
						/*** foreground random ***/
						iTemp = 0;
						if (InArea (event, 33, 114, 46, 127) == 1)
							if (iChangeForeRand != 0) { iChangeForeRand = 0; iTemp = 1; }
						if (InArea (event, 48, 114, 61, 127) == 1)
							if (iChangeForeRand != 1) { iChangeForeRand = 1; iTemp = 1; }
						if (InArea (event, 63, 114, 76, 127) == 1)
							if (iChangeForeRand != 2) { iChangeForeRand = 2; iTemp = 1; }
						if (InArea (event, 78, 114, 91, 127) == 1)
							if (iChangeForeRand != 3) { iChangeForeRand = 3; iTemp = 1; }
						if (iTemp == 1) { PlaySound ("wav/check_box.wav"); }

						/*** foreground mod ***/
						if ((InArea (event, 114, 114, 127, 127) == 1) &&
							(iChangeForeMod != 0))
						{
							iChangeForeMod = 0;
							PlaySound ("wav/check_box.wav");
						}
						if ((InArea (event, 129, 114, 142, 127) == 1) &&
							(iChangeForeMod != 1))
						{
							iChangeForeMod = 1;
							PlaySound ("wav/check_box.wav");
						}

						/*** foreground code ***/
						if (InArea (event, 164, 109, 176, 128) == 1)
							{ ChangeCustom (-1, 1, 1); }
						if (InArea (event, 234, 109, 246, 128) == 1)
							{ ChangeCustom (1, 1, 1); }

						/*** modifier code 1 ***/
						if (InArea (event, 409, 109, 421, 128) == 1)
							{ ChangeCustom (-10, 2, 1); }
						if (InArea (event, 424, 109, 436, 128) == 1)
							{ ChangeCustom (-1, 2, 1); }
						if (InArea (event, 494, 109, 506, 128) == 1)
							{ ChangeCustom (1, 2, 1); }
						if (InArea (event, 509, 109, 521, 128) == 1)
							{ ChangeCustom (10, 2, 1); }

						if (iEditPoP == 2)
						{
							/*** modifier code 2 ***/
							if (InArea (event, 521, 22, 533, 41) == 1)
								{ ChangeCustom (-10, 2, 2); }
							if (InArea (event, 536, 22, 548, 41) == 1)
								{ ChangeCustom (-1, 2, 2); }
							if (InArea (event, 606, 22, 618, 41) == 1)
								{ ChangeCustom (1, 2, 2); }
							if (InArea (event, 621, 22, 633, 41) == 1)
								{ ChangeCustom (10, 2, 2); }

							/*** modifier code 3 ***/
							if (InArea (event, 521, 45, 533, 64) == 1)
								{ ChangeCustom (-10, 2, 3); }
							if (InArea (event, 536, 45, 548, 64) == 1)
								{ ChangeCustom (-1, 2, 3); }
							if (InArea (event, 606, 45, 618, 64) == 1)
								{ ChangeCustom (1, 2, 3); }
							if (InArea (event, 621, 45, 633, 64) == 1)
								{ ChangeCustom (10, 2, 3); }

							/*** modifier code 4 ***/
							if (InArea (event, 521, 68, 533, 87) == 1)
								{ ChangeCustom (-10, 2, 4); }
							if (InArea (event, 536, 68, 548, 87) == 1)
								{ ChangeCustom (-1, 2, 4); }
							if (InArea (event, 606, 68, 618, 87) == 1)
								{ ChangeCustom (1, 2, 4); }
							if (InArea (event, 621, 68, 633, 87) == 1)
								{ ChangeCustom (10, 2, 4); }
						}

						/*** CLOSE ***/
						if (InArea (event, 656, 0, 692, 387) == 1)
							{ iChangingCustom = 0; }

						/*** USE ***/
						if (InArea (event, 656, 387, 692, 455) == 1)
						{
							iTemp = 0;
							switch (iChangeForeRand)
							{
								case 0: break;
								case 1: iTemp+=64; break;
								case 2: iTemp+=128; break;
								case 3: iTemp+=192; break;
							}
							if (iChangeForeMod == 1) { iTemp+=32; }
							iTemp+=iChangeFore;
							SetLocation (iCurRoom, iLocation, iTemp, iChangeMod[1],
								iChangeMod[2], iChangeMod[3], iChangeMod[4]);
							iChanged++;
							iChangingCustom = 0;
							iChanging = 0;
						}
					}

					ShowChangeCustom (iLocation);
					break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	if (iChanging == 1) { PlaySound ("wav/ok_close.wav"); }

	return (iChanging);
}
/*****************************************************************************/
void ChangeGuards (int iLocation, SDL_Surface *screen)
/*****************************************************************************/
{
	SDL_Event event;
	int iChangingGuards;
	int iGuardType;
	int iGuardY;
	int iGuardNrDir;
	int iGuardNr;
	int iGuardDir;
	int iGuardX;
	int iRowGuard, iRowTo;
	int iEntryGuard, iRow, iColumn;
	int iTempCoor;

	iChangingGuards = 1;

	ShowChangeGuards (iLocation);
	while (iChangingGuards == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							iChangingGuards = 0; break;
						default: break;
					}
					ShowChangeGuards (iLocation);
					break;
				case SDL_MOUSEMOTION:
					SDL_GetMouseState (&iXPos, &iYPos);

					/*** X-coordinate tooltip ***/
					iGuardTooltipOld = iGuardTooltip;
					iGuardTooltip = 0;
					if (InArea (event, 290, 119, 403, 152) == 1)
						{ iGuardTooltip = 1; }
					if (InArea (event, 408, 119, 521, 152) == 1)
						{ iGuardTooltip = 2; }
					if (InArea (event, 526, 119, 639, 152) == 1)
						{ iGuardTooltip = 3; }
					if (InArea (event, 290, 260, 403, 293) == 1)
						{ iGuardTooltip = 4; }
					if (InArea (event, 408, 260, 521, 293) == 1)
						{ iGuardTooltip = 5; }
					if (iGuardTooltip != iGuardTooltipOld)
						{ ShowChangeGuards (iLocation); }
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (event, 656, 0, 692, 455) == 1) /*** close ***/
						{
							if (iCloseGuardsOn != 1) { iCloseGuardsOn = 1; }
							ShowChangeGuards (iLocation);
						}
					}
					break;
				case SDL_MOUSEBUTTONUP:
					iCloseGuardsOn = 0;
					if (event.button.button == 1) /*** left mouse button ***/
					{
						/*** guards in room ***/
						if (InArea (event, 544, 191, 556, 205) == 1)
						{
							iStaticGuards_Amount[iCurRoom - 1] = 0;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 559, 191, 571, 205) == 1)
						{
							iStaticGuards_Amount[iCurRoom - 1] = 1;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 573, 191, 587, 205) == 1)
						{
							iStaticGuards_Amount[iCurRoom - 1] = 2;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 588, 191, 602, 205) == 1)
						{
							iStaticGuards_Amount[iCurRoom - 1] = 3;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 603, 191, 617, 205) == 1)
						{
							iStaticGuards_Amount[iCurRoom - 1] = 4;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 618, 191, 632, 205) == 1)
						{
							iStaticGuards_Amount[iCurRoom - 1] = 5;
							iChanged++; PlaySound ("wav/check_box.wav");
						}

						/*** incoming sets ***/
						if (InArea (event, 566, 233, 580, 247) == 1)
						{
							iDynamicGuards_Sets[iCurRoom - 1] = 0;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 581, 233, 595, 247) == 1)
						{
							iDynamicGuards_Sets[iCurRoom - 1] = 1;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 596, 233, 610, 247) == 1)
						{
							iDynamicGuards_Sets[iCurRoom - 1] = 2;
							iChanged++; PlaySound ("wav/check_box.wav");
						}

						/*** incoming skill ***/
						PlusMinus (event, &iDynamicGuards_Skill[iCurRoom - 1],
							546, 271, 0, 255, -1, 1);
						PlusMinus (event, &iDynamicGuards_Skill[iCurRoom - 1],
							616, 271, 0, 255, +1, 1);

						/*** rows ***/
						iRowGuard = -1;
						iRowTo = -1;
						/*** row 1st ***/
						if (InArea (event, 366, 18, 366 + ROWW, 18 + ROWH) == 1)
							{ iRowGuard = 0; iRowTo = 0; }
						if (InArea (event, 366, 25, 366 + ROWW, 25 + ROWH) == 1)
							{ iRowGuard = 0; iRowTo = 10; }
						if (InArea (event, 366, 32, 366 + ROWW, 32 + ROWH) == 1)
							{ iRowGuard = 0; iRowTo = 20; }
						/*** row 2nd ***/
						if (InArea (event, 484, 18, 484 + ROWW, 18 + ROWH) == 1)
							{ iRowGuard = 1; iRowTo = 0; }
						if (InArea (event, 484, 25, 484 + ROWW, 25 + ROWH) == 1)
							{ iRowGuard = 1; iRowTo = 10; }
						if (InArea (event, 484, 32, 484 + ROWW, 32 + ROWH) == 1)
							{ iRowGuard = 1; iRowTo = 20; }
						/*** row 3rd ***/
						if (InArea (event, 602, 18, 602 + ROWW, 18 + ROWH) == 1)
							{ iRowGuard = 2; iRowTo = 0; }
						if (InArea (event, 602, 25, 602 + ROWW, 25 + ROWH) == 1)
							{ iRowGuard = 2; iRowTo = 10; }
						if (InArea (event, 602, 32, 602 + ROWW, 32 + ROWH) == 1)
							{ iRowGuard = 2; iRowTo = 20; }
						/*** row 4th ***/
						if (InArea (event, 366, 159, 366 + ROWW, 159 + ROWH) == 1)
							{ iRowGuard = 3; iRowTo = 0; }
						if (InArea (event, 366, 166, 366 + ROWW, 166 + ROWH) == 1)
							{ iRowGuard = 3; iRowTo = 10; }
						if (InArea (event, 366, 173, 366 + ROWW, 173 + ROWH) == 1)
							{ iRowGuard = 3; iRowTo = 20; }
						/*** row 5th ***/
						if (InArea (event, 484, 159, 484 + ROWW, 159 + ROWH) == 1)
							{ iRowGuard = 4; iRowTo = 0; }
						if (InArea (event, 484, 166, 484 + ROWW, 166 + ROWH) == 1)
							{ iRowGuard = 4; iRowTo = 10; }
						if (InArea (event, 484, 173, 484 + ROWW, 173 + ROWH) == 1)
							{ iRowGuard = 4; iRowTo = 20; }
						if (iRowGuard != -1)
						{
							iStaticGuards_1_Locations[iCurRoom - 1][iRowGuard] = iRowTo;
							iChanged++;
							PlaySound ("wav/hum_adj.wav");
						}

						/*** checkboxes ***/
						for (iGuardType = 1; iGuardType <= 13; iGuardType++)
						{
							switch (iGuardType)
							{
								case 1: iGuardY = GTYPE1; break;
								case 2: iGuardY = GTYPE2; break;
								case 3: iGuardY = GTYPE3; break;
								case 4: iGuardY = GTYPE4; break;
								case 5: iGuardY = GTYPE5; break;
								case 6: iGuardY = GTYPE6; break;
								case 7: iGuardY = GTYPE7; break;
								case 8: iGuardY = GTYPE8; break;
								case 9: iGuardY = GTYPE9; break;
								case 10: iGuardY = GTYPE10; break;
								case 11: iGuardY = GTYPE11; break;
								case 12: iGuardY = GTYPE12; break;
								case 13: iGuardY = GTYPE13; break;
								default: printf ("[ WARN ] Unknown iGuardType.\n");
							}
							for (iGuardNrDir = 1; iGuardNrDir <= 10; iGuardNrDir++)
							{
								switch (iGuardNrDir)
								{
									case 1: iGuardX = G1LEFT; break;
									case 2: iGuardX = G1RIGHT; break;
									case 3: iGuardX = G2LEFT; break;
									case 4: iGuardX = G2RIGHT; break;
									case 5: iGuardX = G3LEFT; break;
									case 6: iGuardX = G3RIGHT; break;
									case 7: iGuardX = G4LEFT; break;
									case 8: iGuardX = G4RIGHT; break;
									case 9: iGuardX = G5LEFT; break;
									case 10: iGuardX = G5RIGHT; break;
									default: printf ("[ WARN ] Unknown iGuardNrDir.\n");
								}
								if (InArea (event, iGuardX, iGuardY,
									iGuardX + 14, iGuardY + 14) == 1)
								{
									switch (iGuardNrDir)
									{
										case 1: iGuardNr = 0; iGuardDir = 255; break;
										case 2: iGuardNr = 0; iGuardDir = 0; break;
										case 3: iGuardNr = 1; iGuardDir = 255; break;
										case 4: iGuardNr = 1; iGuardDir = 0; break;
										case 5: iGuardNr = 2; iGuardDir = 255; break;
										case 6: iGuardNr = 2; iGuardDir = 0; break;
										case 7: iGuardNr = 3; iGuardDir = 255; break;
										case 8: iGuardNr = 3; iGuardDir = 0; break;
										case 9: iGuardNr = 4; iGuardDir = 255; break;
										case 10: iGuardNr = 4; iGuardDir = 0; break;
										default: printf ("[ WARN ] Unknown iGuardNrDir.\n");
									}
									iStaticGuards_4_Directions[iCurRoom - 1][iGuardNr] =
										iGuardDir;
									switch (iGuardType)
									{
										case 1: /*** black guard ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 0;
											iStaticGuards_10_Colors[iCurRoom - 1][iGuardNr] = 1;
											break;
										case 2: /*** blue guard ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 0;
											iStaticGuards_10_Colors[iCurRoom - 1][iGuardNr] = 2;
											break;
										case 3: /*** red guard ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 0;
											iStaticGuards_10_Colors[iCurRoom - 1][iGuardNr] = 3;
											break;
										case 4: /*** skeleton wakes up; wobbly if jump ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 2;
											iStaticGuards_18_SkelAct1[iCurRoom - 1][iGuardNr] = 0;
											iStaticGuards_19_SkelAct2[iCurRoom - 1][iGuardNr] = 0;
											break;
										case 5: /*** skeleton already awake ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 2;
											iStaticGuards_18_SkelAct1[iCurRoom - 1][iGuardNr] = 1;
											iStaticGuards_19_SkelAct2[iCurRoom - 1][iGuardNr] = 0;
											break;
										case 6: /*** skeleton only sleeps ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 2;
											iStaticGuards_18_SkelAct1[iCurRoom - 1][iGuardNr] = 255;
											iStaticGuards_19_SkelAct2[iCurRoom - 1][iGuardNr] = 255;
											break;
										case 7: /*** skeleton wakes up; NOT wobbly if jump ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 2;
											iStaticGuards_18_SkelAct1[iCurRoom - 1][iGuardNr] = 255;
											iStaticGuards_19_SkelAct2[iCurRoom - 1][iGuardNr] = 0;
											break;
										case 8: /*** head, long hair ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 5;
											break;
										case 9: /*** head, short hair ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 6;
											break;
										case 10: /*** snake ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 8;
											break;
										case 11: /*** birdman ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 7;
											break;
										case 12: /*** sword ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 1;
											break;
										case 13: /*** fake prince ***/
											iStaticGuards_16_Type[iCurRoom - 1][iGuardNr] = 3;
											break;
										default: printf ("[ WARN ] Unknown iGuardType.\n");
									}
									iChanged++;
									PlaySound ("wav/check_box.wav");
								}
							}
						}

						/*** entry ***/
						iEntryGuard = -1;
						/*** 1st ***/
						if (InArea (event, 303, 367, 319, 373) == 1)
							{ iEntryGuard = 0; iRow = 0; iColumn = 0; }
						if (InArea (event, 320, 367, 336, 373) == 1)
							{ iEntryGuard = 0; iRow = 0; iColumn = 9; }
						if (InArea (event, 303, 374, 319, 380) == 1)
							{ iEntryGuard = 0; iRow = 1; iColumn = 0; }
						if (InArea (event, 320, 374, 336, 380) == 1)
							{ iEntryGuard = 0; iRow = 1; iColumn = 9; }
						if (InArea (event, 303, 381, 319, 387) == 1)
							{ iEntryGuard = 0; iRow = 2; iColumn = 0; }
						if (InArea (event, 320, 381, 336, 387) == 1)
							{ iEntryGuard = 0; iRow = 2; iColumn = 9; }
						/*** 2nd ***/
						if (InArea (event, 474, 367, 490, 373) == 1)
							{ iEntryGuard = 1; iRow = 0; iColumn = 0; }
						if (InArea (event, 491, 367, 507, 373) == 1)
							{ iEntryGuard = 1; iRow = 0; iColumn = 9; }
						if (InArea (event, 474, 374, 490, 380) == 1)
							{ iEntryGuard = 1; iRow = 1; iColumn = 0; }
						if (InArea (event, 491, 374, 507, 380) == 1)
							{ iEntryGuard = 1; iRow = 1; iColumn = 9; }
						if (InArea (event, 474, 381, 490, 387) == 1)
							{ iEntryGuard = 1; iRow = 2; iColumn = 0; }
						if (InArea (event, 491, 381, 507, 387) == 1)
							{ iEntryGuard = 1; iRow = 2; iColumn = 9; }
						if (iEntryGuard != -1)
						{
							iDynamicGuards_3_Row[iCurRoom - 1][iEntryGuard] = iRow;
							iDynamicGuards_4_Column[iCurRoom - 1][iEntryGuard] = iColumn;
							iChanged++;
							PlaySound ("wav/hum_adj.wav");
						}

						/*** spawn if guard(s) alive ***/
						/*** 1st ***/
						if (InArea (event, 354, 321, 368, 335) == 1)
						{
							iDynamicGuards_8_SpIfAl[iCurRoom - 1][0] = 0;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 369, 321, 383, 335) == 1)
						{
							iDynamicGuards_8_SpIfAl[iCurRoom - 1][0] = 1;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 384, 321, 398, 335) == 1)
						{
							iDynamicGuards_8_SpIfAl[iCurRoom - 1][0] = 2;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						/*** 2nd ***/
						if (InArea (event, 525, 321, 539, 335) == 1)
						{
							iDynamicGuards_8_SpIfAl[iCurRoom - 1][1] = 0;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 540, 321, 554, 335) == 1)
						{
							iDynamicGuards_8_SpIfAl[iCurRoom - 1][1] = 1;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 555, 321, 569, 335) == 1)
						{
							iDynamicGuards_8_SpIfAl[iCurRoom - 1][1] = 2;
							iChanged++; PlaySound ("wav/check_box.wav");
						}

						/*** max alive ***/
						/*** 1st ***/
						if (InArea (event, 438, 311, 452, 325) == 1)
						{
							iDynamicGuards_2_MaxTog[iCurRoom - 1][0] = 1;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 453, 311, 467, 325) == 1)
						{
							iDynamicGuards_2_MaxTog[iCurRoom - 1][0] = 2;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						/*** 2nd ***/
						if (InArea (event, 609, 311, 623, 325) == 1)
						{
							iDynamicGuards_2_MaxTog[iCurRoom - 1][1] = 1;
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 624, 311, 638, 325) == 1)
						{
							iDynamicGuards_2_MaxTog[iCurRoom - 1][1] = 2;
							iChanged++; PlaySound ("wav/check_box.wav");
						}

						/*** can jump ***/
						/*** 1st ***/
						if (InArea (event, 438, 331, 452, 345) == 1)
						{
							iDynamicGuards_7_CanJump[iCurRoom - 1][0] = 255; /*** N ***/
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 453, 331, 467, 345) == 1)
						{
							iDynamicGuards_7_CanJump[iCurRoom - 1][0] = 1; /*** Y ***/
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						/*** 2nd ***/
						if (InArea (event, 609, 331, 623, 345) == 1)
						{
							iDynamicGuards_7_CanJump[iCurRoom - 1][1] = 255; /*** N ***/
							iChanged++; PlaySound ("wav/check_box.wav");
						}
						if (InArea (event, 624, 331, 638, 345) == 1)
						{
							iDynamicGuards_7_CanJump[iCurRoom - 1][1] = 1; /*** Y ***/
							iChanged++; PlaySound ("wav/check_box.wav");
						}

						/*** hit points, dynamic ***/
						/*** 1st ***/
						PlusMinus (event, &iDynamicGuards_10_Hitpoints[iCurRoom - 1][0],
							385, 350, 0, 128, -1, 1);
						PlusMinus (event, &iDynamicGuards_10_Hitpoints[iCurRoom - 1][0],
							455, 350, 0, 128, 1, 1);
						/*** 2nd ***/
						PlusMinus (event, &iDynamicGuards_10_Hitpoints[iCurRoom - 1][1],
							556, 350, 0, 128, -1, 1);
						PlusMinus (event, &iDynamicGuards_10_Hitpoints[iCurRoom - 1][1],
							626, 350, 0, 128, 1, 1);

						/*** nr. of guards ***/
						/*** 1st ***/
						PlusMinus (event, &iDynamicGuards_9_Amount[iCurRoom - 1][0],
							385, 372, 0, 9, -1, 1);
						PlusMinus (event, &iDynamicGuards_9_Amount[iCurRoom - 1][0],
							455, 372, 0, 9, 1, 1);
						/*** 2nd ***/
						PlusMinus (event, &iDynamicGuards_9_Amount[iCurRoom - 1][1],
							556, 372, 0, 9, -1, 1);
						PlusMinus (event, &iDynamicGuards_9_Amount[iCurRoom - 1][1],
							626, 372, 0, 9, 1, 1);

						/*** before spawn ***/
						/*** 1st ***/
						PlusMinus (event, &iDynamicGuards_5_LatBefore[iCurRoom - 1][0],
							355, 394, 0, 255, -10, 1);
						PlusMinus (event, &iDynamicGuards_5_LatBefore[iCurRoom - 1][0],
							370, 394, 0, 255, -1, 1);
						PlusMinus (event, &iDynamicGuards_5_LatBefore[iCurRoom - 1][0],
							440, 394, 0, 255, 1, 1);
						PlusMinus (event, &iDynamicGuards_5_LatBefore[iCurRoom - 1][0],
							455, 394, 0, 255, 10, 1);
						/*** 2nd ***/
						PlusMinus (event, &iDynamicGuards_5_LatBefore[iCurRoom - 1][1],
							526, 394, 0, 255, -10, 1);
						PlusMinus (event, &iDynamicGuards_5_LatBefore[iCurRoom - 1][1],
							541, 394, 0, 255, -1, 1);
						PlusMinus (event, &iDynamicGuards_5_LatBefore[iCurRoom - 1][1],
							611, 394, 0, 255, 1, 1);
						PlusMinus (event, &iDynamicGuards_5_LatBefore[iCurRoom - 1][1],
							626, 394, 0, 255, 10, 1);

						/*** between spawns ***/
						/*** 1st ***/
						PlusMinus (event, &iDynamicGuards_6_LatBetween[iCurRoom - 1][0],
							355, 416, 0, 255, -10, 1);
						PlusMinus (event, &iDynamicGuards_6_LatBetween[iCurRoom - 1][0],
							370, 416, 0, 255, -1, 1);
						PlusMinus (event, &iDynamicGuards_6_LatBetween[iCurRoom - 1][0],
							440, 416, 0, 255, 1, 1);
						PlusMinus (event, &iDynamicGuards_6_LatBetween[iCurRoom - 1][0],
							455, 416, 0, 255, 10, 1);
						/*** 2nd ***/
						PlusMinus (event, &iDynamicGuards_6_LatBetween[iCurRoom - 1][1],
							526, 416, 0, 255, -10, 1);
						PlusMinus (event, &iDynamicGuards_6_LatBetween[iCurRoom - 1][1],
							541, 416, 0, 255, -1, 1);
						PlusMinus (event, &iDynamicGuards_6_LatBetween[iCurRoom - 1][1],
							611, 416, 0, 255, 1, 1);
						PlusMinus (event, &iDynamicGuards_6_LatBetween[iCurRoom - 1][1],
							626, 416, 0, 255, 10, 1);

						/*** skill ***/
						/*** 1st ***/
						PlusMinus (event, &iStaticGuards_5_Skills[iCurRoom - 1][0],
							305, 56, 0, 255, -1, 1);
						PlusMinus (event, &iStaticGuards_5_Skills[iCurRoom - 1][0],
							375, 56, 0, 255, 1, 1);
						/*** 2nd ***/
						PlusMinus (event, &iStaticGuards_5_Skills[iCurRoom - 1][1],
							423, 56, 0, 255, -1, 1);
						PlusMinus (event, &iStaticGuards_5_Skills[iCurRoom - 1][1],
							493, 56, 0, 255, 1, 1);
						/*** 3rd ***/
						PlusMinus (event, &iStaticGuards_5_Skills[iCurRoom - 1][2],
							541, 56, 0, 255, -1, 1);
						PlusMinus (event, &iStaticGuards_5_Skills[iCurRoom - 1][2],
							611, 56, 0, 255, 1, 1);
						/*** 4th ***/
						PlusMinus (event, &iStaticGuards_5_Skills[iCurRoom - 1][3],
							305, 197, 0, 255, -1, 1);
						PlusMinus (event, &iStaticGuards_5_Skills[iCurRoom - 1][3],
							375, 197, 0, 255, 1, 1);
						/*** 5th ***/
						PlusMinus (event, &iStaticGuards_5_Skills[iCurRoom - 1][4],
							423, 197, 0, 255, -1, 1);
						PlusMinus (event, &iStaticGuards_5_Skills[iCurRoom - 1][4],
							493, 197, 0, 255, 1, 1);

						/*** hit points, static ***/
						/*** 1st ***/
						PlusMinus (event, &iStaticGuards_17_Hitpoints[iCurRoom - 1][0],
							305, 94, 0, 128, -1, 1);
						PlusMinus (event, &iStaticGuards_17_Hitpoints[iCurRoom - 1][0],
							375, 94, 0, 128, 1, 1);
						/*** 2nd ***/
						PlusMinus (event, &iStaticGuards_17_Hitpoints[iCurRoom - 1][1],
							423, 94, 0, 128, -1, 1);
						PlusMinus (event, &iStaticGuards_17_Hitpoints[iCurRoom - 1][1],
							493, 94, 0, 128, 1, 1);
						/*** 3rd ***/
						PlusMinus (event, &iStaticGuards_17_Hitpoints[iCurRoom - 1][2],
							541, 94, 0, 128, -1, 1);
						PlusMinus (event, &iStaticGuards_17_Hitpoints[iCurRoom - 1][2],
							611, 94, 0, 128, 1, 1);
						/*** 4th ***/
						PlusMinus (event, &iStaticGuards_17_Hitpoints[iCurRoom - 1][3],
							305, 235, 0, 128, -1, 1);
						PlusMinus (event, &iStaticGuards_17_Hitpoints[iCurRoom - 1][3],
							375, 235, 0, 128, 1, 1);
						/*** 5th ***/
						PlusMinus (event, &iStaticGuards_17_Hitpoints[iCurRoom - 1][4],
							423, 235, 0, 128, -1, 1);
						PlusMinus (event, &iStaticGuards_17_Hitpoints[iCurRoom - 1][4],
							493, 235, 0, 128, 1, 1);

						/*** X-coordinate ***/
						/*** 1st ***/
						iTempCoor = IntsToHexToSSLittleEndian
							(iStaticGuards_3_Offsets2[iCurRoom - 1][0],
							iStaticGuards_2_Offsets1[iCurRoom - 1][0]);
						PlusMinus (event, &iTempCoor, 290, 132, 0, 608, -10, 1);
						PlusMinus (event, &iTempCoor, 305, 132, 0, 608, -1, 1);
						PlusMinus (event, &iTempCoor, 375, 132, 0, 608, 1, 1);
						PlusMinus (event, &iTempCoor, 390, 132, 0, 608, 10, 1);
						SSLittleEndianToHexToInts (iTempCoor,
							&iStaticGuards_3_Offsets2[iCurRoom - 1][0],
							&iStaticGuards_2_Offsets1[iCurRoom - 1][0]);
						/*** 2nd ***/
						iTempCoor = IntsToHexToSSLittleEndian
							(iStaticGuards_3_Offsets2[iCurRoom - 1][1],
							iStaticGuards_2_Offsets1[iCurRoom - 1][1]);
						PlusMinus (event, &iTempCoor, 408, 132, 0, 608, -10, 1);
						PlusMinus (event, &iTempCoor, 423, 132, 0, 608, -1, 1);
						PlusMinus (event, &iTempCoor, 493, 132, 0, 608, 1, 1);
						PlusMinus (event, &iTempCoor, 508, 132, 0, 608, 10, 1);
						SSLittleEndianToHexToInts (iTempCoor,
							&iStaticGuards_3_Offsets2[iCurRoom - 1][1],
							&iStaticGuards_2_Offsets1[iCurRoom - 1][1]);
						/*** 3rd ***/
						iTempCoor = IntsToHexToSSLittleEndian
							(iStaticGuards_3_Offsets2[iCurRoom - 1][2],
							iStaticGuards_2_Offsets1[iCurRoom - 1][2]);
						PlusMinus (event, &iTempCoor, 526, 132, 0, 608, -10, 1);
						PlusMinus (event, &iTempCoor, 541, 132, 0, 608, -1, 1);
						PlusMinus (event, &iTempCoor, 611, 132, 0, 608, 1, 1);
						PlusMinus (event, &iTempCoor, 626, 132, 0, 608, 10, 1);
						SSLittleEndianToHexToInts (iTempCoor,
							&iStaticGuards_3_Offsets2[iCurRoom - 1][2],
							&iStaticGuards_2_Offsets1[iCurRoom - 1][2]);
						/*** 4th ***/
						iTempCoor = IntsToHexToSSLittleEndian
							(iStaticGuards_3_Offsets2[iCurRoom - 1][3],
							iStaticGuards_2_Offsets1[iCurRoom - 1][3]);
						PlusMinus (event, &iTempCoor, 290, 273, 0, 608, -10, 1);
						PlusMinus (event, &iTempCoor, 305, 273, 0, 608, -1, 1);
						PlusMinus (event, &iTempCoor, 375, 273, 0, 608, 1, 1);
						PlusMinus (event, &iTempCoor, 390, 273, 0, 608, 10, 1);
						SSLittleEndianToHexToInts (iTempCoor,
							&iStaticGuards_3_Offsets2[iCurRoom - 1][3],
							&iStaticGuards_2_Offsets1[iCurRoom - 1][3]);
						/*** 5th ***/
						iTempCoor = IntsToHexToSSLittleEndian
							(iStaticGuards_3_Offsets2[iCurRoom - 1][4],
							iStaticGuards_2_Offsets1[iCurRoom - 1][4]);
						PlusMinus (event, &iTempCoor, 408, 273, 0, 608, -10, 1);
						PlusMinus (event, &iTempCoor, 423, 273, 0, 608, -1, 1);
						PlusMinus (event, &iTempCoor, 493, 273, 0, 608, 1, 1);
						PlusMinus (event, &iTempCoor, 508, 273, 0, 608, 10, 1);
						SSLittleEndianToHexToInts (iTempCoor,
							&iStaticGuards_3_Offsets2[iCurRoom - 1][4],
							&iStaticGuards_2_Offsets1[iCurRoom - 1][4]);

						/*** CLOSE ***/
						if (InArea (event, 656, 0, 692, 455) == 1)
							{ iChangingGuards = 0; }
					}
					ShowChangeGuards (iLocation);
					break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/ok_close.wav");
}
/*****************************************************************************/
void ChangeMusic (int iLocation, SDL_Surface *screen)
/*****************************************************************************/
{
	SDL_Event event;
	int iChangingMusic;
	int iTemp;
	int iX1, iY1, iX2, iY2;
	int iTile;

	iChangingMusic = 1;

	ShowChangeMusic (iLocation);
	while (iChangingMusic == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							iChangingMusic = 0; break;
						default: break;
					}
					ShowChangeMusic (iLocation);
					break;
				case SDL_MOUSEMOTION:
					SDL_GetMouseState (&iXPos, &iYPos);
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (event, 32, 115, 103, 144) == 1) /*** music all ***/
							{ if (iMAllOn != 1) { iMAllOn = 1; } }
						if (InArea (event, 32, 145, 103, 174) == 1) /*** music none ***/
							{ if (iMNoneOn != 1) { iMNoneOn = 1; } }
						if (InArea (event, 104, 115, 175, 144) == 1) /*** music left ***/
							{ if (iMLeftOn != 1) { iMLeftOn = 1; } }
						if (InArea (event, 104, 145, 175, 174) == 1) /*** music right ***/
							{ if (iMRightOn != 1) { iMRightOn = 1; } }
						if (InArea (event, 481, 115, 552, 144) == 1) /*** music top ***/
							{ if (iMTopOn != 1) { iMTopOn = 1; } }
						if (InArea (event, 481, 145, 552, 174) == 1) /*** music middle ***/
							{ if (iMMiddleOn != 1) { iMMiddleOn = 1; } }
						if (InArea (event, 553, 115, 624, 144) == 1) /*** music bottom ***/
							{ if (iMBottomOn != 1) { iMBottomOn = 1; } }
						if (InArea (event, 553, 145, 624, 174) == 1) /*** music invert ***/
							{ if (iMInvertOn != 1) { iMInvertOn = 1; } }

						if (InArea (event, 656, 0, 692, 455) == 1) /*** close ***/
						{
							if (iCloseMusicOn != 1) { iCloseMusicOn = 1; }
						}
						ShowChangeMusic (iLocation);
					}
					break;
				case SDL_MOUSEBUTTONUP:
					iCloseMusicOn = 0;
					iMAllOn = 0;
					iMNoneOn = 0;
					iMLeftOn = 0;
					iMRightOn = 0;
					iMTopOn = 0;
					iMMiddleOn = 0;
					iMBottomOn = 0;
					iMInvertOn = 0;
					if (event.button.button == 1) /*** left mouse button ***/
					{
						/*** music checkboxes ***/
						for (iTemp = 0; iTemp < 30; iTemp++)
						{
							if ((iTemp >= 0) && (iTemp <= 9)) { iY1 = 130; }
							if ((iTemp >= 10) && (iTemp <= 19)) { iY1 = 145; }
							if ((iTemp >= 20) && (iTemp <= 29)) { iY1 = 160; }
							switch (iTemp)
							{
								case 0: case 10: case 20: iX1 = 253 + (0 * 15); break;
								case 1: case 11: case 21: iX1 = 253 + (1 * 15); break;
								case 2: case 12: case 22: iX1 = 253 + (2 * 15); break;
								case 3: case 13: case 23: iX1 = 253 + (3 * 15); break;
								case 4: case 14: case 24: iX1 = 253 + (4 * 15); break;
								case 5: case 15: case 25: iX1 = 253 + (5 * 15); break;
								case 6: case 16: case 26: iX1 = 253 + (6 * 15); break;
								case 7: case 17: case 27: iX1 = 253 + (7 * 15); break;
								case 8: case 18: case 28: iX1 = 253 + (8 * 15); break;
								case 9: case 19: case 29: iX1 = 253 + (9 * 15); break;
							}
							iX2 = iX1 + 14;
							iY2 = iY1 + 14;
							if (InArea (event, iX1, iY1, iX2, iY2) == 1)
							{
								if (sMusic[((iCurRoom - 1) * 30) + iTemp] == 1)
								{
									sMusic[((iCurRoom - 1) * 30) + iTemp] = 0;
								} else {
									sMusic[((iCurRoom - 1) * 30) + iTemp] = 1;
								}
								ShowChangeMusic (iLocation);
								PlaySound ("wav/check_box.wav");
								iChanged++;
							}
						}

						/*** music buttons ***/
						if (InArea (event, 32, 115, 103, 144) == 1)
						{ /*** music all ***/
							for (iTile = 0; iTile < 30; iTile++)
								{ sMusic[((iCurRoom - 1) * 30) + iTile] = 1; }
							PlaySound ("wav/screen2or3.wav");
							iChanged++;
						}
						if (InArea (event, 32, 145, 103, 174) == 1)
						{ /*** music none ***/
							for (iTile = 0; iTile < 30; iTile++)
								{ sMusic[((iCurRoom - 1) * 30) + iTile] = 0; }
							PlaySound ("wav/screen2or3.wav");
							iChanged++;
						}
						if (InArea (event, 104, 115, 175, 144) == 1)
						{ /*** music left ***/
							for (iTile = 0; iTile < 30; iTile++)
							{
								if (((iTile >= 0) && (iTile <= 4)) ||
									((iTile >= 10) && (iTile <= 14)) ||
									((iTile >= 20) && (iTile <= 24)))
									{ sMusic[((iCurRoom - 1) * 30) + iTile] = 1; }
										else { sMusic[((iCurRoom - 1) * 30) + iTile] = 0; }
							}
							PlaySound ("wav/screen2or3.wav");
							iChanged++;
						}
						if (InArea (event, 104, 145, 175, 174) == 1)
						{ /*** music right ***/
							for (iTile = 0; iTile < 30; iTile++)
							{
								if (((iTile >= 5) && (iTile <= 9)) ||
									((iTile >= 15) && (iTile <= 19)) ||
									((iTile >= 25) && (iTile <= 29)))
									{ sMusic[((iCurRoom - 1) * 30) + iTile] = 1; }
										else { sMusic[((iCurRoom - 1) * 30) + iTile] = 0; }
							}
							PlaySound ("wav/screen2or3.wav");
							iChanged++;
						}
						if (InArea (event, 481, 115, 552, 144) == 1)
						{ /*** music top ***/
							for (iTile = 0; iTile < 30; iTile++)
							{
								if ((iTile >= 0) && (iTile <= 9))
									{ sMusic[((iCurRoom - 1) * 30) + iTile] = 1; }
										else { sMusic[((iCurRoom - 1) * 30) + iTile] = 0; }
							}
							PlaySound ("wav/screen2or3.wav");
							iChanged++;
						}
						if (InArea (event, 481, 145, 552, 174) == 1)
						{ /*** music middle ***/
							for (iTile = 0; iTile < 30; iTile++)
							{
								if ((iTile >= 10) && (iTile <= 19))
									{ sMusic[((iCurRoom - 1) * 30) + iTile] = 1; }
										else { sMusic[((iCurRoom - 1) * 30) + iTile] = 0; }
							}
							PlaySound ("wav/screen2or3.wav");
							iChanged++;
						}
						if (InArea (event, 553, 115, 624, 144) == 1)
						{ /*** music bottom ***/
							for (iTile = 0; iTile < 30; iTile++)
							{
								if ((iTile >= 20) && (iTile <= 29))
									{ sMusic[((iCurRoom - 1) * 30) + iTile] = 1; }
										else { sMusic[((iCurRoom - 1) * 30) + iTile] = 0; }
							}
							PlaySound ("wav/screen2or3.wav");
							iChanged++;
						}
						if (InArea (event, 553, 145, 624, 174) == 1)
						{ /*** music invert ***/
							for (iTile = 0; iTile < 30; iTile++)
							{
								if (sMusic[((iCurRoom - 1) * 30) + iTile] == 0)
									{ sMusic[((iCurRoom - 1) * 30) + iTile] = 1; }
										else { sMusic[((iCurRoom - 1) * 30) + iTile] = 0; }
							}
							PlaySound ("wav/screen2or3.wav");
							iChanged++;
						}

						/*** CLOSE ***/
						if (InArea (event, 656, 0, 692, 455) == 1)
							{ iChangingMusic = 0; }
					}
					ShowChangeMusic (iLocation);
					break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/ok_close.wav");
}
/*****************************************************************************/
int ChangeBackground (int iLocation, SDL_Surface *screen)
/*****************************************************************************/
{
	SDL_Event event;
	int iChangingBackground;
	int iChanging;
	int iRoom, iTile;
	int iBK;

	iChanging = 1;

	iChangingBackground = 1;

	ShowChangeBackground (iLocation);
	while (iChangingBackground == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					iBK = 0;
					switch (event.key.keysym.sym)
					{
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
							if ((event.key.keysym.mod & KMOD_LSHIFT) ||
								(event.key.keysym.mod & KMOD_RSHIFT))
							{
								for (iTile = 0; iTile < 30; iTile++)
								{
									iModifierA[iCurRoom][iTile][3] = iMod3Hi;
									iModifierA[iCurRoom][iTile][4] = iMod4Hi;
								}
							} else if ((event.key.keysym.mod & KMOD_LCTRL) ||
								(event.key.keysym.mod & KMOD_RCTRL))
							{
								for (iRoom = 1; iRoom <= iRooms; iRoom++)
								{
									for (iTile = 0; iTile < 30; iTile++)
									{
										iModifierA[iRoom][iTile][3] = iMod3Hi;
										iModifierA[iRoom][iTile][4] = iMod4Hi;
									}
								}
							} else {
								iModifierA[iCurRoom][iLocation - 1][3] = iMod3Hi;
								iModifierA[iCurRoom][iLocation - 1][4] = iMod4Hi;
							}
							iChanged++; iChangingBackground = 0; iChanging = 0;
							break;

						case SDLK_PERIOD: case SDLK_KP_PERIOD:
							iBK = BackKey (event, '.', iLocation); break;
						case SDLK_a: iBK = BackKey (event, 'a', iLocation); break;
						case SDLK_b: iBK = BackKey (event, 'b', iLocation); break;
						case SDLK_c: iBK = BackKey (event, 'c', iLocation); break;
						case SDLK_d: iBK = BackKey (event, 'd', iLocation); break;
						case SDLK_e: iBK = BackKey (event, 'e', iLocation); break;
						case SDLK_f: iBK = BackKey (event, 'f', iLocation); break;
						case SDLK_g: iBK = BackKey (event, 'g', iLocation); break;
						case SDLK_h: iBK = BackKey (event, 'h', iLocation); break;
						case SDLK_i: iBK = BackKey (event, 'i', iLocation); break;
						case SDLK_j: iBK = BackKey (event, 'j', iLocation); break;
						case SDLK_k: iBK = BackKey (event, 'k', iLocation); break;
						case SDLK_l: iBK = BackKey (event, 'l', iLocation); break;
						case SDLK_m: iBK = BackKey (event, 'm', iLocation); break;
						case SDLK_n: iBK = BackKey (event, 'n', iLocation); break;
						case SDLK_o: iBK = BackKey (event, 'o', iLocation); break;
						case SDLK_p: iBK = BackKey (event, 'p', iLocation); break;
						case SDLK_q: iBK = BackKey (event, 'q', iLocation); break;
						case SDLK_r: iBK = BackKey (event, 'r', iLocation); break;
						case SDLK_s: iBK = BackKey (event, 's', iLocation); break;
						case SDLK_t: iBK = BackKey (event, 't', iLocation); break;
						case SDLK_u: iBK = BackKey (event, 'u', iLocation); break;
						case SDLK_v: iBK = BackKey (event, 'v', iLocation); break;
						case SDLK_w: iBK = BackKey (event, 'w', iLocation); break;
						case SDLK_x: iBK = BackKey (event, 'x', iLocation); break;
						case SDLK_y: iBK = BackKey (event, 'y', iLocation); break;
						case SDLK_z: iBK = BackKey (event, 'z', iLocation); break;
						case SDLK_0: case SDLK_KP0:
							iBK = BackKey (event, '0', iLocation); break;
						case SDLK_1: case SDLK_KP1:
							iBK = BackKey (event, '1', iLocation); break;
						case SDLK_2: case SDLK_KP2:
							iBK = BackKey (event, '2', iLocation); break;
						case SDLK_3: case SDLK_KP3:
							iBK = BackKey (event, '3', iLocation); break;
						case SDLK_4: case SDLK_KP4:
							iBK = BackKey (event, '4', iLocation); break;
						case SDLK_5: case SDLK_KP5:
							iBK = BackKey (event, '5', iLocation); break;
						case SDLK_6: case SDLK_KP6:
							iBK = BackKey (event, '6', iLocation); break;
						case SDLK_7: case SDLK_KP7:
							iBK = BackKey (event, '7', iLocation); break;
						case SDLK_8: case SDLK_KP8:
							iBK = BackKey (event, '8', iLocation); break;
						case SDLK_9: case SDLK_KP9:
							iBK = BackKey (event, '9', iLocation); break;

						case SDLK_ESCAPE:
							iChangingBackground = 0; break;
						default: break;
					}
					if (iBK == 1)
						{ iChanged++; iChangingBackground = 0; iChanging = 0; }
					ShowChangeBackground (iLocation);
					break;
				case SDL_MOUSEMOTION:
					SDL_GetMouseState (&iXPos, &iYPos);
					HiSetBack (event, iLocation, 0);
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						/*** CLOSE ***/
						if (InArea (event, 656, 0, 692, 455) == 1)
						{
							if (iCloseBackgroundOn != 1) { iCloseBackgroundOn = 1; }
							ShowChangeBackground (iLocation);
						}
					}
					break;
				case SDL_MOUSEBUTTONUP:
					iCloseBackgroundOn = 0;
					if (event.button.button == 1) /*** left mouse button ***/
					{
						if (HiSetBack (event, iLocation, 1) == 1)
							{ iChanged++; iChangingBackground = 0; iChanging = 0; }

						/*** CLOSE ***/
						if (InArea (event, 656, 0, 692, 455) == 1)
							{ iChangingBackground = 0; }
					}
					if (event.button.button == 2) /*** middle mouse button ***/
					{
						if (HiSetBack (event, iLocation, 2) == 1)
							{ iChanged++; iChangingBackground = 0; iChanging = 0; }
					}
					if (event.button.button == 3) /*** right mouse button ***/
					{
						if (HiSetBack (event, iLocation, 3) == 1)
							{ iChanged++; iChangingBackground = 0; iChanging = 0; }
					}
					ShowChangeBackground (iLocation);
					break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	if (iChanging == 1) { PlaySound ("wav/ok_close.wav"); }

	return (iChanging);
}
/*****************************************************************************/
void ChangeEvent (int iAmount, int iChangePos)
/*****************************************************************************/
{
	if (((iAmount > 0) && (iChangeEvent != 255)) ||
		((iAmount < 0) && (iChangeEvent != 0)))
	{
		switch (iAmount)
		{
			case -10: iChangeEvent-=10; break;
			case -1: iChangeEvent--; break;
			case 1: iChangeEvent++; break;
			case 10: iChangeEvent+=10; break;
		}
		if (iChangeEvent < 0) { iChangeEvent = 0; }
		if (iChangeEvent > 255) { iChangeEvent = 255; }
		if (iChangePos == 1)
		{
			if (iEditPoP == 1)
			{
				if ((iThingA[iCurRoom][iSelected - 1] == 6) ||
					(iThingA[iCurRoom][iSelected - 1] == 15) ||
					(iThingA[iCurRoom][iSelected - 1] == 38) ||
					(iThingA[iCurRoom][iSelected - 1] == 47))
				{
					iModifierA[iCurRoom][iSelected - 1][1] = iChangeEvent;
					iChanged++;
				}
			} else {
				if ((iThingA[iCurRoom][iSelected - 1] == 5) ||
					(iThingA[iCurRoom][iSelected - 1] == 6) ||
					(iThingA[iCurRoom][iSelected - 1] == 34))
				{
					iModifierA[iCurRoom][iSelected - 1][1] = iChangeEvent;
					iChanged++;
				}
			}
		}
		PlaySound ("wav/plus_minus.wav");
	}
}
/*****************************************************************************/
void ChangeCustom (int iAmount, int iForeBack, int iArNr)
/*****************************************************************************/
{
	switch (iForeBack)
	{
		case 1: /*** fore ***/
			if (((iAmount == 1) && (iChangeFore != 31)) ||
				((iAmount == -1) && (iChangeFore != 0)))
			{
				switch (iAmount)
				{
					case -1: iChangeFore--; break;
					case 1: iChangeFore++; break;
				}
				PlaySound ("wav/plus_minus.wav");
			}
			break;
		case 2: /*** back ***/
			if (((iAmount > 0) && (iChangeMod[iArNr] != 255)) ||
				((iAmount < 0) && (iChangeMod[iArNr] != 0)))
			{
				switch (iAmount)
				{
					case -10: iChangeMod[iArNr]-=10; break;
					case -1: iChangeMod[iArNr]--; break;
					case 1: iChangeMod[iArNr]++; break;
					case 10: iChangeMod[iArNr]+=10; break;
				}
				if (iChangeMod[iArNr] < 0) { iChangeMod[iArNr] = 0; }
				if (iChangeMod[iArNr] > 255) { iChangeMod[iArNr] = 255; }
				PlaySound ("wav/plus_minus.wav");
			}
			break;
	}
}
/*****************************************************************************/
int IsDisabled (int iTile)
/*****************************************************************************/
{
	if (iEditPoP == 2) { return (0); }

	if (((iTile == 73) || (iTile == 74)) && ((int)luLevelNr != 12))
	{
		PrIfDe ("[  OK  ] State of shadow: disabled\n");
		return (1);
	}
	else if (((iTile == 75) || (iTile == 76)) && ((int)luLevelNr != 6))
	{
		PrIfDe ("[  OK  ] State of fat guard: disabled\n");
		return (1);
	}
	else if (((iTile == 77) || (iTile == 78)) && ((int)luLevelNr != 3))
	{
		PrIfDe ("[  OK  ] State of skeleton: disabled\n");
		return (1);
	}
	else if (((iTile == 108) || (iTile == 109)) && ((int)luLevelNr != 13))
	{
		PrIfDe ("[  OK  ] State of jaffar: disabled\n");
		return (1);
	}
	else if (((iTile >= 92) && (iTile <= 105)) &&
		(((int)luLevelNr == 14) || ((int)luLevelNr == 15) || ((int)luLevelNr ==
		3) || ((int)luLevelNr == 6) || ((int)luLevelNr == 13) || ((int)luLevelNr
		== 12)))
	{
		PrIfDe ("[  OK  ] State of guards: disabled\n");
		return (1);
	} else {
		return (0);
	}
}
/*****************************************************************************/
void UseTile (int iTile, int iLocation, int iRoom)
/*****************************************************************************/
{
	int iTemp;

	if (iTile == -1)
	{
		switch (cCurType)
		{
			case 'd':
				do {
					iTile = 1 + (int) (72.0 * RANDOM() / (RAND_MAX + 1.0));
				} while ((iTile == 48) || (iTile == 64) || (iTile == 65) ||
				(iTile == 67) || (iTile == 68) || (iTile == 69) || (iTile == 70));
				break;
			case 'p':
				do {
					iTile = 1 + (int) (72.0 * RANDOM() / (RAND_MAX + 1.0));
				} while (iTile == 62);
				break;
			case 'o':
				iTemp = 1 + (int) (3.0 * RANDOM() / (RAND_MAX + 1.0));
				switch (iTemp)
				{
					case 1: iTile = 17; break;
					case 2: iTile = 18; break;
					case 3: iTile = 19; break;
				}
				break;
			case 'c':
				do {
					iTile = 1 + (int) (57.0 * RANDOM() / (RAND_MAX + 1.0));
				} while ((iTile == 36) || (iTile == 51));
				break;
			case 'r':
				iTile = 1 + (int) (55.0 * RANDOM() / (RAND_MAX + 1.0));
				break;
			case 't':
				do {
					iTile = 1 + (int) (58.0 * RANDOM() / (RAND_MAX + 1.0));
				} while (((iTile >= 27) && (iTile <= 31)) || ((iTile >= 40) &&
					(iTile <= 44)) || ((iTile >= 53) && (iTile <= 57)));
				break;
		}
	}

	/*** Make sure the disabled guards can't be used. ***/
	if (iEditPoP == 1)
	{
		if (((int)luLevelNr != 12) && ((iTile == 73) || (iTile == 74))) { return; }
		if (((int)luLevelNr != 6) && ((iTile == 75) || (iTile == 76))) { return; }
		if (((int)luLevelNr != 3) && ((iTile == 77) || (iTile == 78))) { return; }
		if (((int)luLevelNr != 13) && ((iTile == 108) || (iTile == 109)))
			{ return; }
		if (((int)luLevelNr == 12) || ((int)luLevelNr == 6)
			|| ((int)luLevelNr == 3) || ((int)luLevelNr == 13)
			|| ((int)luLevelNr == 14) || ((int)luLevelNr == 15))
		{
			if ((iTile >= 92) && (iTile <= 105)) { return; }
		}
	}

	if (iTile == 106) /*** kid, turned right ***/
	{
		if ((luKidRoom != iCurRoom) || (luKidPos != iLocation) ||
			(luKidDir != 0))
		{
			luKidRoom = iCurRoom;
			luKidPos = iLocation;
			luKidDir = 0;
			PlaySound ("wav/hum_adj.wav");
		}
	}
	if (iTile == 107) /*** kid, turned left ***/
	{
		if ((luKidRoom != iCurRoom) || (luKidPos != iLocation) ||
			(luKidDir != 255))
		{
			luKidRoom = iCurRoom;
			luKidPos = iLocation;
			luKidDir = 255;
			PlaySound ("wav/hum_adj.wav");
		}
	}

	if ((cCurType == 'd') || (cCurType == 'p'))
	{
		switch (iTile)
		{
			case 1: SetLocation (iRoom, iLocation, 0, 0, -1, -1, -1); break;
			case 2: SetLocation (iRoom, iLocation, 0, 1, -1, -1, -1); break;
			case 3: SetLocation (iRoom, iLocation, 0, 2, -1, -1, -1); break;
			case 4: SetLocation (iRoom, iLocation, 0, 3, -1, -1, -1); break;
			case 5: SetLocation (iRoom, iLocation, 0, 255, -1, -1, -1); break;
			case 6: SetLocation (iRoom, iLocation, 1, 0, -1, -1, -1); break;
			case 7: SetLocation (iRoom, iLocation, 1, 1, -1, -1, -1); break;
			case 8: SetLocation (iRoom, iLocation, 1, 2, -1, -1, -1); break;
			case 9: SetLocation (iRoom, iLocation, 1, 3, -1, -1, -1); break;
			case 10: SetLocation (iRoom, iLocation, 1, 255, -1, -1, -1); break;
			case 11: SetLocation (iRoom, iLocation, 2, 0, -1, -1, -1); break;
			case 12: SetLocation (iRoom, iLocation, 2, 1, -1, -1, -1); break;
			case 13: SetLocation (iRoom, iLocation, 2, 2, -1, -1, -1); break;
			case 14: SetLocation (iRoom, iLocation, 2, 3, -1, -1, -1); break;
			case 15: SetLocation (iRoom, iLocation, 2, 4, -1, -1, -1); break;
			case 16: SetLocation (iRoom, iLocation, 2, 5, -1, -1, -1); break;
			case 17: SetLocation (iRoom, iLocation, 2, 6, -1, -1, -1); break;
			case 18: SetLocation (iRoom, iLocation, 2, 7, -1, -1, -1); break;
			case 19: SetLocation (iRoom, iLocation, 2, 8, -1, -1, -1); break;
			case 20: SetLocation (iRoom, iLocation, 2, 9, -1, -1, -1); break;
			case 21: SetLocation (iRoom, iLocation, 3, 0, -1, -1, -1); break;
			case 22: SetLocation (iRoom, iLocation, 4, 0, -1, -1, -1); break;
			case 23: SetLocation (iRoom, iLocation, 4, 1, -1, -1, -1); break;
			case 24: SetLocation (iRoom, iLocation, 5, 0, -1, -1, -1); break;
			case 25: SetLocation (iRoom, iLocation, 6,
				iChangeEvent, -1, -1, -1); break;
			case 26: SetLocation (iRoom, iLocation, 7, 0, -1, -1, -1); break;
			case 27: SetLocation (iRoom, iLocation, 7, 1, -1, -1, -1); break;
			case 28: SetLocation (iRoom, iLocation, 7, 2, -1, -1, -1); break;
			case 29: SetLocation (iRoom, iLocation, 7, 3, -1, -1, -1); break;
			case 30: SetLocation (iRoom, iLocation, 8, 0, -1, -1, -1); break;
			case 31: SetLocation (iRoom, iLocation, 9, 0, -1, -1, -1); break;
			case 32: SetLocation (iRoom, iLocation, 10, 0, -1, -1, -1); break;
			case 33: SetLocation (iRoom, iLocation, 10, 1, -1, -1, -1); break;
			case 34: SetLocation (iRoom, iLocation, 10, 2, -1, -1, -1); break;
			case 35: SetLocation (iRoom, iLocation, 10, 3, -1, -1, -1); break;
			case 36: SetLocation (iRoom, iLocation, 10, 4, -1, -1, -1); break;
			case 37: SetLocation (iRoom, iLocation, 10, 5, -1, -1, -1); break;
			case 38: SetLocation (iRoom, iLocation, 10, 6, -1, -1, -1); break;
			case 39: SetLocation (iRoom, iLocation, 11, 0, -1, -1, -1); break;
			case 40: SetLocation (iRoom, iLocation, 12, 0, -1, -1, -1); break;
			case 41: SetLocation (iRoom, iLocation, 12, 1, -1, -1, -1); break;
			case 42: SetLocation (iRoom, iLocation, 12, 2, -1, -1, -1); break;
			case 43: SetLocation (iRoom, iLocation, 12, 3, -1, -1, -1); break;
			case 44: SetLocation (iRoom, iLocation, 12, 4, -1, -1, -1); break;
			case 45: SetLocation (iRoom, iLocation, 12, 5, -1, -1, -1); break;
			case 46: SetLocation (iRoom, iLocation, 12, 6, -1, -1, -1); break;
			case 47: SetLocation (iRoom, iLocation, 12, 7, -1, -1, -1); break;
			case 48: SetLocation (iRoom, iLocation, 13, 0, -1, -1, -1); break;
			case 49: SetLocation (iRoom, iLocation, 14, 0, -1, -1, -1); break;
			case 50: SetLocation (iRoom, iLocation, 15,
				iChangeEvent, -1, -1, -1); break;
			case 51: SetLocation (iRoom, iLocation, 16, 0, -1, -1, -1); break;
			case 52: SetLocation (iRoom, iLocation, 17, 0, -1, -1, -1); break;
			case 53: SetLocation (iRoom, iLocation, 18, 0, -1, -1, -1); break;
			case 54: SetLocation (iRoom, iLocation, 18, 1, -1, -1, -1); break;
			case 55: SetLocation (iRoom, iLocation, 18, 2, -1, -1, -1); break;
			case 56: SetLocation (iRoom, iLocation, 18, 3, -1, -1, -1); break;
			case 57: SetLocation (iRoom, iLocation, 18, 4, -1, -1, -1); break;
			case 58: SetLocation (iRoom, iLocation, 18, 5, -1, -1, -1); break;
			case 59: SetLocation (iRoom, iLocation, 19, 0, -1, -1, -1); break;
			case 60: SetLocation (iRoom, iLocation, 20, 0, -1, -1, -1); break;
			case 61: SetLocation (iRoom, iLocation, 20, 1, -1, -1, -1); break;
			case 62: SetLocation (iRoom, iLocation, 21, 0, -1, -1, -1); break;
			case 63: SetLocation (iRoom, iLocation, 22, 0, -1, -1, -1); break;
			case 64: SetLocation (iRoom, iLocation, 23, 0, -1, -1, -1); break;
			case 65: SetLocation (iRoom, iLocation, 24, 0, -1, -1, -1); break;
			case 66: SetLocation (iRoom, iLocation, 25, 0, -1, -1, -1); break;
			case 67: SetLocation (iRoom, iLocation, 26, 0, -1, -1, -1); break;
			case 68: SetLocation (iRoom, iLocation, 27, 0, -1, -1, -1); break;
			case 69: SetLocation (iRoom, iLocation, 28, 0, -1, -1, -1); break;
			case 70: SetLocation (iRoom, iLocation, 29, 0, -1, -1, -1); break;
			case 71: SetLocation (iRoom, iLocation, 30, 0, -1, -1, -1); break;
			case 72: SetLocation (iRoom, iLocation, 43, 0, -1, -1, -1); break;
			case 73: /*** shadow, turned right ***/
				if ((sGuardLocations[iCurRoom - 1] == iLocation - 1)
					&& (sGuardDirections[iCurRoom - 1] == 0))
					sGuardLocations[iCurRoom - 1] = 30; else
					SetGuard (iCurRoom, iLocation - 1, 0, iGuardSkill, -1); break;
			case 74: /*** shadow, turned left ***/
				if ((sGuardLocations[iCurRoom - 1] == iLocation - 1)
					&& (sGuardDirections[iCurRoom - 1] == 255))
					sGuardLocations[iCurRoom - 1] = 30; else
					SetGuard (iCurRoom, iLocation - 1, 255, iGuardSkill, -1); break;
			case 75: /*** fat, turned right ***/
				if ((sGuardLocations[iCurRoom - 1] == iLocation - 1)
					&& (sGuardDirections[iCurRoom - 1] == 0))
					sGuardLocations[iCurRoom - 1] = 30; else
					SetGuard (iCurRoom, iLocation - 1, 0, iGuardSkill, -1); break;
			case 76: /*** fat, turned left ***/
				if ((sGuardLocations[iCurRoom - 1] == iLocation - 1)
					&& (sGuardDirections[iCurRoom - 1] == 255))
					sGuardLocations[iCurRoom - 1] = 30; else
					SetGuard (iCurRoom, iLocation - 1, 255, iGuardSkill, -1); break;
			case 77: /*** skeleton, turned right ***/
				if ((sGuardLocations[iCurRoom - 1] == iLocation - 1)
					&& (sGuardDirections[iCurRoom - 1] == 0))
					sGuardLocations[iCurRoom - 1] = 30; else
					SetGuard (iCurRoom, iLocation - 1, 0, iGuardSkill, -1); break;
			case 78: /*** skeleton, turned left ***/
				if ((sGuardLocations[iCurRoom - 1] == iLocation - 1)
					&& (sGuardDirections[iCurRoom - 1] == 255))
					sGuardLocations[iCurRoom - 1] = 30; else
					SetGuard (iCurRoom, iLocation - 1, 255, iGuardSkill, -1); break;
			case 108: /*** jaffar, turned right ***/
				if ((sGuardLocations[iCurRoom - 1] == iLocation - 1)
					&& (sGuardDirections[iCurRoom - 1] == 0))
					sGuardLocations[iCurRoom - 1] = 30; else
					SetGuard (iCurRoom, iLocation - 1, 0, iGuardSkill, -1); break;
			case 109: /*** jaffar, turned left ***/
				if ((sGuardLocations[iCurRoom - 1] == iLocation - 1)
					&& (sGuardDirections[iCurRoom - 1] == 255))
					sGuardLocations[iCurRoom - 1] = 30; else
					SetGuard (iCurRoom, iLocation - 1, 255, iGuardSkill, -1); break;
			case 92: /*** normal guards ***/
			case 93:
			case 94:
			case 95:
			case 96:
			case 97:
			case 98:
			case 99:
			case 100:
			case 101:
			case 102:
			case 103:
			case 104:
			case 105:
				switch (iTile) /*** store color ***/
				{
					case 92: case 93: iTemp = 1; break;
					case 94: case 95: iTemp = 2; break;
					case 96: case 97: iTemp = 3; break;
					case 98: case 99: iTemp = 4; break;
					case 100: case 101: iTemp = 5; break;
					case 102: case 203: iTemp = 6; break;
					case 104: case 105: iTemp = 7; break;
				}
				switch (iTile)
				{
					case 92: case 94: case 96: case 98: case 100: case 102: case 104:
						if ((sGuardLocations[iCurRoom - 1] == iLocation - 1)
							&& (sGuardDirections[iCurRoom - 1] == 0) &&
							(sGuardColors[iCurRoom - 1] == iTemp))
							sGuardLocations[iCurRoom - 1] = 30; else
							SetGuard (iCurRoom, iLocation - 1, 0, iGuardSkill, iTemp); break;
					case 93: case 95: case 97: case 99: case 101: case 103: case 105:
						if ((sGuardLocations[iCurRoom - 1] == iLocation - 1)
							&& (sGuardDirections[iCurRoom - 1] == 255) &&
							(sGuardColors[iCurRoom - 1] == iTemp))
							sGuardLocations[iCurRoom - 1] = 30; else
							SetGuard (iCurRoom, iLocation - 1, 255, iGuardSkill, iTemp);
						break;
				}
				break;
		}
	}
	if (cCurType == 'o')
	{
		switch (iTile)
		{
			case 17: SetLocation (iRoom, iLocation, 0, 0, -1, -1, -1); break;
			case 18: SetLocation (iRoom, iLocation, 1, 0, -1, -1, -1); break;
			case 19: SetLocation (iRoom, iLocation, 20, 0, -1, -1, -1); break;
			case 23: SetLocation (iRoom, iLocation, 3, 0, -1, -1, -1); break;
			case 24: SetLocation (iRoom, iLocation, 37, 0, -1, -1, -1); break;
			case 25: SetLocation (iRoom, iLocation, 38, 0, -1, -1, -1); break;
			case 26: SetLocation (iRoom, iLocation, 39, 0, -1, -1, -1); break;
			case 29: SetLocation (iRoom, iLocation, 4,
				PercentAsMod (iChangePercent, 4), -1, -1, -1); break;
			case 30: SetLocation (iRoom, iLocation, 28, 0, -1, -1, -1); break;
			case 31: SetLocation (iRoom, iLocation, 29, 0, -1, -1, -1); break;
			case 32: SetLocation (iRoom, iLocation, 30, 0, -1, -1, -1); break;
			case 36: SetLocation (iRoom, iLocation, 10, 32, -1, -1, -1); break;
			case 37: SetLocation (iRoom, iLocation, 31, 0, -1, -1, -1); break;
			case 38: SetLocation (iRoom, iLocation, 40, 0, -1, -1, -1); break;
			case 39: SetLocation (iRoom, iLocation, 41, 0, -1, -1, -1); break;
			case 40: SetLocation (iRoom, iLocation, 2, 0, -1, -1, -1); break;
			case 41: SetLocation (iRoom, iLocation, 5,
				iChangeEvent, -1, -1, -1); break;
			case 42: SetLocation (iRoom, iLocation, 6,
				iChangeEvent, -1, -1, -1); break;
			case 43: SetLocation (iRoom, iLocation, 7, 0, -1, -1, -1); break;
			case 44: SetLocation (iRoom, iLocation, 7, 1, -1, -1, -1); break;
			case 45: SetLocation (iRoom, iLocation, 7, 2, -1, -1, -1); break;
			case 46: SetLocation (iRoom, iLocation, 7, 3, -1, -1, -1); break;
			case 47: SetLocation (iRoom, iLocation, 8, 0, -1, -1, -1); break;
			case 48: SetLocation (iRoom, iLocation, 9, 0, -1, -1, -1); break;
			case 49: SetLocation (iRoom, iLocation, 10, 0, -1, -1, -1); break;
			case 50: SetLocation (iRoom, iLocation, 10, 64, -1, -1, -1); break;
			case 51: SetLocation (iRoom, iLocation, 10, 96, -1, -1, -1); break;
			case 52: SetLocation (iRoom, iLocation, 10, 128, -1, -1, -1); break;
			case 53: SetLocation (iRoom, iLocation, 10, 160, -1, -1, -1); break;
			case 54: SetLocation (iRoom, iLocation, 10, 192, -1, -1, -1); break;
			case 55: SetLocation (iRoom, iLocation, 10, 224, -1, -1, -1); break;
			case 56: SetLocation (iRoom, iLocation, 11, JumpsAsMod
				(iChangeJumps) + FrameAsMod (iChangeFrame), -1, -1, -1); break;
			case 57: SetLocation (iRoom, iLocation, 12, FrameAsMod
				(iChangeFrame), -1, -1, -1); break;
			case 58: SetLocation (iRoom, iLocation, 13, 0, -1, -1, -1); break;
			case 59: SetLocation (iRoom, iLocation, 14, 0, -1, -1, -1); break;
			case 60: SetLocation (iRoom, iLocation, 15, 0, -1, -1, -1); break;
			case 61: SetLocation (iRoom, iLocation, 16, 0, -1, -1, -1); break;
			case 62: SetLocation (iRoom, iLocation, 17,
				PercentAsMod (iChangePercent, 17), -1, -1, -1); break;
			case 66: SetLocation (iRoom, iLocation, 18, 0, -1, -1, -1); break;
			case 67: SetLocation (iRoom, iLocation, 19, 0, -1, -1, -1); break;
			case 68: SetLocation (iRoom, iLocation, 21, 0, -1, -1, -1); break;
			case 69: SetLocation (iRoom, iLocation, 22, 0, -1, -1, -1); break;
			case 70: SetLocation (iRoom, iLocation, 22, 1, -1, -1, -1); break;
			case 71: SetLocation (iRoom, iLocation, 23, 0, -1, -1, -1); break;
			case 72: SetLocation (iRoom, iLocation, 24, 0, -1, -1, -1); break;
			case 73: SetLocation (iRoom, iLocation, 25, 0, -1, -1, -1); break;
			case 74: SetLocation (iRoom, iLocation, 26, 0, -1, -1, -1); break;
			case 75: SetLocation (iRoom, iLocation, 27, 0, -1, -1, -1); break;
			case 79: SetLocation (iRoom, iLocation, 32, 0, -1, -1, -1); break;
			case 80: SetLocation (iRoom, iLocation, 33, 0, -1, -1, -1); break;
			case 81: SetLocation (iRoom, iLocation, 34,
				iChangeEvent, -1, -1, -1); break;
			case 82: SetLocation (iRoom, iLocation, 35, 0, -1, -1, -1); break;
			case 83: SetLocation (iRoom, iLocation, 36, FrameAsMod
				(iChangeFrame), -1, -1, -1); break;
			case 87: SetLocation (iRoom, iLocation, 43, 0, -1, -1, -1); break;
			case 88: SetLocation (iRoom, iLocation, 44, 0, -1, -1, -1); break;
		}
	}
	if (cCurType == 'c')
	{
		switch (iTile)
		{
			case 1: SetLocation (iRoom, iLocation, 0, 0,
				ExtractedAsMod2 (' ', 's'), -1, -1); break;
			case 2: SetLocation (iRoom, iLocation, 0, 1,
				ExtractedAsMod2 (' ', 's'), -1, -1); break;
			case 3: SetLocation (iRoom, iLocation, 0, 2,
				ExtractedAsMod2 (' ', 's'), -1, -1); break;
			case 4: SetLocation (iRoom, iLocation, 0, 3,
				ExtractedAsMod2 (' ', 's'), -1, -1); break;
			case 5: SetLocation (iRoom, iLocation, 0, 4,
				ExtractedAsMod2 (' ', 's'), -1, -1); break;
			case 6: SetLocation (iRoom, iLocation, 1, 0,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 7: SetLocation (iRoom, iLocation, 1, 1,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 8: SetLocation (iRoom, iLocation, 1, 2,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 9: SetLocation (iRoom, iLocation, 2, 0,
				ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 10: SetLocation (iRoom, iLocation, 2, 5,
				ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 11: SetLocation (iRoom, iLocation, 3, 0,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 12: SetLocation (iRoom, iLocation, 4, 0,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 13: SetLocation (iRoom, iLocation, 4, PercentAsMod
				(iChangePercent, 4), ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 14: SetLocation (iRoom, iLocation, 4, 200,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 15: SetLocation (iRoom, iLocation, 8, 0,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 16: SetLocation (iRoom, iLocation, 8, 1,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 17: SetLocation (iRoom, iLocation, 8, 2,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 18: SetLocation (iRoom, iLocation, 9, 0,
				ExtractedAsMod2 (' ', 's'), -1, -1); break;
			case 19: SetLocation (iRoom, iLocation, 10, 0,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 20: SetLocation (iRoom, iLocation, 10, 32,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 21: SetLocation (iRoom, iLocation, 10, 64,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 22: SetLocation (iRoom, iLocation, 10, 96,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 23: SetLocation (iRoom, iLocation, 10, 128,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 24: SetLocation (iRoom, iLocation, 10, 160,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 25: SetLocation (iRoom, iLocation, 10, 192,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 26: SetLocation (iRoom, iLocation, 10, 224,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 27: SetLocation (iRoom, iLocation, 11, JumpsAsMod (iChangeJumps) +
				FrameAsMod (iChangeFrame), ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 28: SetLocation (iRoom, iLocation, 11, JumpsAsMod (iChangeJumps) +
				FrameAsMod (iChangeFrame) + 128,
				ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 29: SetLocation (iRoom, iLocation, 14, 0,
				ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 30: SetLocation (iRoom, iLocation, 14, 1,
				ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 31: SetLocation (iRoom, iLocation, 14, 2,
				ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 32: SetLocation (iRoom, iLocation, 16, 0,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 33: SetLocation (iRoom, iLocation, 17, 0,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 34: SetLocation (iRoom, iLocation, 17, PercentAsMod
				(iChangePercent, 17), ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 35: SetLocation (iRoom, iLocation, 17, 42,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 36: SetLocation (iRoom, iLocation, 18, 0,
				ExtractedAsMod2 (' ', ' '), -1, -1); break;
			case 37: SetLocation (iRoom, iLocation, 19, 0,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 38: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 0), ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 39: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 1), ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 40: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 2), ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 41: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 3), ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 42: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 4), ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 43: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 5), ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 44: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 6), ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 45: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 7), ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 46: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 8), ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 47: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 9), ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 48: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 10), ExtractedAsMod2 ('b', ' '), -1, -1); break;
			case 49: SetLocation (iRoom, iLocation, 23, 0,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 50: SetLocation (iRoom, iLocation, 24, 0,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 51: SetLocation (iRoom, iLocation, 28, 0,
				ExtractedAsMod2 (' ', ' '), -1, -1); break;
			case 52: SetLocation (iRoom, iLocation, 32, 0,
				ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 53: SetLocation (iRoom, iLocation, 33, 0,
				ExtractedAsMod2 (' ', 's'), -1, -1); break;
			case 54: SetLocation (iRoom, iLocation, 34, iChangeEvent,
				ExtractedAsMod2 ('b', ' ') + 0, -1, -1); break;
			case 55: SetLocation (iRoom, iLocation, 34, iChangeEvent,
				ExtractedAsMod2 ('b', ' ') + 8, -1, -1); break;
			case 56: SetLocation (iRoom, iLocation, 35, 0,
				ExtractedAsMod2 (' ', 's'), -1, -1); break;
			case 57: SetLocation (iRoom, iLocation, 36, FrameAsMod
				(iChangeFrame), ExtractedAsMod2 ('b', 's'), -1, -1); break;
			case 58: SetLocation (iRoom, iLocation, 44, 0,
				ExtractedAsMod2 (' ', ' '), -1, -1); break;
		}
	}
	if (cCurType == 'r')
	{
		switch (iTile)
		{
			case 1: SetLocation (iRoom, iLocation, 0, 0,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 2: SetLocation (iRoom, iLocation, 1, 0,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 3: SetLocation (iRoom, iLocation, 1, 1,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 4: SetLocation (iRoom, iLocation, 1, 2,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 5: SetLocation (iRoom, iLocation, 1, 3,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 6: SetLocation (iRoom, iLocation, 2, 0,
				ExtractedAsMod2 (' ', ' '), -1, -1); break;
			case 7: SetLocation (iRoom, iLocation, 3, 0,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 8: SetLocation (iRoom, iLocation, 3, 1,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 9: SetLocation (iRoom, iLocation, 3, 2,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 10: SetLocation (iRoom, iLocation, 4, 0,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 11: SetLocation (iRoom, iLocation, 4, PercentAsMod
				(iChangePercent, 4), ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 12: SetLocation (iRoom, iLocation, 4, 200,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 13: SetLocation (iRoom, iLocation, 5, iChangeEvent,
				ExtractedAsMod2 (' ', ' ') + 0, -1, -1); break;
			case 14: SetLocation (iRoom, iLocation, 5, iChangeEvent,
				ExtractedAsMod2 (' ', ' ') + 8, -1, -1); break;
			case 15: SetLocation (iRoom, iLocation, 6, iChangeEvent,
				ExtractedAsMod2 (' ', ' ') + 0, -1, -1); break;
			case 16: SetLocation (iRoom, iLocation, 6, iChangeEvent,
				ExtractedAsMod2 (' ', ' ') + 8, -1, -1); break;
			case 17: SetLocation (iRoom, iLocation, 7, 0,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 18: SetLocation (iRoom, iLocation, 7, 1,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 19: SetLocation (iRoom, iLocation, 7, 2,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 20: SetLocation (iRoom, iLocation, 7, 3,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 21: SetLocation (iRoom, iLocation, 8, 0,
				ExtractedAsMod2 ('c', ' '), -1, -1); break;
			case 22: SetLocation (iRoom, iLocation, 9, 0,
				ExtractedAsMod2 ('c', ' '), -1, -1); break;
			case 23: SetLocation (iRoom, iLocation, 10, 0,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 24: SetLocation (iRoom, iLocation, 10, 32,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 25: SetLocation (iRoom, iLocation, 10, 64,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 26: SetLocation (iRoom, iLocation, 10, 96,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 27: SetLocation (iRoom, iLocation, 10, 128,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 28: SetLocation (iRoom, iLocation, 10, 160,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 29: SetLocation (iRoom, iLocation, 10, 192,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 30: SetLocation (iRoom, iLocation, 10, 224,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 31: SetLocation (iRoom, iLocation, 11, JumpsAsMod
				(iChangeJumps) + FrameAsMod (iChangeFrame),
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 32: SetLocation (iRoom, iLocation, 12, FrameAsMod
				(iChangeFrame) + 0, ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 33: SetLocation (iRoom, iLocation, 12, FrameAsMod
				(iChangeFrame) + 32, ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 34: SetLocation (iRoom, iLocation, 12, FrameAsMod
				(iChangeFrame) + 128, ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 35: SetLocation (iRoom, iLocation, 12, FrameAsMod
				(iChangeFrame) + 160, ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 36: SetLocation (iRoom, iLocation, 13, 0,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 37: SetLocation (iRoom, iLocation, 13, 32,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 38: SetLocation (iRoom, iLocation, 14, 16,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 39: SetLocation (iRoom, iLocation, 14, 32,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 40: SetLocation (iRoom, iLocation, 14, 64,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 41: SetLocation (iRoom, iLocation, 15, 0,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 42: SetLocation (iRoom, iLocation, 15, 1,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 43: SetLocation (iRoom, iLocation, 15, 2,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 44: SetLocation (iRoom, iLocation, 15, 3,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 45: SetLocation (iRoom, iLocation, 16, 0,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 46: SetLocation (iRoom, iLocation, 17, 0,
				ExtractedAsMod2 (' ', 's'), -1, -1); break;
			case 47: SetLocation (iRoom, iLocation, 17, PercentAsMod
				(iChangePercent, 17), ExtractedAsMod2 (' ', 's'), -1, -1); break;
			case 48: SetLocation (iRoom, iLocation, 17, 42,
				ExtractedAsMod2 (' ', 's'), -1, -1); break;
			case 49: SetLocation (iRoom, iLocation, 18, 0,
				ExtractedAsMod2 (' ', ' '), -1, -1); break;
			case 50: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 0), ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 51: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 1), ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 52: SetLocation (iRoom, iLocation, 20, WallAsMod
				(iChangeWall, 2), ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 53: SetLocation (iRoom, iLocation, 21, 0,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 54: SetLocation (iRoom, iLocation, 22, 2,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
			case 55: SetLocation (iRoom, iLocation, 22, 1,
				ExtractedAsMod2 ('c', 's'), -1, -1); break;
		}
	}
	if (cCurType == 't')
	{
		switch (iTile)
		{
			case 1: SetLocation (iRoom, iLocation, 0, 0, 0, -1, -1); break;
			case 2: SetLocation (iRoom, iLocation, 1, 0, 0, -1, -1); break;
			case 3: SetLocation (iRoom, iLocation, 1, 1, 0, -1, -1); break;
			case 4: SetLocation (iRoom, iLocation, 2, 0, 0, -1, -1); break;
			case 5: SetLocation (iRoom, iLocation, 2, 5, 0, -1, -1); break;
			case 6: SetLocation (iRoom, iLocation, 3, 0, 0, -1, -1); break;
			case 7: SetLocation (iRoom, iLocation, 3, 1, 0, -1, -1); break;
			case 8: SetLocation (iRoom, iLocation, 4, 0, 0, -1, -1); break;
			case 9: SetLocation (iRoom, iLocation, 4,
				PercentAsMod (iChangePercent, 4), 0, -1, -1); break;
			case 10: SetLocation (iRoom, iLocation, 4, 200, 0, -1, -1); break;
			case 11: SetLocation (iRoom, iLocation, 5,
				iChangeEvent, 0, -1, -1); break;
			case 12: SetLocation (iRoom, iLocation, 5,
				iChangeEvent, 8, -1, -1); break;
			case 13: SetLocation (iRoom, iLocation, 6,
				iChangeEvent, 0, -1, -1); break;
			case 14: SetLocation (iRoom, iLocation, 6,
				iChangeEvent, 8, -1, -1); break;
			case 15: SetLocation (iRoom, iLocation, 8, 0, 0, -1, -1); break;
			case 16: SetLocation (iRoom, iLocation, 9, 0, 0, -1, -1); break;
			case 17: SetLocation (iRoom, iLocation, 10, 0, 0, -1, -1); break;
			case 18: SetLocation (iRoom, iLocation, 10, 32, 0, -1, -1); break;
			case 19: SetLocation (iRoom, iLocation, 10, 64, 0, -1, -1); break;
			case 20: SetLocation (iRoom, iLocation, 10, 96, 0, -1, -1); break;
			case 21: SetLocation (iRoom, iLocation, 10, 128, 0, -1, -1); break;
			case 22: SetLocation (iRoom, iLocation, 10, 160, 0, -1, -1); break;
			case 23: SetLocation (iRoom, iLocation, 10, 192, 0, -1, -1); break;
			case 24: SetLocation (iRoom, iLocation, 10, 224, 0, -1, -1); break;
			case 25: SetLocation (iRoom, iLocation, 11, JumpsAsMod
				(iChangeJumps) + FrameAsMod (iChangeFrame), 0, -1, -1); break;
			case 26: SetLocation (iRoom, iLocation, 12, FrameAsMod
				(iChangeFrame) + 0, 0, -1, -1); break;
			case 32: SetLocation (iRoom, iLocation, 12, FrameAsMod
				(iChangeFrame) + 32, 0, -1, -1); break;
			case 33: SetLocation (iRoom, iLocation, 12, FrameAsMod
				(iChangeFrame) + 128, 0, -1, -1); break;
			case 34: SetLocation (iRoom, iLocation, 12, FrameAsMod
				(iChangeFrame) + 160, 0, -1, -1); break;
			case 35: SetLocation (iRoom, iLocation, 13, 0, 0, -1, -1); break;
			case 36: SetLocation (iRoom, iLocation, 13, 32, 0, -1, -1); break;
			case 37: SetLocation (iRoom, iLocation, 14, 0, 0, -1, -1); break;
			case 38: SetLocation (iRoom, iLocation, 14, 1, 0, -1, -1); break;
			case 39: SetLocation (iRoom, iLocation, 16, 0, 0, -1, -1); break;
			case 45: SetLocation (iRoom, iLocation, 17, 0, 0, -1, -1); break;
			case 46: SetLocation (iRoom, iLocation, 17,
				PercentAsMod (iChangePercent, 17), 0, -1, -1); break;
			case 47: SetLocation (iRoom, iLocation, 17, 42, 0, -1, -1); break;
			case 48: SetLocation (iRoom, iLocation, 18, 0, 0, -1, -1); break;
			case 49: SetLocation (iRoom, iLocation, 19, 0, 0, -1, -1); break;
			case 50: SetLocation (iRoom, iLocation, 20,
				WallAsMod (iChangeWall, 0), 0, -1, -1); break;
			case 51: SetLocation (iRoom, iLocation, 25,
				WallAsMod (iChangeWall, 0), 0, -1, -1); break;
			case 52: SetLocation (iRoom, iLocation, 26, 0, 0, -1, -1); break;
			case 58: SetLocation (iRoom, iLocation, 32, 0, 0, -1, -1); break;
		}
	}
}
/*****************************************************************************/
void SetGuard (int iRoom, int iLoc, int iDirection, int iSkill, int iColor)
/*****************************************************************************/
{
	sGuardLocations[iCurRoom - 1] = iLoc;
	sGuardDirections[iCurRoom - 1] = iDirection;
	sGuardSkills[iCurRoom - 1] = iSkill;
	sGuardColors[iCurRoom - 1] = iColor;
	PlaySound ("wav/hum_adj.wav");
}
/*****************************************************************************/
void ShowChange (int iLocation)
/*****************************************************************************/
{
	int iDistort;

	switch (cCurType)
	{
		case 'd':
			ShowImage (-12, (int[]){0, 0, 0, 0}, screen, 31, 0, 0, 692, 455); break;
		case 'p':
			ShowImage (-12, (int[]){1, 0, 0, 0}, screen, 31, 0, 0, 692, 455); break;
		case 'o':
			ShowImage (-12, (int[]){20, 0, 0, 0}, screen, 31, 0, 0, 692, 455); break;
		case 'c':
			ShowImage (-12, (int[]){21, 0, 0, 0}, screen, 31, 0, 0, 692, 455); break;
		case 'r':
			ShowImage (-12, (int[]){22, 0, 0, 0}, screen, 31, 0, 0, 692, 455); break;
		case 't':
			ShowImage (-12, (int[]){24, 0, 0, 0}, screen, 31, 0, 0, 692, 455); break;
	}
	switch (iCloseOn)
	{
		case 0: /*** off ***/
			ShowImage (-12, (int[]){2, 0, 0, 0}, screen, 47, 0, 0, 36, 387); break;
		case 1: /*** on ***/
			ShowImage (-12, (int[]){3, 0, 0, 0}, screen, 47, 0, 0, 36, 387); break;
	}
	if ((cCurType == 'r') || (cCurType == 't'))
	{
		switch (iBackgroundOn)
		{
			case 0: /*** off ***/
				ShowImage (-12, (int[]){26, 0, 0, 0},
					screen, 96, 0, 0, 148, 62); break;
			case 1: /*** on ***/
				ShowImage (-12, (int[]){27, 0, 0, 0},
					screen, 96, 0, 0, 148, 62); break;
		}
	}
	if (iEditPoP == 2)
	{
		switch (iGuardsOn)
		{
			case 0: /*** off ***/
				ShowImage (-12, (int[]){28, 0, 0, 0}, screen, 97, 0, 0, 98, 62); break;
			case 1: /*** on ***/
				ShowImage (-12, (int[]){29, 0, 0, 0}, screen, 97, 0, 0, 98, 62); break;
		}

		switch (iMusicOn)
		{
			case 0: /*** off ***/
				ShowImage (-12, (int[]){39, 0, 0, 0},
					screen, 119, 0, 0, 14, 24); break;
			case 1: /*** on ***/
				ShowImage (-12, (int[]){40, 0, 0, 0},
					screen, 119, 0, 0, 14, 24); break;
		}
	}
	if (iEditPoP == 1)
	{
		DisableSome ((int)luLevelNr);
	} else {
		if (((luLevelNr == 7) || (luLevelNr == 21)) &&
			((iCurRoom != 32) || (iLocation != 25)))
		{ /*** prince warning ***/
			ShowImage (-12, (int[]){46, 0, 0, 0}, screen, 123, 0, 0, 52, 66);
		}
	}
	/*** old tile ***/
	ShowImage (-12, (int[]){11, 0, 0, 0}, screen, 82, 0, 0, 52, 66);
	switch (iCustomOn)
	{
		case 0:
			if (iIsCustom == 1)
				ShowImage (-12, (int[]){13, 0, 0, 0}, screen, 88, 0, 0, 36, 68);
			if (iIsCustom == 0)
				ShowImage (-12, (int[]){14, 0, 0, 0}, screen, 88, 0, 0, 36, 68);
			break;
		case 1:
			if (iIsCustom == 1)
				ShowImage (-12, (int[]){15, 0, 0, 0}, screen, 88, 0, 0, 36, 68);
			if (iIsCustom == 0)
				ShowImage (-12, (int[]){16, 0, 0, 0}, screen, 88, 0, 0, 36, 68);
			break;
	}
	if ((iCurRoom == (int)luKidRoom) && (iLocation == (int)luKidPos))
	{
		switch ((int)luKidDir)
		{
			case 0:
				ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 57, 0, 0, 27, 66); break;
			case 255:
				ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 58, 0, 0, 27, 66); break;
		}
	}
	if ((iEditPoP == 1) && (iLocation == sGuardLocations[iCurRoom - 1] + 1))
	{
		iGuardSkill = sGuardSkills[iCurRoom - 1];
		switch ((int)luLevelNr)
		{
			case 3: /*** skeleton ***/
				if (sGuardDirections[iCurRoom - 1] == 0)
					ShowImage (-12, (int[]){11, 0, 0, 0}, screen, 73, 0, 0, 52, 66);
				if (sGuardDirections[iCurRoom - 1] == 255)
					ShowImage (-12, (int[]){11, 0, 0, 0}, screen, 74, 0, 0, 52, 66);
				break;
			case 6: /*** fat ***/
				if (sGuardDirections[iCurRoom - 1] == 0)
					ShowImage (-12, (int[]){11, 0, 0, 0}, screen, 75, 0, 0, 52, 66);
				if (sGuardDirections[iCurRoom - 1] == 255)
					ShowImage (-12, (int[]){11, 0, 0, 0}, screen, 76, 0, 0, 52, 66);
				break;
			case 13: /*** jaffar ***/
				if (sGuardDirections[iCurRoom - 1] == 0)
					ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 77, 0, 0, 27, 66);
				if (sGuardDirections[iCurRoom - 1] == 255)
					ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 78, 0, 0, 27, 66);
				break;
			case 12: /*** shadow ***/
				if (sGuardDirections[iCurRoom - 1] == 0)
					ShowImage (-12, (int[]){11, 0, 0, 0}, screen, 79, 0, 0, 52, 66);
				if (sGuardDirections[iCurRoom - 1] == 255)
					ShowImage (-12, (int[]){11, 0, 0, 0}, screen, 80, 0, 0, 52, 66);
				break;
			default:
				switch (sGuardColors[iCurRoom - 1] - 1)
				{
					case 0:
						if (sGuardDirections[iCurRoom - 1] == 0)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 59, 0, 0, 27, 66);
						if (sGuardDirections[iCurRoom - 1] == 255)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 60, 0, 0, 27, 66);
						break;
					case 1:
						if (sGuardDirections[iCurRoom - 1] == 0)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 61, 0, 0, 27, 66);
						if (sGuardDirections[iCurRoom - 1] == 255)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 62, 0, 0, 27, 66);
						break;
					case 2:
						if (sGuardDirections[iCurRoom - 1] == 0)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 63, 0, 0, 27, 66);
						if (sGuardDirections[iCurRoom - 1] == 255)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 64, 0, 0, 27, 66);
						break;
					case 3:
						if (sGuardDirections[iCurRoom - 1] == 0)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 65, 0, 0, 27, 66);
						if (sGuardDirections[iCurRoom - 1] == 255)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 66, 0, 0, 27, 66);
						break;
					case 4:
						if (sGuardDirections[iCurRoom - 1] == 0)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 67, 0, 0, 27, 66);
						if (sGuardDirections[iCurRoom - 1] == 255)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 68, 0, 0, 27, 66);
						break;
					case 5:
						if (sGuardDirections[iCurRoom - 1] == 0)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 69, 0, 0, 27, 66);
						if (sGuardDirections[iCurRoom - 1] == 255)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 70, 0, 0, 27, 66);
						break;
					case 6:
					default:
						if (sGuardDirections[iCurRoom - 1] == 0)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 71, 0, 0, 27, 66);
						if (sGuardDirections[iCurRoom - 1] == 255)
							ShowImage (-12, (int[]){10, 0, 0, 0}, screen, 72, 0, 0, 27, 66);
						break;
				}
				break;
		}
	}
	/*** guard skill ***/
	if (iEditPoP == 1)
		{ ShowImage (-12, (int[]){12, 0, 0, 0}, screen, 81, 0, 0, 14, 14); }

	/*** bright green border ***/
	if (iOnTile != 0)
		{ ShowImage (-12, (int[]){4, 0, 0, 0}, screen, 48, 0, 0, 52, 66); }

	/*** show event number ***/
	if (iEditPoP == 1)
	{
		CenterNumber (screen, iChangeEvent + 1, 562, 424, color_bl, color_wh);
	} else {
		CenterNumber (screen, iChangeEvent + 1, 549, 424, color_bl, color_wh);
	}

	if (iEditPoP == 2)
	{
		/*** jumps ***/
		ShowImage (-12, (int[]){12, 0, 0, 0}, screen, 125, 0, 0, 14, 14);

		/*** frame ***/
		if (iChangeFrame != 0)
		{
			CenterNumber (screen, iChangeFrame, 576, 306, color_red, color_wh);
		} else {
			CenterNumber (screen, iChangeFrame, 576, 306, color_bl, color_wh);
		}

		/*** percent ***/
		CenterNumber (screen, iChangePercent, 549, 364, color_bl, color_wh);

		/*** alt wall ***/
		if (cCurType != 'o')
		{
			ShowImage (-12, (int[]){12, 0, 0, 0}, screen, 130, 0, 0, 14, 14);
		}

		if (cCurType == 'c')
		{
			/*** stal ***/
			ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 131, 0, 0, 14, 14);
			/*** bot ***/
			ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 132, 0, 0, 14, 14);
		}

		if (cCurType == 'r')
		{
			/*** sym ***/
			ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 133, 0, 0, 14, 14);
			/*** crk ***/
			ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 134, 0, 0, 14, 14);
		}
	}

	/*** distort ***/
	iDistort = 0;
	if (cCurType == 'd')
	{
		if ((iOnTile == 48) || (iOnTile == 64) || (iOnTile == 65) ||
			(iOnTile == 67) || (iOnTile == 68) || (iOnTile == 69) || (iOnTile == 70))
			{ iDistort = 1; }
	}
	if (cCurType == 'p')
	{
		if (iOnTile == 62) { iDistort = 1; }
	}
	if (cCurType == 'o')
	{
		if ((iOnTile == 25) || (iOnTile == 26) || (iOnTile == 31) ||
			(iOnTile == 32) || (iOnTile == 36) || (iOnTile == 38) ||
			(iOnTile == 39) || (iOnTile == 50) || (iOnTile == 51) ||
			(iOnTile == 52) || (iOnTile == 53) || (iOnTile == 54) ||
			(iOnTile == 55) || (iOnTile == 57) || (iOnTile == 58) ||
			(iOnTile == 60) || (iOnTile == 66) || (iOnTile == 87) ||
			(iOnTile == 88))
			{ iDistort = 1; }
	}
	if (cCurType == 'c')
	{
		if ((iOnTile == 36) || (iOnTile == 51) || (iOnTile == 58))
			{ iDistort = 1; }
	}
	if (iDistort == 1)
	{
		ShowImage (-12, (int[]){56, 0, 0, 0}, screen, 137, 0, 0, 648, 32);
	}

	/*** refresh screen ***/
	if (SDL_Flip (screen) == -1)
		{ printf ("[FAILED] SDL_Flip error!\n"); exit (EXIT_ERROR); }
}
/*****************************************************************************/
void ShowChangeCustom (int iLocation)
/*****************************************************************************/
{
	Uint32 color;
	SDL_Rect remove;

	ShowImage (-12, (int[]){17, 0, 0, 0}, screen, 31, 0, 0, 692, 455);

	switch (iCloseCustomOn)
	{
		case 0: /*** off ***/
			ShowImage (-12, (int[]){2, 0, 0, 0}, screen, 47, 0, 0, 36, 387); break; 
		case 1: /*** on ***/
			ShowImage (-12, (int[]){3, 0, 0, 0}, screen, 47, 0, 0, 36, 387); break;
	}
	switch (iCustomUseOn)
	{
		case 0: /*** off ***/
			ShowImage (-12, (int[]){18, 0, 0, 0}, screen, 88, 0, 0, 36, 68); break;
		case 1: /*** on ***/
			ShowImage (-12, (int[]){19, 0, 0, 0}, screen, 88, 0, 0, 36, 68); break;
	}

	/*** foreground random ***/
	ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 89, 0, 0, 14, 14);
	/*** foreground mod ***/
	ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 90, 0, 0, 14, 14);

	/*** foreground code and modifiers ***/
	CenterNumber (screen, iChangeFore, 177, 109, color_wh, color_bl);
	CenterNumber (screen, iChangeMod[1], 437, 109, color_wh, color_bl);
	if (iEditPoP == 2)
	{
		CenterNumber (screen, iChangeMod[2], 549, 22, color_wh, color_bl);
		CenterNumber (screen, iChangeMod[3], 549, 45, color_wh, color_bl);
		CenterNumber (screen, iChangeMod[4], 549, 68, color_wh, color_bl);

		/*** remove PoP1 suggestion ***/
		remove.x = 276; remove.y = 287; remove.w = 319; remove.h = 33;
		color = SDL_MapRGB (screen->format, 0x00, 0x00, 0x00);
		SDL_FillRect (screen, &remove, color);
	} else {
		/*** remove PoP2 modifiers ***/
		remove.x = 517; remove.y = 18; remove.w = 121; remove.h = 74;
		color = SDL_MapRGB (screen->format, 0x00, 0x00, 0x00);
		SDL_FillRect (screen, &remove, color);
	}

	/*** current custom ***/
	ShowCurrentCustom (screen);

	/*** refresh screen ***/
	if (SDL_Flip (screen) == -1)
		{ printf ("[FAILED] SDL_Flip error!\n"); exit (EXIT_ERROR); }
}
/*****************************************************************************/
void ShowChangeGuards (int iLocation)
/*****************************************************************************/
{
	ShowImage (-12, (int[]){30, 0, 0, 0}, screen, 31, 0, 0, 692, 455);

	switch (iCloseGuardsOn)
	{
		case 0: /*** off ***/
			ShowImage (-12, (int[]){31, 0, 0, 0}, screen, 47, 0, 0, 36, 455); break;
		case 1: /*** on ***/
			ShowImage (-12, (int[]){32, 0, 0, 0}, screen, 47, 0, 0, 36, 455); break;
	}

	/*****************/
	/* Static Guards */
	/*****************/

	/*** guards in room ***/
	ShowImage (-12, (int[]){12, 0, 0, 0}, screen, 98, 0, 0, 14, 14);
	/*** rows ***/
	ShowImage (-12, (int[]){33, 0, 0, 0}, screen, 100, 0, 0, 35, 8);
	ShowImage (-12, (int[]){33, 0, 0, 0}, screen, 101, 0, 0, 35, 8);
	ShowImage (-12, (int[]){33, 0, 0, 0}, screen, 102, 0, 0, 35, 8);
	ShowImage (-12, (int[]){33, 0, 0, 0}, screen, 103, 0, 0, 35, 8);
	ShowImage (-12, (int[]){33, 0, 0, 0}, screen, 104, 0, 0, 35, 8);
	/*** skills ***/
	if (iStaticGuards_5_Skills[iCurRoom - 1][0] > 11)
	{
		CenterNumber (screen, iStaticGuards_5_Skills[iCurRoom - 1][0],
			318, 56, color_red, color_bl);
	} else {
		CenterNumber (screen, iStaticGuards_5_Skills[iCurRoom - 1][0],
			318, 56, color_wh, color_bl);
	}
	if (iStaticGuards_5_Skills[iCurRoom - 1][1] > 11)
	{
		CenterNumber (screen, iStaticGuards_5_Skills[iCurRoom - 1][1],
			436, 56, color_red, color_bl);
	} else {
		CenterNumber (screen, iStaticGuards_5_Skills[iCurRoom - 1][1],
			436, 56, color_wh, color_bl);
	}
	if (iStaticGuards_5_Skills[iCurRoom - 1][2] > 11)
	{
		CenterNumber (screen, iStaticGuards_5_Skills[iCurRoom - 1][2],
			554, 56, color_red, color_bl);
	} else {
		CenterNumber (screen, iStaticGuards_5_Skills[iCurRoom - 1][2],
			554, 56, color_wh, color_bl);
	}
	if (iStaticGuards_5_Skills[iCurRoom - 1][3] > 11)
	{
		CenterNumber (screen, iStaticGuards_5_Skills[iCurRoom - 1][3],
			318, 197, color_red, color_bl);
	} else {
		CenterNumber (screen, iStaticGuards_5_Skills[iCurRoom - 1][3],
			318, 197, color_wh, color_bl);
	}
	if (iStaticGuards_5_Skills[iCurRoom - 1][4] > 11)
	{
		CenterNumber (screen, iStaticGuards_5_Skills[iCurRoom - 1][4],
			436, 197, color_red, color_bl);
	} else {
		CenterNumber (screen, iStaticGuards_5_Skills[iCurRoom - 1][4],
			436, 197, color_wh, color_bl);
	}
	/*** hit points, static ***/
	CenterNumber (screen, iStaticGuards_17_Hitpoints[iCurRoom - 1][0],
		318, 94, color_wh, color_bl);
	CenterNumber (screen, iStaticGuards_17_Hitpoints[iCurRoom - 1][1],
		436, 94, color_wh, color_bl);
	CenterNumber (screen, iStaticGuards_17_Hitpoints[iCurRoom - 1][2],
		554, 94, color_wh, color_bl);
	CenterNumber (screen, iStaticGuards_17_Hitpoints[iCurRoom - 1][3],
		318, 235, color_wh, color_bl);
	CenterNumber (screen, iStaticGuards_17_Hitpoints[iCurRoom - 1][4],
		436, 235, color_wh, color_bl);
	/*** X-coordinate ***/
	CenterNumber (screen, (int)IntsToHexToSSLittleEndian
		(iStaticGuards_3_Offsets2[iCurRoom - 1][0],
		iStaticGuards_2_Offsets1[iCurRoom - 1][0]),
		318, 132, color_wh, color_bl);
	CenterNumber (screen, (int)IntsToHexToSSLittleEndian
		(iStaticGuards_3_Offsets2[iCurRoom - 1][1],
		iStaticGuards_2_Offsets1[iCurRoom - 1][1]),
		436, 132, color_wh, color_bl);
	CenterNumber (screen, (int)IntsToHexToSSLittleEndian
		(iStaticGuards_3_Offsets2[iCurRoom - 1][2],
		iStaticGuards_2_Offsets1[iCurRoom - 1][2]),
		554, 132, color_wh, color_bl);
	CenterNumber (screen, (int)IntsToHexToSSLittleEndian
		(iStaticGuards_3_Offsets2[iCurRoom - 1][3],
		iStaticGuards_2_Offsets1[iCurRoom - 1][3]),
		318, 273, color_wh, color_bl);
	CenterNumber (screen, (int)IntsToHexToSSLittleEndian
		(iStaticGuards_3_Offsets2[iCurRoom - 1][4],
		iStaticGuards_2_Offsets1[iCurRoom - 1][4]),
		436, 273, color_wh, color_bl);
	/*** checkboxes ***/
	ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 105, 0, 0, 14, 14);
	ShowImage (-10, (int[]){1, 1, 0, 0}, screen, 105, 0, 0, 14, 14);
	ShowImage (-10, (int[]){1, 2, 0, 0}, screen, 105, 0, 0, 14, 14);
	ShowImage (-10, (int[]){1, 3, 0, 0}, screen, 105, 0, 0, 14, 14);
	ShowImage (-10, (int[]){1, 4, 0, 0}, screen, 105, 0, 0, 14, 14);

	/******************/
	/* Dynamic Guards */
	/******************/

	/*** incoming sets ***/
	ShowImage (-12, (int[]){12, 0, 0, 0}, screen, 99, 0, 0, 14, 14);
	/*** incoming skill ***/
	if ((iDynamicGuards_Skill[iCurRoom - 1] == 1) ||
		(iDynamicGuards_Skill[iCurRoom - 1] == 9) ||
		(iDynamicGuards_Skill[iCurRoom - 1] == 10) ||
		(iDynamicGuards_Skill[iCurRoom - 1] >= 12))
	{ /*** Unused in (the original) PoP2. ***/
		CenterNumber (screen, iDynamicGuards_Skill[iCurRoom - 1],
			559, 271, color_red, color_wh);
	} else {
		CenterNumber (screen, iDynamicGuards_Skill[iCurRoom - 1],
			559, 271, color_bl, color_wh);
	}
	/*** hit points, dynamic ***/
	CenterNumber (screen, iDynamicGuards_10_Hitpoints[iCurRoom - 1][0],
		398, 350, color_wh, color_bl);
	CenterNumber (screen, iDynamicGuards_10_Hitpoints[iCurRoom - 1][1],
		569, 350, color_wh, color_bl);
	/*** nr. of guards ***/
	CenterNumber (screen, iDynamicGuards_9_Amount[iCurRoom - 1][0],
		398, 372, color_wh, color_bl);
	CenterNumber (screen, iDynamicGuards_9_Amount[iCurRoom - 1][1],
		569, 372, color_wh, color_bl);
	/*** before spawn ***/
	CenterNumber (screen, iDynamicGuards_5_LatBefore[iCurRoom - 1][0],
		383, 394, color_wh, color_bl);
	CenterNumber (screen, iDynamicGuards_5_LatBefore[iCurRoom - 1][1],
		554, 394, color_wh, color_bl);
	/*** between spawns ***/
	CenterNumber (screen, iDynamicGuards_6_LatBetween[iCurRoom - 1][0],
		383, 416, color_wh, color_bl);
	CenterNumber (screen, iDynamicGuards_6_LatBetween[iCurRoom - 1][1],
		554, 416, color_wh, color_bl);
	/*** entry ***/
	ShowImage (-12, (int[]){34, 0, 0, 0}, screen, 106, 0, 0, 18, 8);
	ShowImage (-12, (int[]){34, 0, 0, 0}, screen, 107, 0, 0, 18, 8);
	/*** max alive ***/
	ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 108, 0, 0, 14, 14);
	ShowImage (-10, (int[]){1, 1, 0, 0}, screen, 108, 0, 0, 14, 14);
	/*** can jump ***/
	ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 109, 0, 0, 14, 14);
	ShowImage (-10, (int[]){1, 1, 0, 0}, screen, 109, 0, 0, 14, 14);
	/*** spawn if guard(s) alive ***/
	ShowImage (-10, (int[]){1, 0, 0, 0}, screen, 110, 0, 0, 14, 14);
	ShowImage (-10, (int[]){1, 1, 0, 0}, screen, 110, 0, 0, 14, 14);

	/************/
	/* Tooltips */
	/************/

	if (iGuardTooltip != 0)
	{
		ShowImage (-12, (int[]){38, iGuardTooltip - 1, 0, 0},
			screen, 115, 0, 0, 117, 99);
	}

	/************/
	/* Warnings */
	/************/

	/*** guard warning static ***/
	if ((luLevelNr == 1) || (luLevelNr == 15))
	{
		ShowImage (-12, (int[]){47, 0, 0, 0}, screen, 124, 0, 0, 203, 373);
	}
	if ((luLevelNr == 2) || (luLevelNr == 16))
	{
		ShowImage (-12, (int[]){48, 0, 0, 0}, screen, 124, 0, 0, 203, 373);
	}
	if (((luLevelNr >= 3) && (luLevelNr <= 5)) ||
		((luLevelNr >= 17) && (luLevelNr <= 19)))
	{
		ShowImage (-12, (int[]){49, 0, 0, 0}, screen, 124, 0, 0, 203, 373);
	}
	if (((luLevelNr >= 6) && (luLevelNr <= 9)) ||
		((luLevelNr >= 20) && (luLevelNr <= 23)))
	{
		ShowImage (-12, (int[]){50, 0, 0, 0}, screen, 124, 0, 0, 203, 373);
	}
	if (((luLevelNr >= 10) && (luLevelNr <= 13)) ||
		((luLevelNr >= 24) && (luLevelNr <= 27)))
	{
		ShowImage (-12, (int[]){51, 0, 0, 0}, screen, 124, 0, 0, 203, 373);
	}
	if ((luLevelNr == 14) || (luLevelNr == 28))
	{
		ShowImage (-12, (int[]){52, 0, 0, 0}, screen, 124, 0, 0, 203, 373);
	}
	/*** guard warning dynamic ***/
	if ((luLevelNr == 2) || (luLevelNr == 16) ||
		(luLevelNr == 14) || (luLevelNr == 28) ||
		((luLevelNr >= 6) && (luLevelNr <= 9)) ||
		((luLevelNr >= 20) && (luLevelNr <= 23)))
	{
		ShowImage (-12, (int[]){53, 0, 0, 0}, screen, 126, 0, 0, 347, 137);
	}

	/******************/
	/* Disable Guards */
	/******************/

	/*** disable static ***/
	if (iStaticGuards_Amount[iCurRoom - 1] < 1)
	{
		ShowImage (-12, (int[]){36, 0, 0, 0}, screen, 111, 0, 0, 43, 426);
		ShowImage (-12, (int[]){37, 0, 0, 0}, screen, 112, 0, 0, 119, 142);
	}
	if (iStaticGuards_Amount[iCurRoom - 1] < 2)
	{
		ShowImage (-12, (int[]){36, 1, 0, 0}, screen, 111, 0, 0, 43, 426);
		ShowImage (-12, (int[]){37, 1, 0, 0}, screen, 112, 0, 0, 119, 142);
	}
	if (iStaticGuards_Amount[iCurRoom - 1] < 3)
	{
		ShowImage (-12, (int[]){36, 2, 0, 0}, screen, 111, 0, 0, 43, 426);
		ShowImage (-12, (int[]){37, 2, 0, 0}, screen, 112, 0, 0, 119, 142);
	}
	if (iStaticGuards_Amount[iCurRoom - 1] < 4)
	{
		ShowImage (-12, (int[]){36, 3, 0, 0}, screen, 111, 0, 0, 43, 426);
		ShowImage (-12, (int[]){37, 3, 0, 0}, screen, 112, 0, 0, 119, 142);
	}
	if (iStaticGuards_Amount[iCurRoom - 1] < 5)
	{
		ShowImage (-12, (int[]){36, 4, 0, 0}, screen, 111, 0, 0, 43, 426);
		ShowImage (-12, (int[]){37, 4, 0, 0}, screen, 112, 0, 0, 119, 142);
	}

	/*** disable dynamic ***/
	if (iDynamicGuards_Sets[iCurRoom - 1] < 1)
		{ ShowImage (-12, (int[]){35, 0, 0, 0}, screen, 113, 0, 0, 172, 133); }
	if (iDynamicGuards_Sets[iCurRoom - 1] < 2)
		{ ShowImage (-12, (int[]){35, 0, 0, 0}, screen, 114, 0, 0, 172, 133); }

	/*** refresh screen ***/
	if (SDL_Flip (screen) == -1)
		{ printf ("[FAILED] SDL_Flip error!\n"); exit (EXIT_ERROR); }
}
/*****************************************************************************/
void ShowChangeMusic (int iLocation)
/*****************************************************************************/
{
	int iTile;

	ShowImage (-12, (int[]){41, 0, 0, 0}, screen, 31, 0, 0, 692, 455);

	for (iTile = 0; iTile < 30; iTile++)
	{
		if (sMusic[((iCurRoom - 1) * 30) + iTile] == 1)
		{
			ShowImage (-10, (int[]){1, iTile, 0, 0}, screen, 120, 0, 0, 14, 14);
		}
	}

	switch (iMAllOn)
	{
		case 0: /*** off ***/
			ShowImage (-17, (int[]){1, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
		case 1: /*** on ***/
			ShowImage (-17, (int[]){2, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
	}
	switch (iMNoneOn)
	{
		case 0: /*** off ***/
			ShowImage (-17, (int[]){3, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
		case 1: /*** on ***/
			ShowImage (-17, (int[]){4, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
	}
	switch (iMLeftOn)
	{
		case 0: /*** off ***/
			ShowImage (-17, (int[]){5, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
		case 1: /*** on ***/
			ShowImage (-17, (int[]){6, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
	}
	switch (iMRightOn)
	{
		case 0: /*** off ***/
			ShowImage (-17, (int[]){7, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
		case 1: /*** on ***/
			ShowImage (-17, (int[]){8, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
	}
	switch (iMTopOn)
	{
		case 0: /*** off ***/
			ShowImage (-17, (int[]){9, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
		case 1: /*** on ***/
			ShowImage (-17, (int[]){10, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
	}
	switch (iMMiddleOn)
	{
		case 0: /*** off ***/
			ShowImage (-17, (int[]){11, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
		case 1: /*** on ***/
			ShowImage (-17, (int[]){12, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
	}
	switch (iMBottomOn)
	{
		case 0: /*** off ***/
			ShowImage (-17, (int[]){13, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
		case 1: /*** on ***/
			ShowImage (-17, (int[]){14, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
	}
	switch (iMInvertOn)
	{
		case 0: /*** off ***/
			ShowImage (-17, (int[]){15, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
		case 1: /*** on ***/
			ShowImage (-17, (int[]){16, 0, 0, 0}, screen, 122, 0, 0, 71, 29); break;
	}

	switch (iCloseMusicOn)
	{
		case 0: /*** off ***/
			ShowImage (-12, (int[]){31, 0, 0, 0}, screen, 47, 0, 0, 36, 455); break;
		case 1: /*** on ***/
			ShowImage (-12, (int[]){32, 0, 0, 0}, screen, 47, 0, 0, 36, 455); break;
	}

	/*** refresh screen ***/
	if (SDL_Flip (screen) == -1)
		{ printf ("[FAILED] SDL_Flip error!\n"); exit (EXIT_ERROR); }
}
/*****************************************************************************/
void ShowChangeBackground (int iLocation)
/*****************************************************************************/
{
	if (cCurType == 'r')
		{ ShowImage (-12, (int[]){23, 0, 0, 0}, screen, 31, 0, 0, 692, 455); }
	if (cCurType == 't')
		{ ShowImage (-12, (int[]){25, 0, 0, 0}, screen, 31, 0, 0, 692, 455); }

	/*** sel back ***/
	ShowImage (-12, (int[]){54, 0,
		iModifierA[iCurRoom][iLocation - 1][3],
		iModifierA[iCurRoom][iLocation - 1][4]},
		screen, 129, 0, 0, 32, 63);

	/*** highlight back ***/
	ShowImage (-12, (int[]){55, 0, iMod3Hi, iMod4Hi},
		screen, 129, 0, 0, 32, 63);

	switch (iCloseBackgroundOn)
	{
		case 0: /*** off ***/
			ShowImage (-12, (int[]){31, 0, 0, 0}, screen, 47, 0, 0, 36, 455); break;
		case 1: /*** on ***/
			ShowImage (-12, (int[]){32, 0, 0, 0}, screen, 47, 0, 0, 36, 455); break;
	}

	/*** refresh screen ***/
	if (SDL_Flip (screen) == -1)
		{ printf ("[FAILED] SDL_Flip error!\n"); exit (EXIT_ERROR); }
}
/*****************************************************************************/
void DisableSome (int iCurLevel)
/*****************************************************************************/
{
	if (iCurLevel != 3) /*** disable skeleton ***/
	{
		ShowImage (-12, (int[]){5, 0, 0, 0}, screen, 52, 0, 0, 102, 66);
	}
	if (iCurLevel != 6) /*** disable fat guard ***/
	{
		ShowImage (-12, (int[]){6, 0, 0, 0}, screen, 53, 0, 0, 102, 66);
	}
	if (iCurLevel != 13) /*** disable jaffar ***/
	{
		ShowImage (-12, (int[]){7, 0, 0, 0}, screen, 54, 0, 0, 52, 66);
	}
	if (iCurLevel != 12) /*** disable shadow ***/
	{
		ShowImage (-12, (int[]){8, 0, 0, 0}, screen, 55, 0, 0, 102, 66);
	}
	if ((iCurLevel == 14) || (iCurLevel == 15) /*** disable guards ***/
		|| (iCurLevel == 3) || (iCurLevel == 6)
		|| (iCurLevel == 13) || (iCurLevel == 12))
	{
		ShowImage (-12, (int[]){9, 0, 0, 0}, screen, 56, 0, 0, 352, 66);
	}
}
/*****************************************************************************/
int OnTile (SDL_Event event)
/*****************************************************************************/
{
	int iTempX;
	int iTempY;
	int iTempOn;

	/*** Take care of the special rows for the rooftops, desert and final. ***/
	if (cCurType == 'o')
	{
		/*** 1.5 row ***/
		if (InArea (event, 154, 48, 154 + TILEWIDTH, 48 + TILEHEIGHT) == 1)
			{ return (17); }
		if (InArea (event, 204, 48, 204 + TILEWIDTH, 48 + TILEHEIGHT) == 1)
			{ return (18); }
		if (InArea (event, 254, 48, 254 + TILEWIDTH, 48 + TILEHEIGHT) == 1)
			{ return (19); }
		if (InArea (event, 454, 48, 454 + TILEWIDTH, 48 + TILEHEIGHT) == 1)
			{ return (23); }
		if (InArea (event, 504, 48, 504 + TILEWIDTH, 48 + TILEHEIGHT) == 1)
			{ return (24); }
		if (InArea (event, 554, 48, 554 + TILEWIDTH, 48 + TILEHEIGHT) == 1)
			{ return (25); }
		if (InArea (event, 604, 48, 604 + TILEWIDTH, 48 + TILEHEIGHT) == 1)
			{ return (26); }

		/*** 2.5 row ***/
		if (InArea (event, 104, 122, 104 + TILEWIDTH, 122 + TILEHEIGHT) == 1)
			{ return (29); }
		if (InArea (event, 154, 122, 154 + TILEWIDTH, 122 + TILEHEIGHT) == 1)
			{ return (30); }
		if (InArea (event, 204, 122, 204 + TILEWIDTH, 122 + TILEHEIGHT) == 1)
			{ return (31); }
		if (InArea (event, 254, 122, 254 + TILEWIDTH, 122 + TILEHEIGHT) == 1)
			{ return (32); }
		if (InArea (event, 454, 122, 454 + TILEWIDTH, 122 + TILEHEIGHT) == 1)
			{ return (36); }
		if (InArea (event, 504, 122, 504 + TILEWIDTH, 122 + TILEHEIGHT) == 1)
			{ return (37); }
		if (InArea (event, 554, 122, 554 + TILEWIDTH, 122 + TILEHEIGHT) == 1)
			{ return (38); }
		if (InArea (event, 604, 122, 604 + TILEWIDTH, 122 + TILEHEIGHT) == 1)
			{ return (39); }
	}

	/*** Regular large tiles. ***/
	for (iTempY = 0; iTempY < 7; iTempY++)
	{
		for (iTempX = 0; iTempX < 13; iTempX++)
		{
			if (InArea (event, 4 + (iTempX * (TILEWIDTH + 2)),
				4 + (iTempY * (TILEHEIGHT + 2)),
				2 + ((iTempX + 1) * (TILEWIDTH + 2)),
				2 + ((iTempY + 1) * (TILEHEIGHT + 2))) == 1)
			{
				iTempOn = (iTempY * 13) + iTempX + 1;
				switch (cCurType)
				{
					case 'd':
					case 'p':
						if ((iTempOn >= 1) && (iTempOn <= 78)) { return (iTempOn); }
						break;
					case 'o':
						if ((iTempOn >= 40) && (iTempOn <= 62)) { return (iTempOn); }
						if ((iTempOn >= 66) && (iTempOn <= 75)) { return (iTempOn); }
						if ((iTempOn >= 79) && (iTempOn <= 83)) { return (iTempOn); }
						if ((iTempOn >= 87) && (iTempOn <= 88)) { return (iTempOn); }
						break;
					case 'c':
						if ((iTempOn >= 1) && (iTempOn <= 58)) { return (iTempOn); }
						break;
					case 'r':
						if ((iTempOn >= 1) && (iTempOn <= 55)) { return (iTempOn); }
						break;
					case 't':
						if ((iTempOn >= 1) && (iTempOn <= 26)) { return (iTempOn); }
						if ((iTempOn >= 32) && (iTempOn <= 39)) { return (iTempOn); }
						if ((iTempOn >= 45) && (iTempOn <= 52)) { return (iTempOn); }
						if (iTempOn == 58) { return (iTempOn); }
						break;
				}
			}
		}
	}

	/*** Half sized tiles (humans). ***/
	if ((cCurType == 'd') || (cCurType == 'p'))
	{
		if (InArea (event, 4 + (25 * 0), 388, 2 + (25 * 1), 450) == 1)
			{ return (92); }
		if (InArea (event, 4 + (25 * 1), 388, 2 + (25 * 2), 450) == 1)
			{ return (93); }
		if (InArea (event, 4 + (25 * 2), 388, 2 + (25 * 3), 450) == 1)
			{ return (94); }
		if (InArea (event, 4 + (25 * 3), 388, 2 + (25 * 4), 450) == 1)
			{ return (95); }
		if (InArea (event, 4 + (25 * 4), 388, 2 + (25 * 5), 450) == 1)
			{ return (96); }
		if (InArea (event, 4 + (25 * 5), 388, 2 + (25 * 6), 450) == 1)
			{ return (97); }
		if (InArea (event, 4 + (25 * 6), 388, 2 + (25 * 7), 450) == 1)
			{ return (98); }
		if (InArea (event, 4 + (25 * 7), 388, 2 + (25 * 8), 450) == 1)
			{ return (99); }
		if (InArea (event, 4 + (25 * 8), 388, 2 + (25 * 9), 450) == 1)
			{ return (100); }
		if (InArea (event, 4 + (25 * 9), 388, 2 + (25 * 10), 450) == 1)
			{ return (101); }
		if (InArea (event, 4 + (25 * 10), 388, 2 + (25 * 11), 450) == 1)
			{ return (102); }
		if (InArea (event, 4 + (25 * 11), 388, 2 + (25 * 12), 450) == 1)
			{ return (103); }
		if (InArea (event, 4 + (25 * 12), 388, 2 + (25 * 13), 450) == 1)
			{ return (104); }
		if (InArea (event, 4 + (25 * 13), 388, 2 + (25 * 14), 450) == 1)
			{ return (105); }
	}
	/*** prince, turned right ***/
	if (InArea (event, 4 + (25 * 14), 388, 2 + (25 * 15), 450) == 1)
		{ return (106); }
	/*** prince, turned left ***/
	if (InArea (event, 4 + (25 * 15), 388, 2 + (25 * 16), 450) == 1)
		{ return (107); }
	if ((cCurType == 'd') || (cCurType == 'p'))
	{
		if (InArea (event, 4 + (25 * 16), 388, 2 + (25 * 17), 450) == 1)
			{ return (108); }
		if (InArea (event, 4 + (25 * 17), 388, 2 + (25 * 18), 450) == 1)
			{ return (109); }
	}

	return (0);
}
/*****************************************************************************/
int Modulo (long lOne, long lTwo, int iType)
/*****************************************************************************/
{
	/*** This (somewhat strange) function is no longer in use. ***/

	long lTemp;
	int iCount;

	iCount = 0;
	do {
		lTemp = lOne - lTwo;
		lOne = lTemp;
		iCount++;
	} while (lOne > 0);
	if (iType == 1)
	{
		if (lOne == 0)
		{
			return (lTwo); /*** The % modulo would return 0 here. ***/
		}
		else
		{
			return (lOne + lTwo);
		}
	} else {
		return (iCount);
	}
}
/*****************************************************************************/
void SetLocation (int iRoom, int iLocation, int iThing, int iModifier1,
	int iModifier2, int iModifier3, int iModifier4)
/*****************************************************************************/
{
	iThingA[iRoom][iLocation - 1] = iThing;
	iModifierA[iRoom][iLocation - 1][1] = iModifier1;
	if (iModifier2 != -1) /*** Don't mess up the oth backgrounds. ***/
		{ iModifierA[iRoom][iLocation - 1][2] = iModifier2; }
	if (iModifier3 != -1) /*** Don't mess up backgrounds. ***/
		{ iModifierA[iRoom][iLocation - 1][3] = iModifier3; }
	if (iModifier4 != -1) /*** Don't mess up backgrounds. ***/
		{ iModifierA[iRoom][iLocation - 1][4] = iModifier4; }

	iLastThing = iThing;
	iLastModifier[1] = iModifier1;
	iLastModifier[2] = iModifier2;
	iLastModifier[3] = iModifier3;
	iLastModifier[4] = iModifier4;
}
/*****************************************************************************/
void CallSave (int iJustXML)
/*****************************************************************************/
{
	char sFileName[MAX_FILE + 2];

	if (iJustXML != 1) { CreateBAK(); }
	if (iEditPoP == 1)
	{
		switch ((int)luLevelNr)
		{
			case 0: if (iJustXML != 1) { SavePLV (OUT_DIR "demo.plv"); }
				SaveXML (XML_DIR "demo.xml"); break;
			case 1: if (iJustXML != 1) { SavePLV (OUT_DIR "level1.plv"); }
				SaveXML (XML_DIR "level1.xml"); break;
			case 2: if (iJustXML != 1) { SavePLV (OUT_DIR "level2.plv"); }
				SaveXML (XML_DIR "level2.xml"); break;
			case 3: if (iJustXML != 1) { SavePLV (OUT_DIR "level3.plv"); }
				SaveXML (XML_DIR "level3.xml"); break;
			case 4: if (iJustXML != 1) { SavePLV (OUT_DIR "level4.plv"); }
				SaveXML (XML_DIR "level4.xml"); break;
			case 5: if (iJustXML != 1) { SavePLV (OUT_DIR "level5.plv"); }
				SaveXML (XML_DIR "level5.xml"); break;
			case 6: if (iJustXML != 1) { SavePLV (OUT_DIR "level6.plv"); }
				SaveXML (XML_DIR "level6.xml"); break;
			case 7: if (iJustXML != 1) { SavePLV (OUT_DIR "level7.plv"); }
				SaveXML (XML_DIR "level7.xml"); break;
			case 8: if (iJustXML != 1) { SavePLV (OUT_DIR "level8.plv"); }
				SaveXML (XML_DIR "level8.xml"); break;
			case 9: if (iJustXML != 1) { SavePLV (OUT_DIR "level9.plv"); }
				SaveXML (XML_DIR "level9.xml"); break;
			case 10: if (iJustXML != 1) { SavePLV (OUT_DIR "level10.plv"); }
				SaveXML (XML_DIR "level10.xml"); break;
			case 11: if (iJustXML != 1) { SavePLV (OUT_DIR "level11.plv"); }
				SaveXML (XML_DIR "level11.xml"); break;
			case 12: if (iJustXML != 1) { SavePLV (OUT_DIR "level12a.plv"); }
				SaveXML (XML_DIR "level12a.xml"); break;
			case 13: if (iJustXML != 1) { SavePLV (OUT_DIR "level12b.plv"); }
				SaveXML (XML_DIR "level12b.xml"); break;
			case 14: if (iJustXML != 1) { SavePLV (OUT_DIR "princess.plv"); }
				SaveXML (XML_DIR "princess.xml"); break;
			case 15: if (iJustXML != 1) { SavePLV (OUT_DIR "potions.plv"); }
				SaveXML (XML_DIR "potions.xml"); break;
		}
	} else {
		if ((int)luLevelNr < 15)
		{
			snprintf (sFileName, MAX_FILE, "%s____0%i.plv",
				OUT_DIR_POP2, 1999 + (int)luLevelNr);
		} else {
			snprintf (sFileName, MAX_FILE, "%s____0%i.plv",
				OUT_DIR_POP2, 2005 + (int)luLevelNr);
		}
		if (iJustXML != 1) { SavePLV (sFileName); }
	}
}
/*****************************************************************************/
int ChecksumOrWrite (int iFd)
/*****************************************************************************/
{
	long lSum;
	int iSC;
	int iTemp;
	int iTemp2;
	int iAct1, iAct2;
	int iGuardMod;
	char sToWrite[MAX_TOWRITE + 2];

	lSum = 0;

/*** PoP1 = 0; PoP2 = 0 ***/

	/*** Level Foreground ***/
	for (iTemp = 1; iTemp <= iRooms; iTemp++)
	{
		for (iTemp2 = 0; iTemp2 <= 29; iTemp2++)
		{
			snprintf (sToWrite, MAX_TOWRITE, "%c", iThingA[iTemp][iTemp2]);
			if (iFd == -1)
				{ lSum+=sToWrite[0]; }
					else { write (iFd, sToWrite, 1); }
		}
	}

/*** PoP1 = 720; PoP2 = 960 ***/

	/*** Level Modifier ***/
	for (iTemp = 1; iTemp <= iRooms; iTemp++)
	{
		for (iTemp2 = 0; iTemp2 <= 29; iTemp2++)
		{
			if (iEditPoP == 1)
			{
				snprintf (sToWrite, MAX_TOWRITE, "%c", iModifierA[iTemp][iTemp2][1]);
				if (iFd == -1)
					{ lSum+=sToWrite[0]; }
						else { write (iFd, sToWrite, 1); }
			} else {
				snprintf (sToWrite, MAX_TOWRITE, "%c%c%c%c",
					iModifierA[iTemp][iTemp2][1], iModifierA[iTemp][iTemp2][2],
					iModifierA[iTemp][iTemp2][3], iModifierA[iTemp][iTemp2][4]);
				if (iFd == -1)
					{ for (iSC = 0; iSC < 4; iSC++) { lSum+=sToWrite[iSC]; } }
						else { write (iFd, sToWrite, 4); }
			}
		}
	}

/*** PoP1 = 1440; PoP2 = 4800 ***/

	if (iEditPoP == 1)
	{
		if (iFd == -1)
			{ for (iSC = 0; iSC < 256; iSC++) { lSum+=sFirstDoorEvents[iSC]; } }
				else { WriteCharByChar (iFd, sFirstDoorEvents, 256); }
		if (iFd == -1)
			{ for (iSC = 0; iSC < 256; iSC++) { lSum+=sSecondDoorEvents[iSC]; } }
				else { WriteCharByChar (iFd, sSecondDoorEvents, 256); }
	} else {
		for (iTemp = 0; iTemp < 256; iTemp++)
		{
			snprintf (sToWrite, MAX_TOWRITE, "%c%c%c%c%c", iPoP2DoorRoom[iTemp],
				iPoP2DoorLocation[iTemp], iPoP2DoorType[iTemp],
				iPoP2DoorLeft[iTemp], iPoP2DoorRight[iTemp]);
			if (iFd == -1)
				{ for (iSC = 0; iSC < 5; iSC++) { lSum+=sToWrite[iSC]; } }
					else { write (iFd, sToWrite, 5); }
		}
	}

/*** PoP1 = 1952; PoP2 = 6080 ***/

	for (iTemp = 1; iTemp <= iRoomLinks; iTemp++)
	{
		/*** Were: Modulo (iTemp, 4, 2) and Modulo (iTemp, 4, 1) ***/
		snprintf (sToWrite, MAX_TOWRITE, "%c",
			iRoomConnections[((int)((iTemp - 1) / 4)) + 1][((iTemp - 1) % 4) + 1]);
		if (iFd == -1)
			{ lSum+=sToWrite[0]; }
				else { write (iFd, sToWrite, 1); }
	}

/*** PoP1 = 2048; PoP2 = 6208 ***/

	if (iEditPoP == 1)
	{
		if (iFd == -1)
			{ for (iSC = 0; iSC < 64; iSC++) { lSum+=sUnknownI[iSC]; } }
				else { WriteCharByChar (iFd, sUnknownI, 64); }
	} else {
		/*** Instead of saving sLastRoom; always use 32. ***/
		snprintf (sToWrite, MAX_TOWRITE, "%c", 32);
		if (iFd == -1)
			{ lSum+=sToWrite[0]; }
				else { write (iFd, sToWrite, 1); }

		if (iFd == -1)
			{ for (iSC = 0; iSC < 4; iSC++) { lSum+=sJEFF[iSC]; } }
				else { WriteCharByChar (iFd, sJEFF, 4); }
		if (iFd == -1)
			{ lSum+=sLevelType[0]; }
				else { write (iFd, sLevelType, 1); }
		if (iFd == -1)
			{ lSum+=sUnknownI[0]; }
				else { write (iFd, sUnknownI, 1); }

		/*** Instead of saving sLevelNumberEvents; fixes gameplay 2 problem. ***/
		if (iFd == -1)
			{ lSum+=sLevelNumber[0]; }
				else { write (iFd, sLevelNumber, 1); }

		/*** Instead of saving sExtraImgResources; enable more resources. ***/
		if ((luLevelNr == 1) || (luLevelNr == 15))
		{
			snprintf (sToWrite, MAX_TOWRITE, "%c%c%c%c%c%c%c%c%c%c%c%c"
				"%c%c%c%c%c%c%c%c%c%c%c%c", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0);
		}
		if ((luLevelNr == 2) || (luLevelNr == 16))
		{
			snprintf (sToWrite, MAX_TOWRITE, "%c%c%c%c%c%c%c%c%c%c%c%c"
				"%c%c%c%c%c%c%c%c%c%c%c%c", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		}
		if (((luLevelNr >= 3) && (luLevelNr <= 5)) ||
			((luLevelNr >= 17) && (luLevelNr <= 19)))
		{
			snprintf (sToWrite, MAX_TOWRITE, "%c%c%c%c%c%c%c%c%c%c%c%c"
				"%c%c%c%c%c%c%c%c%c%c%c%c", 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0);
		}
		if (((luLevelNr >= 6) && (luLevelNr <= 9)) ||
			((luLevelNr >= 20) && (luLevelNr <= 23)))
		{
			snprintf (sToWrite, MAX_TOWRITE, "%c%c%c%c%c%c%c%c%c%c%c%c"
				"%c%c%c%c%c%c%c%c%c%c%c%c", 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0);
		}
		if (((luLevelNr >= 10) && (luLevelNr <= 13)) ||
			((luLevelNr >= 24) && (luLevelNr <= 27)))
		{
			snprintf (sToWrite, MAX_TOWRITE, "%c%c%c%c%c%c%c%c%c%c%c%c"
				"%c%c%c%c%c%c%c%c%c%c%c%c", 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0,
				1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0);
		}
		if ((luLevelNr == 14) || (luLevelNr == 28))
		{
			snprintf (sToWrite, MAX_TOWRITE, "%c%c%c%c%c%c%c%c%c%c%c%c"
				"%c%c%c%c%c%c%c%c%c%c%c%c", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0);
		}
		if (iFd == -1)
			{ for (iSC = 0; iSC < 24; iSC++) { lSum+=sToWrite[iSC]; } }
				else { write (iFd, sToWrite, 24); }
	}

/*** PoP1 = 2112; PoP2 = 6240 ***/

	snprintf (sToWrite, MAX_TOWRITE, "%c", (int)luKidRoom);
	if (iFd == -1)
		{ lSum+=sToWrite[0]; }
			else { write (iFd, sToWrite, 1); }
	snprintf (sToWrite, MAX_TOWRITE, "%c", (int)luKidPos - 1);
	if (iFd == -1)
		{ lSum+=sToWrite[0]; }
			else { write (iFd, sToWrite, 1); }
	snprintf (sToWrite, MAX_TOWRITE, "%c", (int)luKidDir);
	/*** 3 of 4 ***/
	if (iEditPoP == 1)
	{
		if (((int)luLevelNr == 1) || ((int)luLevelNr == 13))
		{
			if (luKidDir == 0) { snprintf (sToWrite, MAX_TOWRITE, "%c", 255); }
				else { snprintf (sToWrite, MAX_TOWRITE, "%c", 0); }
		}
	} else {
		if ((luLevelNr == 2) || (luLevelNr == 16) ||
			(((luLevelNr == 1) || (luLevelNr == 15)) && (luKidRoom == 4)) ||
			(((luLevelNr == 6) || (luLevelNr == 20)) && (luKidRoom == 27)) ||
			(((luLevelNr == 10) || (luLevelNr == 24)) && (luKidRoom == 22)) ||
			(((luLevelNr == 14) || (luLevelNr == 28)) && (luKidRoom == 1)))
		{
			if (luKidDir == 0) { snprintf (sToWrite, MAX_TOWRITE, "%c", 255); }
				else { snprintf (sToWrite, MAX_TOWRITE, "%c", 0); }
		}
	}
	if (iFd == -1)
		{ lSum+=sToWrite[0]; }
			else { write (iFd, sToWrite, 1); }

/*** PoP1 = 2115; PoP2 = 6243 ***/

	if (iEditPoP == 1)
	{
		switch ((int)luLevelNr)
		{
			case 15: /*** potions level ***/
				if (iFd == -1)
					{ for (iSC = 0; iSC < 3; iSC++) { lSum+=sUnknownIIandIII[iSC]; } }
						else { WriteCharByChar (iFd, sUnknownIIandIII, 3); }
				break;
			default:
				if (iFd == -1)
					{ for (iSC = 0; iSC < 4; iSC++) { lSum+=sUnknownIIandIII[iSC]; } }
						else { WriteCharByChar (iFd, sUnknownIIandIII, 4); }
				break;
		}
		if (iFd == -1)
			{ for (iSC = 0; iSC < 24; iSC++) { lSum+=sGuardLocations[iSC]; } }
				else { WriteCharByChar (iFd, sGuardLocations, 24); }
		if (iFd == -1)
			{ for (iSC = 0; iSC < 24; iSC++) { lSum+=sGuardDirections[iSC]; } }
				else { WriteCharByChar (iFd, sGuardDirections, 24); }
		if (iFd == -1)
			{ for (iSC = 0; iSC < 48; iSC++) { lSum+=sUnknownIVaandIVb[iSC]; } }
				else { WriteCharByChar (iFd, sUnknownIVaandIVb, 48); }
		if (iFd == -1)
			{ for (iSC = 0; iSC < 24; iSC++) { lSum+=sGuardSkills[iSC]; } }
				else { WriteCharByChar (iFd, sGuardSkills, 24); }
		if (iFd == -1)
			{ for (iSC = 0; iSC < 24; iSC++) { lSum+=sUnknownIVc[iSC]; } }
				else { WriteCharByChar (iFd, sUnknownIVc, 24); }
		if (iFd == -1)
			{ for (iSC = 0; iSC < 24; iSC++) { lSum+=sGuardColors[iSC]; } }
				else { WriteCharByChar (iFd, sGuardColors, 24); }
		if (iFd == -1)
			{ for (iSC = 0; iSC < 16; iSC++) { lSum+=sUnknownIVd[iSC]; } }
				else { WriteCharByChar (iFd, sUnknownIVd, 16); }
	} else {
		if (iFd == -1)
			{ for (iSC = 0; iSC < 4; iSC++) { lSum+=sUnknownII[iSC]; } }
				else { WriteCharByChar (iFd, sUnknownII, 4); }
		for (iTemp = 0; iTemp < iRooms; iTemp++)
		{
			snprintf (sToWrite, MAX_TOWRITE, "%c", iStaticGuards_Amount[iTemp]);
			if (iFd == -1)
				{ lSum+=sToWrite[0]; }
					else { write (iFd, sToWrite, 1); }
			for (iTemp2 = 0; iTemp2 < 5; iTemp2++)
			{
				if (iStaticGuards_16_Type[iTemp][iTemp2] != 2)
				{ /*** not a skeleton ***/
					iAct1 = 0;
					iAct2 = 0;
				} else { /*** a skeleton ***/
					iAct1 = iStaticGuards_18_SkelAct1[iTemp][iTemp2];
					iAct2 = iStaticGuards_19_SkelAct2[iTemp][iTemp2];
				}
				switch (iStaticGuards_16_Type[iTemp][iTemp2])
				{
					case 0: iGuardMod = 77; break; /*** guard ***/
					case 1: iGuardMod = 105; break; /*** sword ***/
					case 2: /*** skeleton ***/
						if (((iAct1 == 0) && (iAct2 == 0)) ||
							((iAct1 == 1) && (iAct2 == 0)) ||
							((iAct1 == 255) && (iAct2 == 0)))
							{ iGuardMod = 112; }
								else { iGuardMod = 119; }
						break;
					case 3: iGuardMod = 2; break; /*** fake prince ***/
					case 5: iGuardMod = 130; break; /*** head, long hair ***/
					case 6: iGuardMod = 130; break; /*** head, short hair ***/
					case 7: iGuardMod = 77; break; /*** birdman ***/
					case 8: iGuardMod = 170; break; /*** snake ***/
					default: iGuardMod = 77; /*** Required, as a fallback. ***/
				}
				snprintf (sToWrite, MAX_TOWRITE,
					"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
					iStaticGuards_1_Locations[iTemp][iTemp2],
					iStaticGuards_2_Offsets1[iTemp][iTemp2],
					iStaticGuards_3_Offsets2[iTemp][iTemp2],
					iStaticGuards_4_Directions[iTemp][iTemp2],
					iStaticGuards_5_Skills[iTemp][iTemp2],
					iGuardMod,
					iStaticGuards_7_Unknown[iTemp][iTemp2],
					iStaticGuards_8_Unknown[iTemp][iTemp2],
					iStaticGuards_9_Unknown[iTemp][iTemp2],
					iStaticGuards_10_Colors[iTemp][iTemp2],
					iTemp2,
					iStaticGuards_12_Unknown[iTemp][iTemp2],
					iStaticGuards_13_Unknown[iTemp][iTemp2],
					iStaticGuards_14_Unknown[iTemp][iTemp2],
					iStaticGuards_15_Unknown[iTemp][iTemp2],
					iStaticGuards_16_Type[iTemp][iTemp2],
					iStaticGuards_17_Hitpoints[iTemp][iTemp2],
					iAct1,
					iAct2,
					iStaticGuards_20_Unknown[iTemp][iTemp2],
					iStaticGuards_21_Unknown[iTemp][iTemp2],
					iStaticGuards_22_Unknown[iTemp][iTemp2],
					iStaticGuards_23_Unknown[iTemp][iTemp2]);
				if (iFd == -1)
					{ for (iSC = 0; iSC < 23; iSC++) { lSum+=sToWrite[iSC]; } }
						else { write (iFd, sToWrite, 23); }
			}
		}
		snprintf (sToWrite, MAX_TOWRITE, "%c%c%c%c%c%c%c%c%c%c"
			"%c%c%c%c%c%c%c%c", iCheckPoints[0], iCheckPoints[1], iCheckPoints[2],
			iCheckPoints[3], iCheckPoints[4], iCheckPoints[5],
			iCheckPoints[6], iCheckPoints[7], iCheckPoints[8],
			iCheckPoints[9], iCheckPoints[10], iCheckPoints[11],
			iCheckPoints[12], iCheckPoints[13], iCheckPoints[14],
			iCheckPoints[15], iCheckPoints[16], iCheckPoints[17]);
		if (iFd == -1)
			{ for (iSC = 0; iSC < 18; iSC++) { lSum+=sToWrite[iSC]; } }
				else { write (iFd, sToWrite, 18); }
		for (iTemp = 0; iTemp < iRooms; iTemp++)
		{
			snprintf (sToWrite, MAX_TOWRITE, "%c", iDynamicGuards_Sets[iTemp]);
			if (iFd == -1)
				{ lSum+=sToWrite[0]; }
					else { write (iFd, sToWrite, 1); }
			snprintf (sToWrite, MAX_TOWRITE, "%c", iDynamicGuards_Skill[iTemp]);
			if (iFd == -1)
				{ lSum+=sToWrite[0]; }
					else { write (iFd, sToWrite, 1); }
			snprintf (sToWrite, MAX_TOWRITE, "%c", iDynamicGuards_Unknown1[iTemp]);
			if (iFd == -1)
				{ lSum+=sToWrite[0]; }
					else { write (iFd, sToWrite, 1); }
			snprintf (sToWrite, MAX_TOWRITE, "%c", iDynamicGuards_Unknown2[iTemp]);
			if (iFd == -1)
				{ lSum+=sToWrite[0]; }
					else { write (iFd, sToWrite, 1); }
			for (iTemp2 = 0; iTemp2 < 3; iTemp2++)
			{
				snprintf (sToWrite, MAX_TOWRITE,
					"%c%c%c%c%c%c%c%c%c%c",
					iTemp2,
					iDynamicGuards_2_MaxTog[iTemp][iTemp2],
					iDynamicGuards_3_Row[iTemp][iTemp2],
					iDynamicGuards_4_Column[iTemp][iTemp2],
					iDynamicGuards_5_LatBefore[iTemp][iTemp2],
					iDynamicGuards_6_LatBetween[iTemp][iTemp2],
					iDynamicGuards_7_CanJump[iTemp][iTemp2],
					iDynamicGuards_8_SpIfAl[iTemp][iTemp2],
					iDynamicGuards_9_Amount[iTemp][iTemp2],
					iDynamicGuards_10_Hitpoints[iTemp][iTemp2]);
				if (iFd == -1)
					{ for (iSC = 0; iSC < 10; iSC++) { lSum+=sToWrite[iSC]; } }
						else { write (iFd, sToWrite, 10); }
			}
		}
		if (iFd == -1)
			{ for (iSC = 0; iSC < 960; iSC++) { lSum+=sMusic[iSC]; } }
				else { WriteCharByChar (iFd, sMusic, 960); }
	}

/*** PoP1 = 2302/2303; PoP2 = 12025 ***/

	if (iEditPoP == 1)
	{
		if (iFd == -1)
			{ lSum = lSum + sEndCode[0] + sEndCode[1]; }
				else { WriteCharByChar (iFd, sEndCode, 2); }
	}

/*** PoP1 = 2304/2305; PoP2 = 12025 ***/

	/*** Was: 255 - Modulo (lSum, 256, 1) ***/
	/*** Now returns 255 instead of -1. ***/
	return (255 - (lSum % 256));
}
/*****************************************************************************/
void SavePLV (char *sFileName)
/*****************************************************************************/
{
	int iFd;
	int iTemp;
	unsigned int iHex;
	char sToWrite[MAX_TOWRITE + 2];
	char sToTemp[MAX_TOWRITE + 2];

	/*** Open file. ***/
	iFd = open (sFileName, O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, 0600);
	if (iFd == -1)
	{
		printf ("[FAILED] Could not create level file: %s!\n", strerror (errno));
		exit (EXIT_ERROR);
	}

	WriteCharByChar (iFd, sMagicID, 7);
	WriteCharByChar (iFd, sPOPVersion, 1);
	WriteCharByChar (iFd, sPLVVersion, 1);
	WriteCharByChar (iFd, sLevelNumber, 1);
	WriteCharByChar (iFd, sNumberOfFields, 4);
	WriteCharByChar (iFd, sLevelSize, 4);
	snprintf (sToWrite, MAX_TOWRITE, "%c", ChecksumOrWrite (-1));
	write (iFd, sToWrite, 1);

	ChecksumOrWrite (iFd);

	iHex = 0;
	snprintf (sToTemp, MAX_TOWRITE, "%08x", WriteUserData (-1, 2));
	for (iTemp = 0; iTemp <= 3; iTemp++)
	{
		snprintf (sToWrite, MAX_TOWRITE, "%c%c", sToTemp[6 - (2 * iTemp)],
			sToTemp[7 - (2 * iTemp)]);
		sscanf (sToWrite, "%x", &iHex);
		snprintf (sToWrite, MAX_TOWRITE, "%c", iHex);
		write (iFd, sToWrite, 1);
	}

	WriteUserData (iFd, 1);

	close (iFd);

	if (iEditPoP == 1)
	{
		if (system (PR_EXECUTABLE " -i -f --resource=" PR_RESOURCES
			" " LEVELS_DAT " > " DEVNULL) == -1)
		{
			printf ("[FAILED] Could not import the PoP1 levels: %s!\n",
				strerror (errno)); exit (EXIT_ERROR);
		}
	} else {
		if (system (PR_EXECUTABLE " -ilevels2 -f --resource=" PR_POP2
			" " PRINCE_DAT " > " DEVNULL) == -1)
		{
			printf ("[FAILED] Could not import the PoP2 levels: %s!\n",
				strerror (errno)); exit (EXIT_ERROR);
		}
	}

	PlaySound ("wav/save.wav");

	iChanged = 0;
}
/*****************************************************************************/
int WriteUserData (int iFd, int iType)
/*****************************************************************************/
{
	int iTemp;
	char sToWrite[MAX_TOWRITE + 2];
	char sDateTime[MAX_DATETIME + 2];
	int iChars;

	iChars = 0;
	for (iTemp = 1; iTemp < iInformationNr; iTemp++)
	{
		snprintf (sToWrite, MAX_TOWRITE, "%s", sInformation[iTemp][0]);
		if (iType == 1) { write (iFd, sToWrite, strlen (sToWrite)); }
			else { iChars += strlen (sToWrite); }
		snprintf (sToWrite, MAX_TOWRITE, "%c", 0x00);
		if (iType == 1) { write (iFd, sToWrite, 1); }
			else { iChars += 1; }
		if (strcmp (sInformation[iTemp][0], "Editor Name") == 0)
		{
			snprintf (sToWrite, MAX_TOWRITE, "%s", EDITOR_NAME);
		}
		else if (strcmp (sInformation[iTemp][0], "Level Author") == 0)
		{
			snprintf (sToWrite, MAX_TOWRITE, "%s", sAuthor);
		}
		else if (strcmp (sInformation[iTemp][0], "Editor Version") == 0)
		{
			snprintf (sToWrite, MAX_TOWRITE, "%s", EDITOR_VERSION);
		}
		else if (strcmp (sInformation[iTemp][0], "Time Last Modified") == 0)
		{
			DateTime (sDateTime);
			snprintf (sToWrite, MAX_TOWRITE, "%s", sDateTime);
		}
		else
		{
			snprintf (sToWrite, MAX_TOWRITE, "%s", sInformation[iTemp][1]);
		}
		if (iType == 1) { write (iFd, sToWrite, strlen (sToWrite)); }
			else { iChars += strlen (sToWrite); }
		snprintf (sToWrite, MAX_TOWRITE, "%c", 0x00);
		if (iType == 1) { write (iFd, sToWrite, 1); }
			else { iChars += 1; }
	}

	return (iChars);
}
/*****************************************************************************/
void WriteCharByChar (int iFd, unsigned char *sString, int iLength)
/*****************************************************************************/
{
	int iTemp;
	char sToWrite[MAX_TOWRITE + 2];

	for (iTemp = 0; iTemp < iLength; iTemp++)
	{
		snprintf (sToWrite, MAX_TOWRITE, "%c", sString[iTemp]);
		write (iFd, sToWrite, 1);
	}
}
/*****************************************************************************/
void ShowUsage (void)
/*****************************************************************************/
{
	printf ("%s %s\n%s\n\n", EDITOR_NAME, EDITOR_VERSION, COPYRIGHT);
	printf ("Usage:\n");
	printf ("  %s [OPTIONS]\n\nOptions:\n", EDITOR_NAME);
	printf ("  -h, -?,    --help           display this help and exit\n");
	printf ("  -v,        --version        output version information and"
		" exit\n");
	printf ("  -x,        --xml            export all PoP1 levels as XML and"
		" exit\n");
	printf ("  -d,        --debug          also show levels on the console\n");
	printf ("  -n,        --noaudio        do not play sound effects\n");
	printf ("  -q,        --quiet          do not play the chomp sound\n");
	printf ("  -i,        --improved       \"improved\" instead of"
		" \"megahit\"\n");
	printf ("  -m,        --makinit        \"makinit\" instead of"
		" \"yippeeyahoo\"\n");
	printf ("  -c='CODE', --cheat='CODE'   use CODE as the cheat code\n");
	printf ("  -a='NAME', --author='NAME'  use NAME as the author when"
		" saving\n");
	printf ("  -z,        --zoom           double the interface size\n");
	printf ("  -l=NR,     --level=NR       start in level NR\n");
	printf ("  -1,        --pop1           edit Prince of Persia 1 levels\n");
	printf ("  -2,        --pop2           edit Prince of Persia 2 levels\n");
	printf ("  -s,        --static         do not display animations\n\n");
	exit (EXIT_NORMAL);
}
/*****************************************************************************/
void PrIfDe (char *sString)
/*****************************************************************************/
{
	if (iDebug == 1) { printf ("%s", sString); }
}
/*****************************************************************************/
void MixAudio (void *unused, Uint8 *stream, int iLen)
/*****************************************************************************/
{
	int iTemp;
	Uint32 amount;

	for (iTemp = 0; iTemp < NUM_SOUNDS; iTemp++)
	{
		amount = (sounds[iTemp].dlen-sounds[iTemp].dpos);
		if (amount > iLen)
		{
			amount = iLen;
		}
		SDL_MixAudio (stream, &sounds[iTemp].data[sounds[iTemp].dpos], amount,
			SDL_MIX_MAXVOLUME);
		sounds[iTemp].dpos += amount;
	}
}
/*****************************************************************************/
void PlaySound (char *sFile)
/*****************************************************************************/
{
	int iIndex;
	SDL_AudioSpec wave;
	Uint8 *data;
	Uint32 dlen;
	SDL_AudioCVT cvt;

	if (iNoAudio == 1) { return; }
	for (iIndex = 0; iIndex < NUM_SOUNDS; iIndex++)
	{
		if (sounds[iIndex].dpos == sounds[iIndex].dlen)
		{
			break;
		}
	}
	if (iIndex == NUM_SOUNDS) { return; }

	if (SDL_LoadWAV (sFile, &wave, &data, &dlen) == NULL)
	{
		printf ("[FAILED] Could not load %s: %s!\n", sFile, SDL_GetError());
		exit (EXIT_ERROR);
	}
	SDL_BuildAudioCVT (&cvt, wave.format, wave.channels, wave.freq, AUDIO_S16, 2,
		22050);
	cvt.buf = (Uint8 *)malloc (dlen*cvt.len_mult);
	memcpy (cvt.buf, data, dlen);
	cvt.len = dlen;
	SDL_ConvertAudio (&cvt);
	SDL_FreeWAV (data);

	if (sounds[iIndex].data)
	{
		free(sounds[iIndex].data);
	}
	SDL_LockAudio();
	sounds[iIndex].data = cvt.buf;
	sounds[iIndex].dlen = cvt.len_cvt;
	sounds[iIndex].dpos = 0;
	SDL_UnlockAudio();
}
/*****************************************************************************/
int BrokenRoomLinks (int iRoomConnections[ROOMS + 2][4 + 2], int iPrint)
/*****************************************************************************/
{
	int iBroken;
	int iTemp;

	for (iTemp = 1; iTemp <= iRooms; iTemp++)
	{
		iDone[iTemp] = 0;
		iRoomConnectionsBroken[iTemp][1] = 0;
		iRoomConnectionsBroken[iTemp][2] = 0;
		iRoomConnectionsBroken[iTemp][3] = 0;
		iRoomConnectionsBroken[iTemp][4] = 0;
	}
	CheckSides ((int)luKidRoom, 0, 0);
	iBroken = 0;

	for (iTemp = 1; iTemp <= iRooms; iTemp++)
	{
		/*** If the room is in use... ***/
		if (iDone[iTemp] == 1)
		{
			/*** check left ***/
			if (iRoomConnections[iTemp][1] != 0)
			{
				if ((iRoomConnections[iTemp][1] == iTemp) ||
					(iRoomConnections[iRoomConnections[iTemp][1]][2] != iTemp))
				{
					iRoomConnectionsBroken[iTemp][1] = 1;
					iBroken = 1;
					if ((iDebug == 1) && (iPrint == 1))
					{
						printf ("[ INFO ] The left of room %i has a broken link.\n",
							iTemp);
					}
				}
			}
			/*** check right ***/
			if (iRoomConnections[iTemp][2] != 0)
			{
				if ((iRoomConnections[iTemp][2] == iTemp) ||
					(iRoomConnections[iRoomConnections[iTemp][2]][1] != iTemp))
				{
					iRoomConnectionsBroken[iTemp][2] = 1;
					iBroken = 1;
					if ((iDebug == 1) && (iPrint == 1))
					{
						printf ("[ INFO ] The right of room %i has a broken link.\n",
							iTemp);
					}
				}
			}
			/*** check up ***/
			if (iRoomConnections[iTemp][3] != 0)
			{
				if ((iRoomConnections[iTemp][3] == iTemp) ||
					(iRoomConnections[iRoomConnections[iTemp][3]][4] != iTemp))
				{
					iRoomConnectionsBroken[iTemp][3] = 1;
					iBroken = 1;
					if ((iDebug == 1) && (iPrint == 1))
					{
						printf ("[ INFO ] The top of room %i has a broken link.\n",
							iTemp);
					}
				}
			}
			/*** check down ***/
			if (iRoomConnections[iTemp][4] != 0)
			{
				if ((iRoomConnections[iTemp][4] == iTemp) ||
					(iRoomConnections[iRoomConnections[iTemp][4]][3] != iTemp))
				{
					iRoomConnectionsBroken[iTemp][4] = 1;
					iBroken = 1;
					if ((iDebug == 1) && (iPrint == 1))
					{
						printf ("[ INFO ] The bottom of room %i has a broken link.\n",
							iTemp);
					}
				}
			}
		}
	}

	return (iBroken);
}
/*****************************************************************************/
int BrokenRoomChange (int iRoom, int iSide)
/*****************************************************************************/
{
	char sOnAdj[MAX_ADJ + 2];

	if (iRoom < 10)
		{ snprintf (sOnAdj, MAX_ADJ, "20%i%i", iRoom, iSide); }
			else { snprintf (sOnAdj, MAX_ADJ, "2%i%i", iRoom, iSide); }

	return (atoi (sOnAdj));
}
/*****************************************************************************/
int MouseSelectAdj (SDL_Event event)
/*****************************************************************************/
{
	int iOnAdj;
	int iRoom;
	int iAdjBaseX;
	int iAdjBaseY;

	iOnAdj = 0;
	for (iRoom = 1; iRoom <= iRooms; iRoom++)
	{
		switch (iRoom)
		{
			case 1: iAdjBaseX = ADJ_BASE_X + (63 * 0);
				iAdjBaseY = ADJ_BASE_Y + (63 * 0); break;
			case 2: iAdjBaseX = ADJ_BASE_X + (63 * 1);
				iAdjBaseY = ADJ_BASE_Y + (63 * 0); break;
			case 3: iAdjBaseX = ADJ_BASE_X + (63 * 2);
				iAdjBaseY = ADJ_BASE_Y + (63 * 0); break;
			case 4: iAdjBaseX = ADJ_BASE_X + (63 * 3);
				iAdjBaseY = ADJ_BASE_Y + (63 * 0); break;
			case 5: iAdjBaseX = ADJ_BASE_X + (63 * 0);
				iAdjBaseY = ADJ_BASE_Y + (63 * 1); break;
			case 6: iAdjBaseX = ADJ_BASE_X + (63 * 1);
				iAdjBaseY = ADJ_BASE_Y + (63 * 1); break;
			case 7: iAdjBaseX = ADJ_BASE_X + (63 * 2);
				iAdjBaseY = ADJ_BASE_Y + (63 * 1); break;
			case 8: iAdjBaseX = ADJ_BASE_X + (63 * 3);
				iAdjBaseY = ADJ_BASE_Y + (63 * 1); break;
			case 9: iAdjBaseX = ADJ_BASE_X + (63 * 0);
				iAdjBaseY = ADJ_BASE_Y + (63 * 2); break;
			case 10: iAdjBaseX = ADJ_BASE_X + (63 * 1);
				iAdjBaseY = ADJ_BASE_Y + (63 * 2); break;
			case 11: iAdjBaseX = ADJ_BASE_X + (63 * 2);
				iAdjBaseY = ADJ_BASE_Y + (63 * 2); break;
			case 12: iAdjBaseX = ADJ_BASE_X + (63 * 3);
				iAdjBaseY = ADJ_BASE_Y + (63 * 2); break;
			case 13: iAdjBaseX = ADJ_BASE_X + (63 * 0);
				iAdjBaseY = ADJ_BASE_Y + (63 * 3); break;
			case 14: iAdjBaseX = ADJ_BASE_X + (63 * 1);
				iAdjBaseY = ADJ_BASE_Y + (63 * 3); break;
			case 15: iAdjBaseX = ADJ_BASE_X + (63 * 2);
				iAdjBaseY = ADJ_BASE_Y + (63 * 3); break;
			case 16: iAdjBaseX = ADJ_BASE_X + (63 * 3);
				iAdjBaseY = ADJ_BASE_Y + (63 * 3); break;
			case 17: iAdjBaseX = ADJ_BASE_X + (63 * 0);
				iAdjBaseY = ADJ_BASE_Y + (63 * 4); break;
			case 18: iAdjBaseX = ADJ_BASE_X + (63 * 1);
				iAdjBaseY = ADJ_BASE_Y + (63 * 4); break;
			case 19: iAdjBaseX = ADJ_BASE_X + (63 * 2);
				iAdjBaseY = ADJ_BASE_Y + (63 * 4); break;
			case 20: iAdjBaseX = ADJ_BASE_X + (63 * 3);
				iAdjBaseY = ADJ_BASE_Y + (63 * 4); break;
			case 21: iAdjBaseX = ADJ_BASE_X + (63 * 0);
				iAdjBaseY = ADJ_BASE_Y + (63 * 5); break;
			case 22: iAdjBaseX = ADJ_BASE_X + (63 * 1);
				iAdjBaseY = ADJ_BASE_Y + (63 * 5); break;
			case 23: iAdjBaseX = ADJ_BASE_X + (63 * 2);
				iAdjBaseY = ADJ_BASE_Y + (63 * 5); break;
			case 24: iAdjBaseX = ADJ_BASE_X + (63 * 3);
				iAdjBaseY = ADJ_BASE_Y + (63 * 5); break;
			case 25: iAdjBaseX = ADJ_BASE_X - (63 * 4);
				iAdjBaseY = ADJ_BASE_Y + (63 * 4); break;
			case 26: iAdjBaseX = ADJ_BASE_X - (63 * 3);
				iAdjBaseY = ADJ_BASE_Y + (63 * 4); break;
			case 27: iAdjBaseX = ADJ_BASE_X - (63 * 2);
				iAdjBaseY = ADJ_BASE_Y + (63 * 4); break;
			case 28: iAdjBaseX = ADJ_BASE_X - (63 * 1);
				iAdjBaseY = ADJ_BASE_Y + (63 * 4); break;
			case 29: iAdjBaseX = ADJ_BASE_X - (63 * 4);
				iAdjBaseY = ADJ_BASE_Y + (63 * 5); break;
			case 30: iAdjBaseX = ADJ_BASE_X - (63 * 3);
				iAdjBaseY = ADJ_BASE_Y + (63 * 5); break;
			case 31: iAdjBaseX = ADJ_BASE_X - (63 * 2);
				iAdjBaseY = ADJ_BASE_Y + (63 * 5); break;
			case 32: iAdjBaseX = ADJ_BASE_X - (63 * 1);
				iAdjBaseY = ADJ_BASE_Y + (63 * 5); break;
		}
		if (InArea (event, iAdjBaseX + 1, iAdjBaseY + 16,
			iAdjBaseX + 15, iAdjBaseY + 30) == 1)
		{
			iChangingBrokenRoom = iRoom;
			iChangingBrokenSide = 1; /*** left ***/
			iOnAdj = 1;
		}
		if (InArea (event, iAdjBaseX + 31, iAdjBaseY + 16,
			iAdjBaseX + 45, iAdjBaseY + 30) == 1)
		{
			iChangingBrokenRoom = iRoom;
			iChangingBrokenSide = 2; /*** right ***/
			iOnAdj = 1;
		}
		if (InArea (event, iAdjBaseX + 16, iAdjBaseY + 1,
			iAdjBaseX + 30, iAdjBaseY + 14) == 1)
		{
			iChangingBrokenRoom = iRoom;
			iChangingBrokenSide = 3; /*** up ***/
			iOnAdj = 1;
		}
		if (InArea (event, iAdjBaseX + 16, iAdjBaseY + 31,
			iAdjBaseX + 30, iAdjBaseY + 45) == 1)
		{
			iChangingBrokenRoom = iRoom;
			iChangingBrokenSide = 4; /*** down ***/
			iOnAdj = 1;
		}
	}

	return (iOnAdj);
}
/*****************************************************************************/
void CustomBlitSurface (SDL_Surface* src, char *srcn, SDL_Rect* srcrect,
	SDL_Surface* dst, SDL_Rect *dstrect, int arOptions[1])
/*****************************************************************************/
{
	SDL_Rect stuff;
	SDL_Surface *src_rota;

	stuff.x = dstrect->x * iScale;
	stuff.y = dstrect->y * iScale;
	if (arOptions[0] == 1)
	{
		src_rota = rotozoomSurface (src, 0, iScale, 0);
	} else {
		src_rota = src;
	}
	if (SDL_BlitSurface (src_rota, srcrect, dst, &stuff) != 0)
	{
		printf ("[ WARN ] SDL_BlitSurface (%s): %s\n", srcn, SDL_GetError());
	}
	SDL_FreeSurface (src_rota);
}
/*****************************************************************************/
void LoadFonts (void)
/*****************************************************************************/
{
	font1 = TTF_OpenFont ("ttf/lazy.ttf", FONT_SIZE_1 * iScale);
	if (font1 == NULL) { printf ("[FAILED] Font gone!\n"); exit (EXIT_ERROR); }
	font2 = TTF_OpenFont ("ttf/lazy.ttf", FONT_SIZE_2 * iScale);
	if (font2 == NULL) { printf ("[FAILED] Font gone!\n"); exit (EXIT_ERROR); }
	font3 = TTF_OpenFont ("ttf/lazy.ttf", FONT_SIZE_3 * iScale);
	if (font3 == NULL) { printf ("[FAILED] Font gone!\n"); exit (EXIT_ERROR); }
}
/*****************************************************************************/
void CustomTile (int iThing, int iModifier[], char *sRetString)
/*****************************************************************************/
{
	int iTemp;
	char cRandom;
	char cMod;

	iTemp = iThing;
	cRandom = '0';
	if (iTemp >= 192) { iTemp-=192; cRandom = '3'; }
	if (iTemp >= 128) { iTemp-=128; cRandom = '2'; }
	if (iTemp >= 64) { iTemp-=64; cRandom = '1'; }
	if (iTemp >= 32) { iTemp-=32; cMod = 'Y'; }
		else { cMod = 'N'; }
	snprintf (sRetString, MAX_CUSTOM, "%c%c%i/%i",
		cRandom, cMod, iTemp, iModifier[0]);
	if (iEditPoP == 2)
	{
		if ((iThing == 27) && (iModifier[0] == 0) && (cCurType == 'c'))
			{ snprintf (sRetString, MAX_CUSTOM, "%s", "(grate)"); }
		if ((iThing == 43) && (iModifier[0] == 0) && (cCurType == 't'))
			{ snprintf (sRetString, MAX_CUSTOM, "%s", "(flame l)"); }
		if ((iThing == 5) && (iModifier[0] == 183) && (cCurType == 't'))
			{ snprintf (sRetString, MAX_CUSTOM, "%s", "(flame r)"); }
	}
}
/*****************************************************************************/
void ShowCurrentCustom (SDL_Surface *screen)
/*****************************************************************************/
{
	int iTemp;

	iTemp = 0;
	switch (iChangeForeRand)
	{
		case 0: break;
		case 1: iTemp+=64; break;
		case 2: iTemp+=128; break;
		case 3: iTemp+=192; break;
	}
	if (iChangeForeMod == 1) { iTemp+=32; }
	iTemp+=iChangeFore;

	if ((cCurType == 'r') || (cCurType == 't'))
	{
		ShowBackground ((int[]){iChangeMod[1], iChangeMod[2],
			iChangeMod[3], iChangeMod[4]}, 100);
	}

	ShowImage (iTemp, (int[]){iChangeMod[1], iChangeMod[2],
		iChangeMod[3], iChangeMod[4]}, screen, 91, 0, 0, 120, 155);
}
/*****************************************************************************/
void CreateBAK (void)
/*****************************************************************************/
{
	FILE *fDAT;
	FILE *fBAK;
	int iData;

	if (iEditPoP == 1)
		{ fDAT = fopen (LEVELS_DAT, "rb"); }
			else { fDAT = fopen (PRINCE_DAT, "rb"); }
	if (fDAT == NULL)
		{ printf ("[FAILED] Could not open %s: %s!\n",
			LEVELS_DAT, strerror (errno)); }
	if (iEditPoP == 1)
		{ fBAK = fopen (LEVELS_BAK, "wb"); }
			else { fBAK = fopen (PRINCE_BAK, "wb"); }
	if (fBAK == NULL)
		{ printf ("[FAILED] Could not open %s: %s!\n",
			LEVELS_BAK, strerror (errno)); }

	while (1)
	{
		iData = fgetc (fDAT);
		if (iData == EOF) { break; }
			else { putc (iData, fBAK); }
	}

	fclose (fDAT);
	fclose (fBAK);
}
/*****************************************************************************/
void Help (int iAtScreen)
/*****************************************************************************/
{
	SDL_Event event;
	int iHelp;

	iHelp = 1;

	PlaySound ("wav/popup.wav");
	ShowHelp();
	while (iHelp == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
						case SDLK_o:
							iHelp = 0;
						default: break;
					}
					break;
				case SDL_MOUSEMOTION:
					SDL_GetMouseState (&iXPos, &iYPos);
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1)
					{
						if (InArea (event, 590, 405, 674, 436) == 1) /*** OK ***/
						{
							if (iHelpOK != 1) { iHelpOK = 1; }
							ShowHelp();
						}
					}
					break;
				case SDL_MOUSEBUTTONUP:
					iHelpOK = 0;
					if (event.button.button == 1) /*** left mouse button ***/
					{
						if (InArea (event, 590, 405, 674, 436) == 1)
							{ iHelp = 0; }
						if (InArea (event, 80, 55, 612, 78) == 1)
							{ OpenURL ("http://www.norbertdejonge.nl/apoplexy/"); }
						if (InArea (event, 38, 139, 654, 162) == 1)
							{ OpenURL ("http://forum.princed.org/viewforum.php?f=112"); }
						if (InArea (event, 52, 223, 640, 246) == 1)
							{ OpenURL ("http://www.youtube.com/watch?v=dtZiAb180ds"); }
						if (InArea (event, 45, 307, 647, 330) == 1)
							{ OpenURL ("http://forum.princed.org/viewforum.php?f=73"); }
					}
					ShowHelp(); break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
	PlaySound ("wav/popup_close.wav");
	ShowScreen (iAtScreen);
}
/*****************************************************************************/
void ShowHelp (void)
/*****************************************************************************/
{
	/*** help ***/
	ShowImage (-9, (int[]){10, 0, 0, 0}, screen, 31, 0, 0, 692, 455);
	switch (iHelpOK)
	{
		case 0:
			/*** OK off ***/
			ShowImage (-9, (int[]){3, 0, 0, 0}, screen, 43, 0, 0, 85, 32); break;
		case 1:
			/*** OK on ***/
			ShowImage (-9, (int[]){4, 0, 0, 0}, screen, 43, 0, 0, 85, 32); break;
	}

	/*** refresh screen ***/
	if (SDL_Flip (screen) == -1)
		{ printf ("[FAILED] SDL_Flip error!\n"); exit (EXIT_ERROR); }
}
/*****************************************************************************/
void Zoom (void)
/*****************************************************************************/
{
	if (iScale == 1) { iScale = 2; } else { iScale = 1; }
	screen = SDL_SetVideoMode ((SCREEN_WIDTH) * iScale,
		(SCREEN_HEIGHT) * iScale, SCREEN_BPP,
		SDL_DOUBLEBUF|SDL_SWSURFACE);
	TTF_CloseFont (font1);
	TTF_CloseFont (font2);
	TTF_CloseFont (font3);
	LoadFonts();
}
/*****************************************************************************/
void Sprinkle (void)
/*****************************************************************************/
{
	int iTemp;
	int iTemp2;
	int iRandom;

	for (iTemp2 = 1; iTemp2 <= iRooms; iTemp2++)
	{
		for (iTemp = 0; iTemp <= 29; iTemp++)
		{
			if (cCurType == 'd')
			{
				/*** empty? add wall pattern ***/
				if (((iThingA[iTemp2][iTemp] == 0) &&
					(iModifierA[iTemp2][iTemp][1] == 0)) ||
					((iThingA[iTemp2][iTemp] == 32) &&
					(iModifierA[iTemp2][iTemp][1] == 0)) ||
					((iThingA[iTemp2][iTemp] == 0) &&
					(iModifierA[iTemp2][iTemp][1] == 255)) ||
					((iThingA[iTemp2][iTemp] == 32) &&
					(iModifierA[iTemp2][iTemp][1] == 255)))
				{
					/*** 1-4 ***/
					iRandom = 1 + (int) (4.0 * RANDOM() / (RAND_MAX + 1.0));
					switch (iRandom)
					{
						case 1:
							iThingA[iTemp2][iTemp] = 0;
							iModifierA[iTemp2][iTemp][1] = 1;
							break;
						case 2:
							iThingA[iTemp2][iTemp] = 0;
							iModifierA[iTemp2][iTemp][1] = 2;
							break;
						default: break;
					}
				}
				/*** empty floor? add wall pattern, broken tile, flame, ***/
				/*** skeleton, flame + broken tile ***/
				if (((iThingA[iTemp2][iTemp] == 1) &&
				(iModifierA[iTemp2][iTemp][1] == 0)) ||
				((iThingA[iTemp2][iTemp] == 33) &&
				(iModifierA[iTemp2][iTemp][1] == 0)) ||
				((iThingA[iTemp2][iTemp] == 1) &&
				(iModifierA[iTemp2][iTemp][1] == 255)) ||
				((iThingA[iTemp2][iTemp] == 33) &&
				(iModifierA[iTemp2][iTemp][1] == 255)))
				{
					/*** 1-12 ***/
					iRandom = 1 + (int) (12.0 * RANDOM() / (RAND_MAX + 1.0));
					switch (iRandom)
					{
						case 1:
							iThingA[iTemp2][iTemp] = 1;
							iModifierA[iTemp2][iTemp][1] = 1;
							break;
						case 2:
							iThingA[iTemp2][iTemp] = 1;
							iModifierA[iTemp2][iTemp][1] = 2;
							break;
						case 3:
							iThingA[iTemp2][iTemp] = 14;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						case 4:
							iThingA[iTemp2][iTemp] = 19;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						case 5:
							iThingA[iTemp2][iTemp] = 21;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						case 6:
							iThingA[iTemp2][iTemp] = 30;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						default: break;
					}
				}
			}
			if (cCurType == 'p')
			{
				/*** simple wall pattern? add variant pattern ***/
				if (((iThingA[iTemp2][iTemp] == 0) &&
					(iModifierA[iTemp2][iTemp][1] == 1)) ||
					((iThingA[iTemp2][iTemp] == 32) &&
					(iModifierA[iTemp2][iTemp][1] == 1)))
				{
					/*** 1-2 ***/
					iRandom = 1 + (int) (2.0 * RANDOM() / (RAND_MAX + 1.0));
					if (iRandom == 1)
					{
						iThingA[iTemp2][iTemp] = 0;
						iModifierA[iTemp2][iTemp][1] = 2;
					}
				}
				/*** empty floor? add variant pattern, broken tile, flame ***/
				/*** flame + broken tile ***/
				if (((iThingA[iTemp2][iTemp] == 1) &&
				(iModifierA[iTemp2][iTemp][1] == 0)) ||
				((iThingA[iTemp2][iTemp] == 33) &&
				(iModifierA[iTemp2][iTemp][1] == 0)) ||
				((iThingA[iTemp2][iTemp] == 1) &&
				(iModifierA[iTemp2][iTemp][1] == 255)) ||
				((iThingA[iTemp2][iTemp] == 33) &&
				(iModifierA[iTemp2][iTemp][1] == 255)))
				{
					/*** 1-8 ***/
					iRandom = 1 + (int) (8.0 * RANDOM() / (RAND_MAX + 1.0));
					switch (iRandom)
					{
						case 1:
							iThingA[iTemp2][iTemp] = 1;
							iModifierA[iTemp2][iTemp][1] = 2;
							break;
						case 2:
							iThingA[iTemp2][iTemp] = 14;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						case 3:
							iThingA[iTemp2][iTemp] = 19;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						case 4:
							iThingA[iTemp2][iTemp] = 30;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						default: break;
					}
				}
			}
			if (cCurType == 'o')
			{
				/*** empty floor? broken tiles, flame, ***/
				/*** skeleton, flame + broken tiles ***/
				if ((iThingA[iTemp2][iTemp] == 1) &&
					(iModifierA[iTemp2][iTemp][1] == 0))
				{
					/*** 1-8 ***/
					iRandom = 1 + (int) (8.0 * RANDOM() / (RAND_MAX + 1.0));
					switch (iRandom)
					{
						case 1:
							iThingA[iTemp2][iTemp] = 14;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						case 2:
							iThingA[iTemp2][iTemp] = 19;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						case 3:
							iThingA[iTemp2][iTemp] = 21;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						case 4:
							iThingA[iTemp2][iTemp] = 32;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						default: break;
					}
				}
			}
			if (cCurType == 'c')
			{
				/*** empty floor? broken tiles, ***/
				/*** flame, flame + broken tiles ***/
				if (((iThingA[iTemp2][iTemp] == 1) &&
					(iModifierA[iTemp2][iTemp][1] == 0)) ||
					((iThingA[iTemp2][iTemp] == 1) &&
					(iModifierA[iTemp2][iTemp][1] == 1)) ||
					((iThingA[iTemp2][iTemp] == 1) &&
					(iModifierA[iTemp2][iTemp][1] == 2)))
				{
					/*** 1-10 ***/
					iRandom = 1 + (int) (10.0 * RANDOM() / (RAND_MAX + 1.0));
					switch (iRandom)
					{
						case 1:
							iThingA[iTemp2][iTemp] = 14;
							iModifierA[iTemp2][iTemp][1] = 0;
							iModifierA[iTemp2][iTemp][2] = 0; /*** no stalactite ***/
							break;
						case 2:
							iThingA[iTemp2][iTemp] = 14;
							iModifierA[iTemp2][iTemp][1] = 1;
							iModifierA[iTemp2][iTemp][2] = 0; /*** no stalactite ***/
							break;
						case 3:
							iThingA[iTemp2][iTemp] = 14;
							iModifierA[iTemp2][iTemp][1] = 2;
							iModifierA[iTemp2][iTemp][2] = 0; /*** no stalactite ***/
							break;
						case 4:
							iThingA[iTemp2][iTemp] = 19;
							iModifierA[iTemp2][iTemp][1] = 0;
							iModifierA[iTemp2][iTemp][2] = 0; /*** no stalactite ***/
							break;
						case 5:
							iThingA[iTemp2][iTemp] = 32;
							iModifierA[iTemp2][iTemp][1] = 0;
							iModifierA[iTemp2][iTemp][2] = 0; /*** no stalactite ***/
							break;
						default: break;
					}
				}
			}
			if (cCurType == 'r')
			{
				/*** empty floor? broken tiles, skeleton ***/
				if (((iThingA[iTemp2][iTemp] == 1) &&
				(iModifierA[iTemp2][iTemp][1] == 0)) ||
				((iThingA[iTemp2][iTemp] == 1) &&
				(iModifierA[iTemp2][iTemp][1] == 1)) ||
				((iThingA[iTemp2][iTemp] == 1) &&
				(iModifierA[iTemp2][iTemp][1] == 2)) ||
				((iThingA[iTemp2][iTemp] == 1) &&
				(iModifierA[iTemp2][iTemp][1] == 3)))
				{
					/*** 1-8 ***/
					iRandom = 1 + (int) (8.0 * RANDOM() / (RAND_MAX + 1.0));
					switch (iRandom)
					{
						case 1:
							iThingA[iTemp2][iTemp] = 14;
							iModifierA[iTemp2][iTemp][1] = 16;
							break;
						case 2:
							iThingA[iTemp2][iTemp] = 14;
							iModifierA[iTemp2][iTemp][1] = 32;
							break;
						case 3:
							iThingA[iTemp2][iTemp] = 14;
							iModifierA[iTemp2][iTemp][1] = 64;
							break;
						case 4:
							iThingA[iTemp2][iTemp] = 21;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						default: break;
					}
				}
			}
			if (cCurType == 't')
			{
				/*** empty floor? broken tiles, flame, flame + broken tiles ***/
				if ((iThingA[iTemp2][iTemp] == 1) &&
					(iModifierA[iTemp2][iTemp][1] == 0))
				{
					/*** 1-6 ***/
					iRandom = 1 + (int) (6.0 * RANDOM() / (RAND_MAX + 1.0));
					switch (iRandom)
					{
						case 1:
							iThingA[iTemp2][iTemp] = 14;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						case 2:
							iThingA[iTemp2][iTemp] = 19;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						case 3:
							iThingA[iTemp2][iTemp] = 32;
							iModifierA[iTemp2][iTemp][1] = 0;
							break;
						default: break;
					}
				}
			}
		}
	}
}
/*****************************************************************************/
void LoadLevel (int iLevel)
/*****************************************************************************/
{
	if (iEditPoP == 1)
	{
		switch (iLevel)
		{
			case 1: LoadPLV (IN_DIR "level1.plv"); cCurType = 'd'; break;
			case 2: LoadPLV (IN_DIR "level2.plv"); cCurType = 'd'; break;
			case 3: LoadPLV (IN_DIR "level3.plv"); cCurType = 'd'; break;
			case 4: LoadPLV (IN_DIR "level4.plv"); cCurType = 'p'; break;
			case 5: LoadPLV (IN_DIR "level5.plv"); cCurType = 'p'; break;
			case 6: LoadPLV (IN_DIR "level6.plv"); cCurType = 'p'; break;
			case 7: LoadPLV (IN_DIR "level7.plv"); cCurType = 'd'; break;
			case 8: LoadPLV (IN_DIR "level8.plv"); cCurType = 'd'; break;
			case 9: LoadPLV (IN_DIR "level9.plv"); cCurType = 'd'; break;
			case 10: LoadPLV (IN_DIR "level10.plv"); cCurType = 'p'; break;
			case 11: LoadPLV (IN_DIR "level11.plv"); cCurType = 'p'; break;
			case 12: LoadPLV (IN_DIR "level12a.plv"); cCurType = 'd'; break;
			case 13: LoadPLV (IN_DIR "level12b.plv"); cCurType = 'd'; break;
			case 14: LoadPLV (IN_DIR "princess.plv"); cCurType = 'p'; break;
			case 15: LoadPLV (IN_DIR "potions.plv"); cCurType = 'd'; break;
			case 0: LoadPLV (IN_DIR "demo.plv"); cCurType = 'd'; break;
		}
	} else {
		switch (iLevel)
		{
			case 1: LoadPLV (IN_DIR_POP2 "____02000.plv"); cCurType = 'o'; break;
			case 2: LoadPLV (IN_DIR_POP2 "____02001.plv"); cCurType = 'o'; break;
			case 3: LoadPLV (IN_DIR_POP2 "____02002.plv"); cCurType = 'c'; break;
			case 4: LoadPLV (IN_DIR_POP2 "____02003.plv"); cCurType = 'c'; break;
			case 5: LoadPLV (IN_DIR_POP2 "____02004.plv"); cCurType = 'c'; break;
			case 6: LoadPLV (IN_DIR_POP2 "____02005.plv"); cCurType = 'r'; break;
			case 7: LoadPLV (IN_DIR_POP2 "____02006.plv"); cCurType = 'r'; break;
			case 8: LoadPLV (IN_DIR_POP2 "____02007.plv"); cCurType = 'r'; break;
			case 9: LoadPLV (IN_DIR_POP2 "____02008.plv"); cCurType = 'r'; break;
			case 10: LoadPLV (IN_DIR_POP2 "____02009.plv"); cCurType = 't'; break;
			case 11: LoadPLV (IN_DIR_POP2 "____02010.plv"); cCurType = 't'; break;
			case 12: LoadPLV (IN_DIR_POP2 "____02011.plv"); cCurType = 't'; break;
			case 13: LoadPLV (IN_DIR_POP2 "____02012.plv"); cCurType = 't'; break;
			case 14: LoadPLV (IN_DIR_POP2 "____02013.plv"); cCurType = 'o'; break;
			case 15: LoadPLV (IN_DIR_POP2 "____02020.plv"); cCurType = 'o'; break;
			case 16: LoadPLV (IN_DIR_POP2 "____02021.plv"); cCurType = 'o'; break;
			case 17: LoadPLV (IN_DIR_POP2 "____02022.plv"); cCurType = 'c'; break;
			case 18: LoadPLV (IN_DIR_POP2 "____02023.plv"); cCurType = 'c'; break;
			case 19: LoadPLV (IN_DIR_POP2 "____02024.plv"); cCurType = 'c'; break;
			case 20: LoadPLV (IN_DIR_POP2 "____02025.plv"); cCurType = 'r'; break;
			case 21: LoadPLV (IN_DIR_POP2 "____02026.plv"); cCurType = 'r'; break;
			case 22: LoadPLV (IN_DIR_POP2 "____02027.plv"); cCurType = 'r'; break;
			case 23: LoadPLV (IN_DIR_POP2 "____02028.plv"); cCurType = 'r'; break;
			case 24: LoadPLV (IN_DIR_POP2 "____02029.plv"); cCurType = 't'; break;
			case 25: LoadPLV (IN_DIR_POP2 "____02030.plv"); cCurType = 't'; break;
			case 26: LoadPLV (IN_DIR_POP2 "____02031.plv"); cCurType = 't'; break;
			case 27: LoadPLV (IN_DIR_POP2 "____02032.plv"); cCurType = 't'; break;
			case 28: LoadPLV (IN_DIR_POP2 "____02033.plv"); cCurType = 'o'; break;
		}
	}

	if ((iEditPoP == 2) && ((iLevel == 1) || (iLevel == 2) ||
		(iLevel == 14) || (iLevel == 15) ||
		(iLevel == 16) || (iLevel == 28)))
	{
		iOnTile = 17;
	} else {
		iOnTile = 1;
	}
}
/*****************************************************************************/
void GetOptionValue (char *sArgv, char *sValue)
/*****************************************************************************/
{
	int iTemp;
	char sTemp[MAX_OPTION + 2];

	iTemp = strlen (sArgv) - 1;
	snprintf (sValue, MAX_OPTION, "%s", "");
	while (sArgv[iTemp] != '=')
	{
		snprintf (sTemp, MAX_OPTION, "%c%s", sArgv[iTemp], sValue);
		snprintf (sValue, MAX_OPTION, "%s", sTemp);
		iTemp--;
	}
}
/*****************************************************************************/
void FlipRoom (int iRoom, int iAxis)
/*****************************************************************************/
{
	int iThingTempA[30 + 2];
	int iModifierTempA[30 + 2][4 + 2];
	int iTemp;
	int iTileUse;
	int iKidPC;
	int iGuardPC;
	int iDynamic, iStatic;
	int iTempCoor;
	unsigned char sMusicTemp[30 + 2];

	/*** Storing data for later usage. ***/
	for (iTemp = 0; iTemp <= 29; iTemp++)
	{
		iThingTempA[iTemp] = iThingA[iRoom][iTemp];
		iModifierTempA[iTemp][1] = iModifierA[iRoom][iTemp][1];
		iModifierTempA[iTemp][2] = iModifierA[iRoom][iTemp][2];
		iModifierTempA[iTemp][3] = iModifierA[iRoom][iTemp][3];
		iModifierTempA[iTemp][4] = iModifierA[iRoom][iTemp][4];

		sMusicTemp[iTemp] = sMusic[((iRoom - 1) * 30) + iTemp];
	}

	if (iAxis == 1) /*** horizontal ***/
	{
		/*** tiles ***/
		for (iTemp = 0; iTemp <= 29; iTemp++)
		{
			if ((iTemp >= 0) && (iTemp <= 9)) { iTileUse = 9 - iTemp; }
			if ((iTemp >= 10) && (iTemp <= 19)) { iTileUse = 29 - iTemp; }
			if ((iTemp >= 20) && (iTemp <= 29)) { iTileUse = 49 - iTemp; }
			iThingA[iRoom][iTemp] = iThingTempA[iTileUse];
			iModifierA[iRoom][iTemp][1] = iModifierTempA[iTileUse][1];
			iModifierA[iRoom][iTemp][2] = iModifierTempA[iTileUse][2];
			iModifierA[iRoom][iTemp][3] = iModifierTempA[iTileUse][3];
			iModifierA[iRoom][iTemp][4] = iModifierTempA[iTileUse][4];

			sMusic[((iRoom - 1) * 30) + iTemp] = sMusicTemp[iTileUse];
		}

		/*** prince ***/
		if ((int)luKidRoom == iRoom)
		{
			/*** looking direction ***/
			if (luKidDir == 0) { luKidDir = 255; } else { luKidDir = 0; }
			/*** horizontal position ***/
			iKidPC = (int)luKidPos;
			if ((iKidPC >= 1) && (iKidPC <= 10)) { luKidPos = 11 - iKidPC; }
			if ((iKidPC >= 11) && (iKidPC <= 20)) { luKidPos = 31 - iKidPC; }
			if ((iKidPC >= 21) && (iKidPC <= 30)) { luKidPos = 51 - iKidPC; }
		}

		if (iEditPoP == 1)
		{
			/*** guard ***/
			if (sGuardLocations[iRoom - 1] != 30)
			{
				/*** looking direction ***/
				if (sGuardDirections[iRoom - 1] == 0)
					{ sGuardDirections[iRoom - 1] = 255; }
						else { sGuardDirections[iRoom - 1] = 0; }
				/*** horizontal position ***/
				iGuardPC = sGuardLocations[iRoom - 1];
				if ((iGuardPC >= 0) && (iGuardPC <= 9))
					{ sGuardLocations[iRoom - 1] = 9 - iGuardPC; }
				if ((iGuardPC >= 10) && (iGuardPC <= 19))
					{ sGuardLocations[iRoom - 1] = 29 - iGuardPC; }
				if ((iGuardPC >= 20) && (iGuardPC <= 29))
					{ sGuardLocations[iRoom - 1] = 49 - iGuardPC; }
			}
		} else {
			/*** static guards ***/
			for (iStatic = 0; iStatic < 5; iStatic++)
			{
				/*** looking direction ***/
				if (iStaticGuards_4_Directions[iRoom - 1][iStatic] == 0)
					{ iStaticGuards_4_Directions[iRoom - 1][iStatic] = 255; }
						else { iStaticGuards_4_Directions[iRoom - 1][iStatic] = 0; }
				/*** horizontal position ***/
				iTempCoor = IntsToHexToSSLittleEndian
					(iStaticGuards_3_Offsets2[iRoom - 1][iStatic],
					iStaticGuards_2_Offsets1[iRoom - 1][iStatic]);
				iTempCoor = 608 - iTempCoor;
				SSLittleEndianToHexToInts (iTempCoor,
					&iStaticGuards_3_Offsets2[iRoom - 1][iStatic],
					&iStaticGuards_2_Offsets1[iRoom - 1][iStatic]);
			}
			/*** dynamic guards ***/
			for (iDynamic = 0; iDynamic < 3; iDynamic++)
			{
				/*** horizontal position ***/
				iDynamicGuards_4_Column[iRoom - 1][iDynamic] =
					9 - iDynamicGuards_4_Column[iRoom - 1][iDynamic];
			}
		}
	} else { /*** vertical ***/
		/*** tiles ***/
		for (iTemp = 0; iTemp <= 29; iTemp++)
		{
			if ((iTemp >= 0) && (iTemp <= 9)) { iTileUse = iTemp + 20; }
			if ((iTemp >= 10) && (iTemp <= 19)) { iTileUse = iTemp; }
			if ((iTemp >= 20) && (iTemp <= 29)) { iTileUse = iTemp - 20; }
			iThingA[iRoom][iTemp] = iThingTempA[iTileUse];
			iModifierA[iRoom][iTemp][1] = iModifierTempA[iTileUse][1];
			iModifierA[iRoom][iTemp][2] = iModifierTempA[iTileUse][2];
			iModifierA[iRoom][iTemp][3] = iModifierTempA[iTileUse][3];
			iModifierA[iRoom][iTemp][4] = iModifierTempA[iTileUse][4];

			sMusic[((iRoom - 1) * 30) + iTemp] = sMusicTemp[iTileUse];
		}

		/*** prince ***/
		if ((int)luKidRoom == iRoom)
		{
			/*** vertical position ***/
			iKidPC = (int)luKidPos;
			if ((iKidPC >= 1) && (iKidPC <= 10)) { luKidPos = iKidPC + 20; }
			if ((iKidPC >= 21) && (iKidPC <= 30)) { luKidPos = iKidPC - 20; }
		}

		if (iEditPoP == 1)
		{
			/*** guard ***/
			if (sGuardLocations[iRoom - 1] != 30)
			{
				/*** vertical position ***/
				iGuardPC = sGuardLocations[iRoom - 1];
				if ((iGuardPC >= 0) && (iGuardPC <= 9))
					{ sGuardLocations[iRoom - 1] = iGuardPC + 20; }
				if ((iGuardPC >= 20) && (iGuardPC <= 29))
					{ sGuardLocations[iRoom - 1] = iGuardPC - 20; }
			}
		} else {
			/*** static guards ***/
			for (iStatic = 0; iStatic < 5; iStatic++)
			{
				/*** vertical position ***/
				iGuardPC = iStaticGuards_1_Locations[iRoom - 1][iStatic];
				if ((iGuardPC >= 0) && (iGuardPC <= 9))
					{ iStaticGuards_1_Locations[iRoom - 1][iStatic] = iGuardPC + 20; }
				if ((iGuardPC >= 20) && (iGuardPC <= 29))
					{ iStaticGuards_1_Locations[iRoom - 1][iStatic] = iGuardPC - 20; }
			}
			/*** dynamic guards ***/
			for (iDynamic = 0; iDynamic < 3; iDynamic++)
			{
				/*** vertical position ***/
				iDynamicGuards_3_Row[iRoom - 1][iDynamic] =
					2 - iDynamicGuards_3_Row[iRoom - 1][iDynamic];
			}
		}
	}
}
/*****************************************************************************/
void CopyPaste (int iRoom, int iAction)
/*****************************************************************************/
{
	int iTemp;
	int iStatic;
	int iDynamic;

	if (iAction == 1) /*** copy ***/
	{
		for (iTemp = 0; iTemp <= 29; iTemp++)
		{
			iThingACopyPaste[iTemp] = iThingA[iRoom][iTemp];
			iModifierACopyPaste[iTemp][1] = iModifierA[iRoom][iTemp][1];
			iModifierACopyPaste[iTemp][2] = iModifierA[iRoom][iTemp][2];
			iModifierACopyPaste[iTemp][3] = iModifierA[iRoom][iTemp][3];
			iModifierACopyPaste[iTemp][4] = iModifierA[iRoom][iTemp][4];
			if (iEditPoP == 2)
			{
				sMusicACopyPaste[iTemp] = sMusic[((iRoom - 1) * 30) + iTemp];
			}
		}
		if (iEditPoP == 1)
		{
			iGuardACopyPaste[0] = sGuardLocations[iRoom - 1];
			iGuardACopyPaste[1] = sGuardDirections[iRoom - 1];
			iGuardACopyPaste[2] = sGuardSkills[iRoom - 1];
			iGuardACopyPaste[3] = sGuardColors[iRoom - 1];
		} else {
			/*** static guards ***/
			iStaticGuardsACopyPaste[0] = iStaticGuards_Amount[iRoom - 1];
			for (iStatic = 0; iStatic < 5; iStatic++)
			{
				iStaticGuardsACopyPaste[(iStatic * 23) + 1] =
					iStaticGuards_1_Locations[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 2] =
					iStaticGuards_2_Offsets1[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 3] =
					iStaticGuards_3_Offsets2[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 4] =
					iStaticGuards_4_Directions[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 5] =
					iStaticGuards_5_Skills[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 6] =
					iStaticGuards_6_Mod[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 7] =
					iStaticGuards_7_Unknown[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 8] =
					iStaticGuards_8_Unknown[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 9] =
					iStaticGuards_9_Unknown[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 10] =
					iStaticGuards_10_Colors[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 11] =
					iStaticGuards_11_Number[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 12] =
					iStaticGuards_12_Unknown[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 13] =
					iStaticGuards_13_Unknown[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 14] =
					iStaticGuards_14_Unknown[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 15] =
					iStaticGuards_15_Unknown[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 16] =
					iStaticGuards_16_Type[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 17] =
					iStaticGuards_17_Hitpoints[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 18] =
					iStaticGuards_18_SkelAct1[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 19] =
					iStaticGuards_19_SkelAct2[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 20] =
					iStaticGuards_20_Unknown[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 21] =
					iStaticGuards_21_Unknown[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 22] =
					iStaticGuards_22_Unknown[iRoom - 1][iStatic];
				iStaticGuardsACopyPaste[(iStatic * 23) + 23] =
					iStaticGuards_23_Unknown[iRoom - 1][iStatic];
			}
			/*** dynamic guards ***/
			iDynamicGuardsACopyPaste[0] = iDynamicGuards_Sets[iRoom - 1];
			iDynamicGuardsACopyPaste[1] = iDynamicGuards_Skill[iRoom - 1];
			iDynamicGuardsACopyPaste[2] = iDynamicGuards_Unknown1[iRoom - 1];
			iDynamicGuardsACopyPaste[3] = iDynamicGuards_Unknown2[iRoom - 1];
			for (iDynamic = 0; iDynamic < 3; iDynamic++)
			{
				iDynamicGuardsACopyPaste[(iDynamic * 10) + 4] =
					iDynamicGuards_1_SetNr[iRoom - 1][iDynamic];
				iDynamicGuardsACopyPaste[(iDynamic * 10) + 5] =
					iDynamicGuards_2_MaxTog[iRoom - 1][iDynamic];
				iDynamicGuardsACopyPaste[(iDynamic * 10) + 6] =
					iDynamicGuards_3_Row[iRoom - 1][iDynamic];
				iDynamicGuardsACopyPaste[(iDynamic * 10) + 7] =
					iDynamicGuards_4_Column[iRoom - 1][iDynamic];
				iDynamicGuardsACopyPaste[(iDynamic * 10) + 8] =
					iDynamicGuards_5_LatBefore[iRoom - 1][iDynamic];
				iDynamicGuardsACopyPaste[(iDynamic * 10) + 9] =
					iDynamicGuards_6_LatBetween[iRoom - 1][iDynamic];
				iDynamicGuardsACopyPaste[(iDynamic * 10) + 10] =
					iDynamicGuards_7_CanJump[iRoom - 1][iDynamic];
				iDynamicGuardsACopyPaste[(iDynamic * 10) + 11] =
					iDynamicGuards_8_SpIfAl[iRoom - 1][iDynamic];
				iDynamicGuardsACopyPaste[(iDynamic * 10) + 12] =
					iDynamicGuards_9_Amount[iRoom - 1][iDynamic];
				iDynamicGuardsACopyPaste[(iDynamic * 10) + 13] =
					iDynamicGuards_10_Hitpoints[iRoom - 1][iDynamic];
			}
		}
		iCopied = 1;
	} else { /*** paste ***/
		for (iTemp = 0; iTemp <= 29; iTemp++)
		{
			if (iCopied == 1)
			{
				iThingA[iRoom][iTemp] = iThingACopyPaste[iTemp];
				iModifierA[iRoom][iTemp][1] = iModifierACopyPaste[iTemp][1];
				iModifierA[iRoom][iTemp][2] = iModifierACopyPaste[iTemp][2];
				iModifierA[iRoom][iTemp][3] = iModifierACopyPaste[iTemp][3];
				iModifierA[iRoom][iTemp][4] = iModifierACopyPaste[iTemp][4];
				if (iEditPoP == 2)
				{
					sMusic[((iRoom - 1) * 30) + iTemp] = sMusicACopyPaste[iTemp];
				}
			} else {
				iThingA[iRoom][iTemp] = 0;
				iModifierA[iRoom][iTemp][1] = 0;
				iModifierA[iRoom][iTemp][2] = 0;
				iModifierA[iRoom][iTemp][3] = 0;
				iModifierA[iRoom][iTemp][4] = 0;
				if (iEditPoP == 2)
				{
					sMusic[((iRoom - 1) * 30) + iTemp] = 0;
				}
			}
		}
		if (iCopied == 1)
		{
			if (iEditPoP == 1)
			{
				sGuardLocations[iRoom - 1] = iGuardACopyPaste[0];
				sGuardDirections[iRoom - 1] = iGuardACopyPaste[1];
				sGuardSkills[iRoom - 1] = iGuardACopyPaste[2];
				sGuardColors[iRoom - 1] = iGuardACopyPaste[3];
			} else {
				/*** static guards ***/
				iStaticGuards_Amount[iRoom - 1] = iStaticGuardsACopyPaste[0];
				for (iStatic = 0; iStatic < 5; iStatic++)
				{
					iStaticGuards_1_Locations[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 1];
					iStaticGuards_2_Offsets1[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 2];
					iStaticGuards_3_Offsets2[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 3];
					iStaticGuards_4_Directions[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 4];
					iStaticGuards_5_Skills[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 5];
					iStaticGuards_6_Mod[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 6];
					iStaticGuards_7_Unknown[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 7];
					iStaticGuards_8_Unknown[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 8];
					iStaticGuards_9_Unknown[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 9];
					iStaticGuards_10_Colors[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 10];
					iStaticGuards_11_Number[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 11];
					iStaticGuards_12_Unknown[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 12];
					iStaticGuards_13_Unknown[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 13];
					iStaticGuards_14_Unknown[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 14];
					iStaticGuards_15_Unknown[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 15];
					iStaticGuards_16_Type[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 16];
					iStaticGuards_17_Hitpoints[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 17];
					iStaticGuards_18_SkelAct1[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 18];
					iStaticGuards_19_SkelAct2[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 19];
					iStaticGuards_20_Unknown[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 20];
					iStaticGuards_21_Unknown[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 21];
					iStaticGuards_22_Unknown[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 22];
					iStaticGuards_23_Unknown[iRoom - 1][iStatic] =
						iStaticGuardsACopyPaste[(iStatic * 23) + 23];
				}
				/*** dynamic guards ***/
				iDynamicGuards_Sets[iRoom - 1] = iDynamicGuardsACopyPaste[0];
				iDynamicGuards_Skill[iRoom - 1] = iDynamicGuardsACopyPaste[1];
				iDynamicGuards_Unknown1[iRoom - 1] = iDynamicGuardsACopyPaste[2];
				iDynamicGuards_Unknown2[iRoom - 1] = iDynamicGuardsACopyPaste[3];
				for (iDynamic = 0; iDynamic < 3; iDynamic++)
				{
					iDynamicGuards_1_SetNr[iRoom - 1][iDynamic] =
						iDynamicGuardsACopyPaste[(iDynamic * 10) + 4];
					iDynamicGuards_2_MaxTog[iRoom - 1][iDynamic] =
						iDynamicGuardsACopyPaste[(iDynamic * 10) + 5];
					iDynamicGuards_3_Row[iRoom - 1][iDynamic] =
						iDynamicGuardsACopyPaste[(iDynamic * 10) + 6];
					iDynamicGuards_4_Column[iRoom - 1][iDynamic] =
						iDynamicGuardsACopyPaste[(iDynamic * 10) + 7];
					iDynamicGuards_5_LatBefore[iRoom - 1][iDynamic] =
						iDynamicGuardsACopyPaste[(iDynamic * 10) + 8];
					iDynamicGuards_6_LatBetween[iRoom - 1][iDynamic] =
						iDynamicGuardsACopyPaste[(iDynamic * 10) + 9];
					iDynamicGuards_7_CanJump[iRoom - 1][iDynamic] =
						iDynamicGuardsACopyPaste[(iDynamic * 10) + 10];
					iDynamicGuards_8_SpIfAl[iRoom - 1][iDynamic] =
						iDynamicGuardsACopyPaste[(iDynamic * 10) + 11];
					iDynamicGuards_9_Amount[iRoom - 1][iDynamic] =
						iDynamicGuardsACopyPaste[(iDynamic * 10) + 12];
					iDynamicGuards_10_Hitpoints[iRoom - 1][iDynamic] =
						iDynamicGuardsACopyPaste[(iDynamic * 10) + 13];
				}
			}
		} else {
			if (iEditPoP == 1)
			{
				sGuardLocations[iRoom - 1] = 30;
			} else {
				iStaticGuards_Amount[iRoom - 1] = 0;
				iDynamicGuards_Sets[iRoom - 1] = 0;
			}
		}
	}
}
/*****************************************************************************/
void DateTime (char *sDateTime)
/*****************************************************************************/
{
	time_t clock;
	struct tm *loctim;

	clock = time (0);
	loctim = localtime (&clock);
	snprintf (sDateTime, MAX_DATETIME, "%04i-%02i-%02i %02i:%02i:%02i",
		loctim->tm_year + 1900, loctim->tm_mon + 1, loctim->tm_mday,
		loctim->tm_hour, loctim->tm_min, loctim->tm_sec);
}
/*****************************************************************************/
void SaveXML (char *sFileName)
/*****************************************************************************/
{
	int iFd;
	int iKidDir;
	int iTemp;
	int iTile;
	int iGuardLocation;
	int iGuardDirection;
	int iGuardSkill;
	int iGuardColors;
	char sToWrite[MAX_TOWRITE + 2];
	char sDateTime[MAX_DATETIME + 2];
	char sKey[100 + 2];
	char sValue[100 + 2];
	struct stat stStatus;

	/*** Create directory. ***/
	if (stat (XML_DIR, &stStatus) == -1)
	{
#if defined WIN32 || _WIN32 || WIN64 || _WIN64
mkdir (XML_DIR);
#else
mkdir (XML_DIR, 0700);
#endif
	}

	/*** Open file. ***/
	iFd = open (sFileName, O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, 0600);
	if (iFd == -1)
	{
		printf ("[FAILED] Could not create XML file: %s!\n", strerror (errno));
		exit (EXIT_ERROR);
	}

	snprintf (sToWrite, MAX_TOWRITE, "%s",
		"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	write (iFd, sToWrite, strlen (sToWrite));
	DateTime (sDateTime);
	snprintf (sToWrite, MAX_TOWRITE,
		"<!-- PoP1 level, exported %s with %s %s. -->\n",
		sDateTime, EDITOR_NAME, EDITOR_VERSION);
	write (iFd, sToWrite, strlen (sToWrite));
	snprintf (sToWrite, MAX_TOWRITE, "<level number=\"%lu\">\n",
		luLevelNr);
	write (iFd, sToWrite, strlen (sToWrite));

	/*** rooms ***/
	write (iFd, "\t<rooms>\n", 9);
	for (iTemp = 1; iTemp <= iRooms; iTemp++)
	{
		snprintf (sToWrite, MAX_TOWRITE,
			"\t\t<room number=\"%i\">\n",
			iTemp);
		write (iFd, sToWrite, strlen (sToWrite));

		/*** tiles ***/
		for (iTile = 0; iTile <= 29; iTile++)
		{
			snprintf (sToWrite, MAX_TOWRITE,
				"\t\t\t<tile element=\"%i\" modifier=\"%i\">\n",
				iThingA[iTemp][iTile], iModifierA[iTemp][iTile][1]);
			write (iFd, sToWrite, strlen (sToWrite));
		}

		/*** guard ***/
		iGuardLocation = sGuardLocations[iTemp - 1] + 1;
		if (iGuardLocation == 31) { iGuardLocation = 0; }
		if (sGuardDirections[iTemp - 1] == 255) { iGuardDirection = 1; }
		else if (sGuardDirections[iTemp - 1] == 0) { iGuardDirection = 2; }
		iGuardSkill = sGuardSkills[iTemp - 1];
		iGuardColors = sGuardColors[iTemp - 1];
		snprintf (sToWrite, MAX_TOWRITE,
			"\t\t\t<guard location=\"%i\" direction=\"%i\""
			" skill=\"%i\" colors=\"%i\">\n",
			iGuardLocation, iGuardDirection, iGuardSkill, iGuardColors);
		write (iFd, sToWrite, strlen (sToWrite));

		/*** links ***/
		snprintf (sToWrite, MAX_TOWRITE,
			"\t\t\t<links left=\"%i\" right=\"%i\" up=\"%i\" down=\"%i\">\n",
			iRoomConnections[iTemp][1], iRoomConnections[iTemp][2],
			iRoomConnections[iTemp][3], iRoomConnections[iTemp][4]);
		write (iFd, sToWrite, strlen (sToWrite));

		write (iFd, "\t\t</room>\n", 10);
	}
	write (iFd, "\t</rooms>\n", 10);

	/*** events ***/
	write (iFd, "\t<events>\n", 10);
	for (iTemp = 0; iTemp < 256; iTemp++)
	{
		snprintf (sToWrite, MAX_TOWRITE,
			"\t\t<event number=\"%i\" room=\"%i\" location=\"%i\" next=\"%i\">\n",
			iTemp + 1, EventInfo (iTemp, 1),
			EventInfo (iTemp, 2), EventInfo (iTemp, 3));
		write (iFd, sToWrite, strlen (sToWrite));
	}
	write (iFd, "\t</events>\n", 11);

	/*** prince ***/
	switch ((int)luKidDir)
	{
		case 0: iKidDir = 1; break;
		case 255: iKidDir = 2; break;
	}
	/*** 4 of 4 ***/
	if (((int)luLevelNr == 1) || ((int)luLevelNr == 13))
	{
		if (luKidDir == 0) { iKidDir = 2; } else { iKidDir = 1; }
	}
	snprintf (sToWrite, MAX_TOWRITE,
		"\t<prince room=\"%lu\" location=\"%lu\" direction=\"%i\">\n",
		luKidRoom, luKidPos, iKidDir);
	write (iFd, sToWrite, strlen (sToWrite));

	/*** userdata ***/
	snprintf (sToWrite, MAX_TOWRITE,
		"\t<userdata fields=\"%lu\">\n", luNumberOfFields);
	write (iFd, sToWrite, strlen (sToWrite));
	for (iTemp = 1; iTemp < iInformationNr; iTemp++)
	{
		snprintf (sKey, 200, "%s", sInformation[iTemp][0]);
		if (strcmp (sInformation[iTemp][0], "Editor Name") == 0)
			{ snprintf (sValue, MAX_TOWRITE, "%s", EDITOR_NAME); }
		else if (strcmp (sInformation[iTemp][0], "Level Author") == 0)
			{ snprintf (sValue, MAX_TOWRITE, "%s", sAuthor); }
		else if (strcmp (sInformation[iTemp][0], "Editor Version") == 0)
			{ snprintf (sValue, MAX_TOWRITE, "%s", EDITOR_VERSION); }
		else if (strcmp (sInformation[iTemp][0], "Time Last Modified") == 0)
			{ snprintf (sValue, MAX_TOWRITE, "%s", sDateTime); }
		else
			{ snprintf (sValue, MAX_TOWRITE, "%s", sInformation[iTemp][1]); }
		snprintf (sToWrite, MAX_TOWRITE,
			"\t\t<field key=\"%s\" value=\"%s\">\n",
			sKey, sValue);
		write (iFd, sToWrite, strlen (sToWrite));
	}
	write (iFd, "\t</userdata>\n", 13);

	write (iFd, "</level>\n", 9);

	close (iFd);
}
/*****************************************************************************/
void PoP1OrPoP2 (void)
/*****************************************************************************/
{
	SDL_Event event;
	int iPoP1OrPoP2;

	iPoP1OrPoP2 = 1;

	ShowPoP1OrPoP2();
	while (iPoP1OrPoP2 == 1)
	{
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							Quit(); break;
						case SDLK_KP_ENTER:
						case SDLK_RETURN:
						case SDLK_SPACE:
							if ((iPoP1 == 1) && (iOnPoP1 == 1))
							{
								iEditPoP = 1;
								iPoP1OrPoP2 = 0;
							}
							if ((iPoP2 == 1) && (iOnPoP2 == 1))
							{
								iEditPoP = 2;
								iPoP1OrPoP2 = 0;
							}
							break;
						case SDLK_1:
							if (iPoP1 == 1)
							{
								iEditPoP = 1;
								iPoP1OrPoP2 = 0;
							}
							break;
						case SDLK_2:
							if (iPoP2 == 1)
							{
								iEditPoP = 2;
								iPoP1OrPoP2 = 0;
							}
							break;
						case SDLK_LEFT:
							if (iOnPoP1 != 1)
							{
								iOnPoP1 = 1; iOnPoP2 = 0;
								ShowPoP1OrPoP2();
							}
							break;
						case SDLK_RIGHT:
							if (iOnPoP2 != 1)
							{
								iOnPoP1 = 0; iOnPoP2 = 1;
								ShowPoP1OrPoP2();
							}
							break;
						default: break;
					}
					break;
				case SDL_MOUSEMOTION:
					SDL_GetMouseState (&iXPos, &iYPos);
					if (InArea (event, 17, 17, 339, 436) == 1) /*** 1 ***/
					{
						if (iOnPoP1 != 1)
						{
							iOnPoP1 = 1; iOnPoP2 = 0;
							ShowPoP1OrPoP2();
						}
					} else if (InArea (event, 352, 17, 674, 436) == 1) { /*** 2 ***/
						if (iOnPoP2 != 1)
						{
							iOnPoP1 = 0; iOnPoP2 = 1;
							ShowPoP1OrPoP2();
						}
					} else {
						if ((iOnPoP1 != 0) || (iOnPoP2 != 0))
						{
							iOnPoP1 = 0; iOnPoP2 = 0;
							ShowPoP1OrPoP2();
						}
					}
					break;
				case SDL_MOUSEBUTTONUP:
					iOnPoP1 = 0;
					iOnPoP2 = 0;
					if (event.button.button == 1)
					{
						if ((iPoP1 == 1) && (InArea (event, 17, 17, 339, 436) == 1))
						{ /*** 1 ***/
							iEditPoP = 1;
							iPoP1OrPoP2 = 0;
						}
						if ((iPoP2 == 1) && (InArea (event, 352, 17, 674, 436) == 1))
						{ /*** 2 ***/
							iEditPoP = 2;
							iPoP1OrPoP2 = 0;
						}
						if ((iPoP1 == 0) && (iPoP2 == 0))
						{
							if (InArea (event, 57, 331, 635, 352) == 1)
								{ OpenURL ("http://www.popot.org/get_the_games/"
								"software/PoP1.zip"); }
							if (InArea (event, 57, 359, 635, 380) == 1)
								{ OpenURL ("http://www.popot.org/get_the_games/"
								"software/PoP2.zip"); }
						}
					}
					ShowPoP1OrPoP2(); break;
				case SDL_QUIT:
					Quit(); break;
			}
		}

		/*** prevent CPU eating ***/
		gamespeed = REFRESH;
		while ((SDL_GetTicks() - looptime) < gamespeed)
		{
			SDL_Delay (10);
		}
		looptime = SDL_GetTicks();
	}
}
/*****************************************************************************/
void ShowPoP1OrPoP2 (void)
/*****************************************************************************/
{
	/*** background ***/
	ShowImage (-13, (int[]){1, 0, 0, 0}, screen, 31, 0, 0, 692, 455);

	if ((iPoP1 == 0) && (iPoP2 == 0))
	{
		ShowImage (-13, (int[]){9, 0, 0, 0}, screen, 94, 0, 0, 658, 420);
	} else if (iPR == 0) {
		ShowImage (-13, (int[]){8, 0, 0, 0}, screen, 94, 0, 0, 658, 420);
	} else {
		/*** PoP1 ***/
		if (iPoP1 == 0)
		{
			/*** disabled ***/
			ShowImage (-13, (int[]){2, 0, 0, 0}, screen, 94, 0, 0, 323, 420);
		} else if (iOnPoP1 == 0) {
			/*** off ***/
			ShowImage (-13, (int[]){3, 0, 0, 0}, screen, 94, 0, 0, 323, 420);
		} else {
			/*** on ***/
			ShowImage (-13, (int[]){4, 0, 0, 0}, screen, 94, 0, 0, 323, 420);
		}

		/*** PoP2 ***/
		if (iPoP2 == 0)
		{
			/*** disabled ***/
			ShowImage (-13, (int[]){5, 0, 0, 0}, screen, 95, 0, 0, 323, 420);
		} else if (iOnPoP2 == 0) {
			/*** off ***/
			ShowImage (-13, (int[]){6, 0, 0, 0}, screen, 95, 0, 0, 323, 420);
		} else {
			/*** on ***/
			ShowImage (-13, (int[]){7, 0, 0, 0}, screen, 95, 0, 0, 323, 420);
		}
	}

	/*** refresh screen ***/
	if (SDL_Flip (screen) == -1)
		{ printf ("[FAILED] SDL_Flip error!\n"); exit (EXIT_ERROR); }
}
/*****************************************************************************/
void PreLoad (char *sPath, char *sPNG, SDL_Surface **imgImage)
/*****************************************************************************/
{
	char sImage[MAX_IMG + 2];
	SDL_Surface *loaded_img;
	int iBarHeight;

	snprintf (sImage, MAX_IMG, "%s%s", sPath, sPNG);
	loaded_img = IMG_Load (sImage);
	if (!loaded_img)
	{
		printf ("[FAILED] IMG_Load: %s\n", IMG_GetError());
		exit (EXIT_ERROR);
	}
	*imgImage = SDL_DisplayFormatAlpha (loaded_img);

	iPreLoaded++;
	iBarHeight = (int)(((float)iPreLoaded/(float)iNrToPreLoad) * BAR_FULL);
	if (iBarHeight >= iCurrentBarHeight + 10)
		{ LoadingBar (screen, iBarHeight); }
}
/*****************************************************************************/
void PreLoadSet (char *sPath, char cType, char *sTile,
	SDL_Surface **imgReg, SDL_Surface **imgSel)
/*****************************************************************************/
{
	char sImage[MAX_IMG + 2];
	SDL_Surface *loaded_img;
	int iBarHeight;

	/*** regular ***/
	snprintf (sImage, MAX_IMG, "%s%c_%s.png", sPath, cType, sTile);
	loaded_img = IMG_Load (sImage);
	if (!loaded_img)
	{
		printf ("[FAILED] IMG_Load: %s\n", IMG_GetError());
		exit (EXIT_ERROR);
	}
	*imgReg = SDL_DisplayFormatAlpha (loaded_img);

	/*** selected ***/
	snprintf (sImage, MAX_IMG, "%s%c_sel_%s.png", sPath, cType, sTile);
	loaded_img = IMG_Load (sImage);
	if (!loaded_img)
	{
		printf ("[FAILED] IMG_Load: %s\n", IMG_GetError());
		exit (EXIT_ERROR);
	}
	*imgSel = SDL_DisplayFormatAlpha (loaded_img);

	iPreLoaded+=2;
	iBarHeight = (int)(((float)iPreLoaded/(float)iNrToPreLoad) * BAR_FULL);
	if (iBarHeight >= iCurrentBarHeight + 10)
		{ LoadingBar (screen, iBarHeight); }
}
/*****************************************************************************/
void LoadingBar (SDL_Surface *screen, int iBarHeight)
/*****************************************************************************/
{
	Uint32 color2;
	SDL_Rect bar;

	/*** bar ***/
	bar.x = 12;
	bar.y = 443 - iBarHeight;
	bar.w = 16;
	bar.h = iBarHeight;
	color2 = SDL_MapRGB (screen->format, 0x44, 0x44, 0x44);
	SDL_FillRect (screen, &bar, color2);
	iCurrentBarHeight = iBarHeight;

	/*** hourglass sprite ***/
	iHourglassFrame = floor (((float)iBarHeight/BAR_FULL) * 7);
	ShowImage (-4, (int[]){3, 0, 0, 0}, screen, 135, 0, 0, 38, 56);

	/*** sand sprite ***/
	ShowImage (-4, (int[]){4, 0, 0, 0}, screen, 136, 0, 0, 2, 16);
	iSandFrame++;
	if (iSandFrame == 4) { iSandFrame = 1; }

	/*** refresh screen ***/
	if (SDL_Flip (screen) == -1)
		{ printf ("[FAILED] SDL_Flip error!\n"); exit (EXIT_ERROR); }
}
/*****************************************************************************/
void CenterNumber (SDL_Surface *screen, int iNumber, int iX, int iY,
	SDL_Color back, SDL_Color fore)
/*****************************************************************************/
{
	char sText[MAX_TEXT];

	snprintf (sText, MAX_TEXT, "%i", iNumber);
	message = TTF_RenderText_Shaded (font3, sText, back, fore);
	if ((iNumber >= 0) && (iNumber <= 9))
	{
		offset.x = iX + 21;
	} else if ((iNumber >= 10) && (iNumber <= 99)) {
		offset.x = iX + 14;
	} else {
		offset.x = iX + 7;
	}
	offset.y = iY - 1;
	CustomBlitSurface (message, "message", NULL, screen, &offset, arFont);
}
/*****************************************************************************/
int IntsToHexToSSLittleEndian (int iHexRight, int iHexLeft)
/*****************************************************************************/
{
	char sHex[MAX_HEX];
	int iValue;

	snprintf (sHex, MAX_HEX, "%02x%02x", iHexRight, iHexLeft);
	iValue = strtol (sHex, NULL, 16);
	if (iValue <= 32767)
	{
		return (iValue);
	} else {
		return ((65535 - iValue + 1) * -1);
	}
}
/*****************************************************************************/
void SSLittleEndianToHexToInts (int iValue, int *iHexRight, int *iHexLeft)
/*****************************************************************************/
{
	char sHex[MAX_HEX];
	char sHexRight[MAX_HEX];
	char sHexLeft[MAX_HEX];

	snprintf (sHex, MAX_HEX, "%04hx", iValue);
	snprintf (sHexRight, MAX_HEX, "%c%c", sHex[0], sHex[1]);
	snprintf (sHexLeft, MAX_HEX, "%c%c", sHex[2], sHex[3]);
	*iHexLeft = (int)strtol(sHexLeft, NULL, 16);
	*iHexRight = (int)strtol(sHexRight, NULL, 16);
}
/*****************************************************************************/
int PlusMinus (SDL_Event event, int *iWhat, int iX, int iY,
	int iMin, int iMax, int iChange, int iAddChanged)
/*****************************************************************************/
{
	if ((InArea (event, iX, iY, iX + 13, iY + 20) == 1) &&
		(((iChange < 0) && (*iWhat > iMin)) ||
		((iChange > 0) && (*iWhat < iMax))))
	{
		*iWhat = *iWhat + iChange;
		if ((iChange < 0) && (*iWhat < iMin)) { *iWhat = iMin; }
		if ((iChange > 0) && (*iWhat > iMax)) { *iWhat = iMax; }
		if (iAddChanged == 1) { iChanged++; }
		PlaySound ("wav/plus_minus.wav");
		return (1);
	} else { return (0); }
}
/*****************************************************************************/
int WhichStaticGuard (int iGuardNr)
/*****************************************************************************/
{
	int iAct1, iAct2;

	switch (iStaticGuards_16_Type[iCurRoom - 1][iGuardNr])
	{
		case 0:
			switch (iStaticGuards_10_Colors[iCurRoom - 1][iGuardNr])
			{
				/*** Case 0 is only used for some skeletons in level 4. ***/
				case 1: return (1); break; /*** black guard ***/
				case 2: return (2); break; /*** blue guard ***/
				case 3: return (3); break; /*** red guard ***/
				default: return (-1); break;
			}
			break;
		case 2:
			iAct1 = iStaticGuards_18_SkelAct1[iCurRoom - 1][iGuardNr];
			iAct2 = iStaticGuards_19_SkelAct2[iCurRoom - 1][iGuardNr];
			if ((iAct1 == 0) && (iAct2 == 0))
			{
				return (4); /*** skeleton wakes up; wobbly if jump ***/
			}
			if ((iAct1 == 1) && (iAct2 == 0))
			{
				return (5); /*** skeleton already awake ***/
			}
			if ((iAct1 == 255) && (iAct2 == 255))
			{
				return (6); /*** skeleton only sleeps ***/
			}
			if (((iAct1 == 255) && (iAct2 == 0)) ||
				((iAct1 == 0) && (iAct2 == 255)) ||
				((iAct1 == 1) && (iAct2 == 255)))
			{
				return (7); /*** skeleton wakes up; NOT wobbly if jump ***/
			}
			break;
		case 5: return (8); break; /*** head, long hair ***/
		case 6: return (9); break; /*** head, short hair ***/
		case 8: return (10); break; /*** snake ***/
		case 7: return (11); break; /*** birdman ***/
		case 1: return (12); break; /*** sword ***/
		case 3: return (13); break; /*** fake prince ***/
		default:
			if (iDebug == 1)
			{
				printf ("[ INFO ] Unknown guard type: %i\n",
					iStaticGuards_16_Type[iCurRoom - 1][iGuardNr]);
			}
			return (-1);
			break;
	}
	return (-1);
}
/*****************************************************************************/
void Backgrounds (void)
/*****************************************************************************/
{
	int iModifier[4 + 2];
	int iTemp;
	int iLoc;

	/*** left of this room ***/
	if (iRoomConnections[iCurRoom][1] != 0)
	{
		iModifier[0] = iModifierA[iRoomConnections[iCurRoom][1]][9][1];
		iModifier[1] = iModifierA[iRoomConnections[iCurRoom][1]][9][2];
		iModifier[2] = iModifierA[iRoomConnections[iCurRoom][1]][9][3];
		iModifier[3] = iModifierA[iRoomConnections[iCurRoom][1]][9][4];
		ShowBackground ((int[]){iModifier[0], iModifier[1],
			iModifier[2], iModifier[3]}, -1);
		iModifier[0] = iModifierA[iRoomConnections[iCurRoom][1]][19][1];
		iModifier[1] = iModifierA[iRoomConnections[iCurRoom][1]][19][2];
		iModifier[2] = iModifierA[iRoomConnections[iCurRoom][1]][19][3];
		iModifier[3] = iModifierA[iRoomConnections[iCurRoom][1]][19][4];
		ShowBackground ((int[]){iModifier[0], iModifier[1],
			iModifier[2], iModifier[3]}, -2);
		iModifier[0] = iModifierA[iRoomConnections[iCurRoom][1]][29][1];
		iModifier[1] = iModifierA[iRoomConnections[iCurRoom][1]][29][2];
		iModifier[2] = iModifierA[iRoomConnections[iCurRoom][1]][29][3];
		iModifier[3] = iModifierA[iRoomConnections[iCurRoom][1]][29][4];
		ShowBackground ((int[]){iModifier[0], iModifier[1],
			iModifier[2], iModifier[3]}, -3);
	}

	/*** inside this room ***/
	for (iTemp = 1; iTemp <= 30; iTemp++)
	{
		if ((iTemp >= 1) && (iTemp <= 10)) { iLoc = 20 + iTemp; }
		if ((iTemp >= 11) && (iTemp <= 20)) { iLoc = iTemp; }
		if ((iTemp >= 21) && (iTemp <= 30)) { iLoc = -20 + iTemp; }

		iModifier[0] = iModifierA[iCurRoom][iLoc - 1][1];
		iModifier[1] = iModifierA[iCurRoom][iLoc - 1][2];
		iModifier[2] = iModifierA[iCurRoom][iLoc - 1][3];
		iModifier[3] = iModifierA[iCurRoom][iLoc - 1][4];
		ShowBackground ((int[]){iModifier[0], iModifier[1],
			iModifier[2], iModifier[3]}, iLoc);
	}

	/*** under this room ***/
	if (iRoomConnections[iCurRoom][4] != 0)
	{
		for (iTemp = 1; iTemp <= 10; iTemp++)
		{
			iModifier[0] = iModifierA[iRoomConnections[iCurRoom][4]][iTemp - 1][1];
			iModifier[1] = iModifierA[iRoomConnections[iCurRoom][4]][iTemp - 1][2];
			iModifier[2] = iModifierA[iRoomConnections[iCurRoom][4]][iTemp - 1][3];
			iModifier[3] = iModifierA[iRoomConnections[iCurRoom][4]][iTemp - 1][4];
			ShowBackground ((int[]){iModifier[0], iModifier[1],
				iModifier[2], iModifier[3]}, -4 - iTemp);
		}
	}
}
/*****************************************************************************/
void ShowBackground (int iModifier[], int iLoc)
/*****************************************************************************/
{
	SDL_Rect dest;
	SDL_Rect loc;
	int iToSwith;
	char sText[MAX_TEXT + 2];

	switch (iLoc)
	{
		case -1: dest.x = HOR0; dest.y = VER1; break;
		case -2: dest.x = HOR0; dest.y = VER2; break;
		case -3: dest.x = HOR0; dest.y = VER3; break;
		case -4: dest.x = HOR0; dest.y = VER4; break;
		case -5: dest.x = HOR1; dest.y = VER4; break;
		case -6: dest.x = HOR2; dest.y = VER4; break;
		case -7: dest.x = HOR3; dest.y = VER4; break;
		case -8: dest.x = HOR4; dest.y = VER4; break;
		case -9: dest.x = HOR5; dest.y = VER4; break;
		case -10: dest.x = HOR6; dest.y = VER4; break;
		case -11: dest.x = HOR7; dest.y = VER4; break;
		case -12: dest.x = HOR8; dest.y = VER4; break;
		case -13: dest.x = HOR9; dest.y = VER4; break;
		case -14: dest.x = HOR10; dest.y = VER4; break;
		case 1: dest.x = HOR1; dest.y = VER1; break;
		case 2: dest.x = HOR2; dest.y = VER1; break;
		case 3: dest.x = HOR3; dest.y = VER1; break;
		case 4: dest.x = HOR4; dest.y = VER1; break;
		case 5: dest.x = HOR5; dest.y = VER1; break;
		case 6: dest.x = HOR6; dest.y = VER1; break;
		case 7: dest.x = HOR7; dest.y = VER1; break;
		case 8: dest.x = HOR8; dest.y = VER1; break;
		case 9: dest.x = HOR9; dest.y = VER1; break;
		case 10: dest.x = HOR10; dest.y = VER1; break;
		case 11: dest.x = HOR1; dest.y = VER2; break;
		case 12: dest.x = HOR2; dest.y = VER2; break;
		case 13: dest.x = HOR3; dest.y = VER2; break;
		case 14: dest.x = HOR4; dest.y = VER2; break;
		case 15: dest.x = HOR5; dest.y = VER2; break;
		case 16: dest.x = HOR6; dest.y = VER2; break;
		case 17: dest.x = HOR7; dest.y = VER2; break;
		case 18: dest.x = HOR8; dest.y = VER2; break;
		case 19: dest.x = HOR9; dest.y = VER2; break;
		case 20: dest.x = HOR10; dest.y = VER2; break;
		case 21: dest.x = HOR1; dest.y = VER3; break;
		case 22: dest.x = HOR2; dest.y = VER3; break;
		case 23: dest.x = HOR3; dest.y = VER3; break;
		case 24: dest.x = HOR4; dest.y = VER3; break;
		case 25: dest.x = HOR5; dest.y = VER3; break;
		case 26: dest.x = HOR6; dest.y = VER3; break;
		case 27: dest.x = HOR7; dest.y = VER3; break;
		case 28: dest.x = HOR8; dest.y = VER3; break;
		case 29: dest.x = HOR9; dest.y = VER3; break;
		case 30: dest.x = HOR10; dest.y = VER3; break;

		case 100: dest.x = 127; dest.y = 242; break; /*** change custom ***/
		case 101: dest.x = 128; dest.y = 147; break; /*** events screen ***/
	}

	switch (iModifier[3])
	{
		case 0: case 4: case 8: loc.x = 0 * iScale; break;
		case 1: case 5: case 9: loc.x = 64 * iScale; break;
		case 2: case 6: case 10: loc.x = 128 * iScale; break;
		case 3: case 7: case 11: loc.x = 192 * iScale; break;
	}
	if (cCurType == 'r') { iToSwith = iRuinsBHeight[iModifier[2]]; }
	if (cCurType == 't') { iToSwith = iTempleBHeight[iModifier[2]]; }
	switch (iModifier[3])
	{
		case 0: case 1: case 2: case 3:
			switch (iToSwith)
			{
				case 1: loc.y = 0; break;
				case 2: loc.y = 126 * iScale; break;
				case 3: loc.y = 252 * iScale; break;
			}
			break;
		case 4: case 5: case 6: case 7:
			switch (iToSwith)
			{
				case 2: loc.y = 0; break;
				case 3: loc.y = 126 * iScale; break;
			}
			break;
		case 8: case 9: case 10: case 11:
			loc.y = 0;
			break;
	}
	loc.w = 64 * iScale;
	loc.h = 126 * iScale;
	if ((iLoc != 100) && (iLoc != 101))
	{
		dest.x+=51; /*** Tile (116) - back (64) - green border (1). ***/
		dest.y++; /*** Green border (1). ***/
	}
	if (cCurType == 'r')
	{
		snprintf (sText, MAX_TEXT, "ruinsb[%i]", iModifier[2]);
		CustomBlitSurface (ruinsb[iModifier[2]], sText, &loc,
			screen, &dest, arImage);
	} else {
		snprintf (sText, MAX_TEXT, "templeb[%i]", iModifier[2]);
		CustomBlitSurface (templeb[iModifier[2]], sText, &loc,
			screen, &dest, arImage);
	}
}
/*****************************************************************************/
void OpenURL (char *sURL)
/*****************************************************************************/
{
	pid_t pid;

#if defined WIN32 || _WIN32 || WIN64 || _WIN64
ShellExecute (NULL, "open", sURL, NULL, NULL, SW_SHOWNORMAL);
#else
pid = fork();
if (pid == 0)
{
	execl ("/usr/bin/xdg-open", "xdg-open", sURL, (char *)NULL);
	exit (EXIT_NORMAL);
}
#endif
}
/*****************************************************************************/
int ModAsJumps (int iModifier)
/*****************************************************************************/
{
	int iSMod;

	iSMod = iModifier;
	if (iSMod >= 128) { iSMod-=128; }
	if ((iSMod >= 0) && (iSMod <= 15)) { return (3); }
	if ((iSMod >= 16) && (iSMod <= 31)) { return (2); }
	if ((iSMod >= 32) && (iSMod <= 47)) { return (1); }
	if ((iSMod >= 48) && (iSMod <= 63)) { return (4); }
	if ((iSMod >= 64) && (iSMod <= 79)) { return (3); }
	if ((iSMod >= 80) && (iSMod <= 95)) { return (2); }
	if ((iSMod >= 96) && (iSMod <= 111)) { return (2); }
	if ((iSMod >= 112) && (iSMod <= 127)) { return (4); }
	return (3); /*** To prevent a -Wreturn-type warning. ***/
}
/*****************************************************************************/
int JumpsAsMod (int iJumps)
/*****************************************************************************/
{
	switch (iJumps)
	{
		case 1: return (32); break;
		case 2: return (16); break;
		case 3: return (0); break;
		case 4: return (48); break;
	}
	return (0); /*** To prevent a -Wreturn-type warning. ***/
}
/*****************************************************************************/
int ModAsFrame (int iModifier)
/*****************************************************************************/
{
	/*** Simple function, just to keep things consistent. ***/
	return (iModifier % 16);
}
/*****************************************************************************/
int FrameAsMod (int iFrame)
/*****************************************************************************/
{
	/*** Simple function, just to keep things consistent. ***/
	return (iFrame);
}
/*****************************************************************************/
int ModAsPercent (int iModifier, int iThing)
/*****************************************************************************/
{
	if (iModifier <= 0) { return (0); }
	if (iThing == 4)
	{
		return (ceil (iModifier / 2)); /*** iModifier is 0-200. ***/
	}
	if (iThing == 17)
	{
		return (ceil (iModifier * 2.38)); /*** iModifier is 0-42. ***/
	}
	return (0); /*** To prevent a -Wreturn-type warning. ***/
}
/*****************************************************************************/
int PercentAsMod (int iPercent, int iThing)
/*****************************************************************************/
{
	if (iThing == 4)
	{
		return (iPercent * 2); /*** iModifier is 0-200. ***/
	}
	if (iThing == 17)
	{
		return (ceil (iPercent * 0.42)); /*** iModifier is 0-42. ***/
	}
	return (0); /*** To prevent a -Wreturn-type warning. ***/
}
/*****************************************************************************/
int ModAsWall (int iModifier)
/*****************************************************************************/
{
	if (cCurType == 'c')
	{
		if (iModifier >= 128) { return (2); } else { return (1); }
	}
	if (cCurType == 'r')
	{
		if ((iModifier >= 0) && (iModifier <= 3)) { return (1); }
		if ((iModifier >= 4) && (iModifier <= 7)) { return (2); }
		if ((iModifier >= 8) && (iModifier <= 11)) { return (3); }
		if ((iModifier >= 12) && (iModifier <= 15)) { return (4); }
	}
	if (cCurType == 't')
	{
		switch (iModifier % 8)
		{
			case 0: return (1);
			case 1: return (2);
			default: return (3);
		}
	}
	return (0); /*** To prevent a -Wreturn-type warning. ***/
}
/*****************************************************************************/
int WallAsMod (int iWall, int iModifier)
/*****************************************************************************/
{
	if (cCurType == 'c')
	{
		switch (iWall)
		{
			case 1: return (iModifier); break;
			case 2: return (128 + iModifier); break;
		}
	}
	if (cCurType == 'r')
	{
		switch (iWall)
		{
			case 1: return (iModifier); break;
			case 2: return (4 + iModifier); break;
			case 3: return (8 + iModifier); break;
			case 4: return (12 + iModifier); break;
		}
	}
	if (cCurType == 't')
	{
		switch (iWall)
		{
			case 1: return (0); break;
			case 2: return (1); break;
			case 3: return (2); break;
		}
	}
	return (iModifier); /*** To prevent a -Wreturn-type warning. ***/
}
/*****************************************************************************/
void PoP1Basics (void)
/*****************************************************************************/
{
	if (system (PR_EXECUTABLE " -x -f --resource=" PR_RESOURCES
		" " LEVELS_DAT " > " DEVNULL) == -1)
	{
		printf ("[FAILED] Could not export the levels: %s!\n", strerror (errno));
		exit (EXIT_ERROR);
	}
	iRooms = 24;
	iRoomLinks = 24 * 4;
	if ((iStartLevel < 0) || (iStartLevel > 15)) { iStartLevel = 1; }
}
/*****************************************************************************/
void PoP2Basics (void)
/*****************************************************************************/
{
	if (system (PR_EXECUTABLE " -xlevels2 -f --resource=" PR_POP2
		" " PRINCE_DAT " > " DEVNULL) == -1)
	{
		printf ("[FAILED] Could not export the levels: %s!\n", strerror (errno));
		exit (EXIT_ERROR);
	}
	iRooms = 32;
	iRoomLinks = 32 * 4;
	if ((iStartLevel < 1) || (iStartLevel > 28)) { iStartLevel = 1; }
}
/*****************************************************************************/
int HiSetBackArea (SDL_Event event, int iX, int iY,
	int iLoc, int iMod3, int iMod4, int iType)
/*****************************************************************************/
{
	int iTile;
	int iRoom;

	if (InArea (event, iX, iY, iX + 32, iY + 63) == 1)
	{
		switch (iType)
		{
			case 0:
				if ((iMod3Hi != iMod3) || (iMod4Hi != iMod4))
				{
					iMod3Hi = iMod3;
					iMod4Hi = iMod4;
					ShowChangeBackground (iLoc);
				}
				return (0); break;
			case 1: /*** one location ***/
				iModifierA[iCurRoom][iLoc - 1][3] = iMod3;
				iModifierA[iCurRoom][iLoc - 1][4] = iMod4;
				return (1); break;
			case 2: /*** entire room ***/
				for (iTile = 0; iTile < 30; iTile++)
				{
					iModifierA[iCurRoom][iTile][3] = iMod3;
					iModifierA[iCurRoom][iTile][4] = iMod4;
				}
				return (1); break;
			case 3: /*** entire level ***/
				for (iRoom = 1; iRoom <= iRooms; iRoom++)
				{
					for (iTile = 0; iTile < 30; iTile++)
					{
						iModifierA[iRoom][iTile][3] = iMod3;
						iModifierA[iRoom][iTile][4] = iMod4;
					}
				}
				return (1); break;
			default: return (0); break;
		}
	} else { return (0); }
}
/*****************************************************************************/
int HiSetBack (SDL_Event event, int iLoc, int iType)
/*****************************************************************************/
{
	if (cCurType == 'r')
	{
		if (HiSetBackArea (event, 601, 159, iLoc, 0, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 23, 86, iLoc, 1, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 55, 86, iLoc, 1, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 23, 23, iLoc, 1, 4, iType)) { return (1); }
		if (HiSetBackArea (event, 55, 23, iLoc, 1, 5, iType)) { return (1); }
		if (HiSetBackArea (event, 97, 86, iLoc, 2, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 129, 86, iLoc, 2, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 97, 23, iLoc, 2, 4, iType)) { return (1); }
		if (HiSetBackArea (event, 129, 23, iLoc, 2, 5, iType)) { return (1); }
		if (HiSetBackArea (event, 171, 86, iLoc, 3, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 203, 86, iLoc, 3, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 171, 23, iLoc, 3, 4, iType)) { return (1); }
		if (HiSetBackArea (event, 203, 23, iLoc, 3, 5, iType)) { return (1); }
		if (HiSetBackArea (event, 245, 86, iLoc, 4, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 277, 86, iLoc, 4, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 245, 23, iLoc, 4, 4, iType)) { return (1); }
		if (HiSetBackArea (event, 277, 23, iLoc, 4, 5, iType)) { return (1); }
		if (HiSetBackArea (event, 319, 86, iLoc, 5, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 351, 86, iLoc, 5, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 383, 86, iLoc, 5, 2, iType)) { return (1); }
		if (HiSetBackArea (event, 415, 86, iLoc, 5, 3, iType)) { return (1); }
		if (HiSetBackArea (event, 319, 23, iLoc, 5, 4, iType)) { return (1); }
		if (HiSetBackArea (event, 351, 23, iLoc, 5, 5, iType)) { return (1); }
		if (HiSetBackArea (event, 383, 23, iLoc, 5, 6, iType)) { return (1); }
		if (HiSetBackArea (event, 415, 23, iLoc, 5, 7, iType)) { return (1); }
		if (HiSetBackArea (event, 467, 149, iLoc, 6, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 499, 149, iLoc, 6, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 467, 86, iLoc, 6, 4, iType)) { return (1); }
		if (HiSetBackArea (event, 499, 86, iLoc, 6, 5, iType)) { return (1); }
		if (HiSetBackArea (event, 467, 23, iLoc, 6, 8, iType)) { return (1); }
		if (HiSetBackArea (event, 499, 23, iLoc, 6, 9, iType)) { return (1); }
		if (HiSetBackArea (event, 23, 222, iLoc, 7, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 55, 222, iLoc, 7, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 23, 159, iLoc, 7, 4, iType)) { return (1); }
		if (HiSetBackArea (event, 55, 159, iLoc, 7, 5, iType)) { return (1); }
		if (HiSetBackArea (event, 97, 222, iLoc, 8, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 129, 222, iLoc, 8, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 161, 222, iLoc, 8, 2, iType)) { return (1); }
		if (HiSetBackArea (event, 193, 222, iLoc, 8, 3, iType)) { return (1); }
		if (HiSetBackArea (event, 97, 159, iLoc, 8, 4, iType)) { return (1); }
		if (HiSetBackArea (event, 129, 159, iLoc, 8, 5, iType)) { return (1); }
		if (HiSetBackArea (event, 161, 159, iLoc, 8, 6, iType)) { return (1); }
		if (HiSetBackArea (event, 193, 159, iLoc, 8, 7, iType)) { return (1); }
		if (HiSetBackArea (event, 235, 159, iLoc, 9, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 267, 159, iLoc, 9, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 309, 159, iLoc, 10, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 341, 159, iLoc, 10, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 383, 159, iLoc, 12, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 425, 159, iLoc, 13, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 23, 295, iLoc,  14, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 65, 295, iLoc,  15, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 107, 295, iLoc, 16, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 149, 295, iLoc, 17, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 191, 295, iLoc, 18, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 233, 295, iLoc, 19, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 275, 295, iLoc, 20, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 23, 368, iLoc, 22, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 65, 368, iLoc, 23, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 107, 368, iLoc, 24, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 149, 368, iLoc, 25, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 191, 368, iLoc, 26, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 233, 368, iLoc, 30, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 275, 368, iLoc, 31, 0, iType)) { return (1); }
	}
	if (cCurType == 't')
	{
		if (HiSetBackArea (event, 344, 373, iLoc, 0, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 18, 18, iLoc, 1, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 60, 18, iLoc, 2, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 102, 18, iLoc, 3, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 144, 18, iLoc, 4, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 186, 18, iLoc, 5, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 228, 18, iLoc, 6, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 270, 18, iLoc, 7, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 60, 91, iLoc, 8, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 144, 91, iLoc, 9, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 102, 91, iLoc, 10, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 18, 91, iLoc, 11, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 186, 91, iLoc, 12, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 228, 91, iLoc, 13, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 270, 91, iLoc, 14, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 186, 164, iLoc, 15, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 228, 164, iLoc, 16, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 270, 164, iLoc, 17, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 312, 18, iLoc, 18, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 344, 18, iLoc, 18, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 18, 227, iLoc, 19, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 50, 227, iLoc, 19, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 18, 164, iLoc, 19, 4, iType)) { return (1); }
		if (HiSetBackArea (event, 50, 164, iLoc, 19, 5, iType)) { return (1); }
		if (HiSetBackArea (event, 102, 227, iLoc, 20, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 134, 227, iLoc, 20, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 102, 164, iLoc, 20, 4, iType)) { return (1); }
		if (HiSetBackArea (event, 134, 164, iLoc, 20, 5, iType)) { return (1); }
		if (HiSetBackArea (event, 102, 373, iLoc, 21, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 18, 300, iLoc, 22, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 60, 300, iLoc, 23, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 102, 300, iLoc, 24, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 144, 300, iLoc, 25, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 176, 300, iLoc, 25, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 218, 300, iLoc, 26, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 250, 300, iLoc, 26, 1, iType)) { return (1); }
		if (HiSetBackArea (event, 292, 300, iLoc, 27, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 18, 373, iLoc, 28, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 60, 373, iLoc, 29, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 144, 373, iLoc, 30, 0, iType)) { return (1); }
		if (HiSetBackArea (event, 186, 373, iLoc, 31, 0, iType)) { return (1); }
	}
	return (0);
}
/*****************************************************************************/
int BackKey (SDL_Event event, char cKey, int iLoc)
/*****************************************************************************/
{
	int iShift;
	char cRealKey;
	int iMod3, iMod4;

	iShift = 0;
	iMod3 = -1;
	iMod4 = -1;

	if ((event.key.keysym.mod & KMOD_LSHIFT) ||
		(event.key.keysym.mod & KMOD_RSHIFT))
	{
		iShift = 1;
	}
	if (iShift == 1) { cRealKey = toupper (cKey); } else { cRealKey = cKey; }
	if (cCurType == 'r')
	{
		switch (cRealKey)
		{
			case 'a': iMod3 = 1; iMod4 = 4; break;
			case 'b': iMod3 = 1; iMod4 = 5; break;
			case 'c': iMod3 = 1; iMod4 = 0; break;
			case 'd': iMod3 = 1; iMod4 = 1; break;
			case 'e': iMod3 = 2; iMod4 = 4; break;
			case 'f': iMod3 = 2; iMod4 = 5; break;
			case 'g': iMod3 = 2; iMod4 = 0; break;
			case 'h': iMod3 = 2; iMod4 = 1; break;
			case 'i': iMod3 = 3; iMod4 = 4; break;
			case 'j': iMod3 = 3; iMod4 = 5; break;
			case 'k': iMod3 = 3; iMod4 = 0; break;
			case 'l': iMod3 = 3; iMod4 = 1; break;
			case 'm': iMod3 = 4; iMod4 = 4; break;
			case 'n': iMod3 = 4; iMod4 = 5; break;
			case 'o': iMod3 = 4; iMod4 = 0; break;
			case 'p': iMod3 = 4; iMod4 = 1; break;
			case 'q': iMod3 = 5; iMod4 = 4; break;
			case 'r': iMod3 = 5; iMod4 = 5; break;
			case 's': iMod3 = 5; iMod4 = 6; break;
			case 't': iMod3 = 5; iMod4 = 7; break;
			case 'u': iMod3 = 5; iMod4 = 0; break;
			case 'v': iMod3 = 5; iMod4 = 1; break;
			case 'w': iMod3 = 5; iMod4 = 2; break;
			case 'x': iMod3 = 5; iMod4 = 3; break;
			case 'y': iMod3 = 6; iMod4 = 8; break;
			case 'z': iMod3 = 6; iMod4 = 9; break;
			case 'A': iMod3 = 6; iMod4 = 4; break;
			case 'B': iMod3 = 6; iMod4 = 5; break;
			case 'C': iMod3 = 6; iMod4 = 0; break;
			case 'D': iMod3 = 6; iMod4 = 1; break;
			case 'E': iMod3 = 7; iMod4 = 4; break;
			case 'F': iMod3 = 7; iMod4 = 5; break;
			case 'G': iMod3 = 7; iMod4 = 0; break;
			case 'H': iMod3 = 7; iMod4 = 1; break;
			case 'I': iMod3 = 8; iMod4 = 4; break;
			case 'J': iMod3 = 8; iMod4 = 5; break;
			case 'K': iMod3 = 8; iMod4 = 6; break;
			case 'L': iMod3 = 8; iMod4 = 7; break;
			case 'M': iMod3 = 8; iMod4 = 0; break;
			case 'N': iMod3 = 8; iMod4 = 1; break;
			case 'O': iMod3 = 8; iMod4 = 2; break;
			case 'P': iMod3 = 8; iMod4 = 3; break;
			case 'Q': iMod3 = 9; iMod4 = 0; break;
			case 'R': iMod3 = 9; iMod4 = 1; break;
			case 'S': iMod3 = 10; iMod4 = 0; break;
			case 'T': iMod3 = 10; iMod4 = 1; break;
			case 'U': iMod3 = 12; iMod4 = 0; break;
			case 'V': iMod3 = 13; iMod4 = 0; break;
			case 'W': iMod3 = 14; iMod4 = 0; break;
			case 'X': iMod3 = 15; iMod4 = 0; break;
			case 'Y': iMod3 = 16; iMod4 = 0; break;
			case 'Z': iMod3 = 17; iMod4 = 0; break;
			case '0': iMod3 = 18; iMod4 = 0; break;
			case '1': iMod3 = 19; iMod4 = 0; break;
			case '2': iMod3 = 20; iMod4 = 0; break;
			case '3': iMod3 = 22; iMod4 = 0; break;
			case '4': iMod3 = 23; iMod4 = 0; break;
			case '5': iMod3 = 24; iMod4 = 0; break;
			case '6': iMod3 = 25; iMod4 = 0; break;
			case '7': iMod3 = 26; iMod4 = 0; break;
			case '8': iMod3 = 30; iMod4 = 0; break;
			case '9': iMod3 = 31; iMod4 = 0; break;
			case '.': iMod3 = 0; iMod4 = 0; break;
		}
	}
	if (cCurType == 't')
	{
		switch (cRealKey)
		{
			case 'a': iMod3 = 1; iMod4 = 0; break;
			case 'b': iMod3 = 2; iMod4 = 0; break;
			case 'c': iMod3 = 3; iMod4 = 0; break;
			case 'd': iMod3 = 4; iMod4 = 0; break;
			case 'e': iMod3 = 5; iMod4 = 0; break;
			case 'f': iMod3 = 6; iMod4 = 0; break;
			case 'g': iMod3 = 7; iMod4 = 0; break;
			case 'h': iMod3 = 11; iMod4 = 0; break;
			case 'i': iMod3 = 8; iMod4 = 0; break;
			case 'j': iMod3 = 10; iMod4 = 0; break;
			case 'k': iMod3 = 9; iMod4 = 0; break;
			case 'l': iMod3 = 12; iMod4 = 0; break;
			case 'm': iMod3 = 13; iMod4 = 0; break;
			case 'n': iMod3 = 14; iMod4 = 0; break;
			case 'o': iMod3 = 15; iMod4 = 0; break;
			case 'p': iMod3 = 16; iMod4 = 0; break;
			case 'q': iMod3 = 17; iMod4 = 0; break;
			case 'r': iMod3 = 18; iMod4 = 0; break;
			case 's': iMod3 = 18; iMod4 = 1; break;
			case 't': iMod3 = 19; iMod4 = 4; break;
			case 'u': iMod3 = 19; iMod4 = 5; break;
			case 'v': iMod3 = 19; iMod4 = 0; break;
			case 'w': iMod3 = 19; iMod4 = 1; break;
			case 'x': iMod3 = 20; iMod4 = 4; break;
			case 'y': iMod3 = 20; iMod4 = 5; break;
			case 'z': iMod3 = 20; iMod4 = 0; break;
			case 'A': iMod3 = 20; iMod4 = 1; break;
			case 'B': iMod3 = 22; iMod4 = 0; break;
			case 'C': iMod3 = 23; iMod4 = 0; break;
			case 'D': iMod3 = 24; iMod4 = 0; break;
			case 'E': iMod3 = 25; iMod4 = 0; break;
			case 'F': iMod3 = 25; iMod4 = 1; break;
			case 'G': iMod3 = 26; iMod4 = 0; break;
			case 'H': iMod3 = 26; iMod4 = 1; break;
			case 'I': iMod3 = 27; iMod4 = 0; break;
			case 'J': iMod3 = 28; iMod4 = 0; break;
			case 'K': iMod3 = 29; iMod4 = 0; break;
			case 'L': iMod3 = 21; iMod4 = 0; break;
			case 'M': iMod3 = 30; iMod4 = 0; break;
			case 'N': iMod3 = 31; iMod4 = 0; break;
			case '.': iMod3 = 0; iMod4 = 0; break;
		}
	}

	if ((iMod3 != -1) && (iMod4 != -1))
	{
		iModifierA[iCurRoom][iLoc - 1][3] = iMod3;
		iModifierA[iCurRoom][iLoc - 1][4] = iMod4;
		return (1);
	} else { return (0); }
}
/*****************************************************************************/
void ApplyJumpsFrameIfNecessary (int iLoc)
/*****************************************************************************/
{
	if (iThingA[iCurRoom][iLoc - 1] == 11)
	{
		if (iModifierA[iCurRoom][iLoc - 1][1] <= 127)
		{
			iModifierA[iCurRoom][iLoc - 1][1] =
				JumpsAsMod (iChangeJumps) + FrameAsMod (iChangeFrame);
		} else {
			iModifierA[iCurRoom][iLoc - 1][1] =
				JumpsAsMod (iChangeJumps) + FrameAsMod (iChangeFrame) + 128;
		}
		iChanged++;
	}
	if (iThingA[iCurRoom][iLoc - 1] == 36)
	{
		iModifierA[iCurRoom][iLoc - 1][1] = FrameAsMod (iChangeFrame);
		iChanged++;
	}
	if (iThingA[iCurRoom][iLoc - 1] == 12)
	{
		if (iModifierA[iCurRoom][iLoc - 1][1] < 32)
		{
			iModifierA[iCurRoom][iLoc - 1][1] = FrameAsMod (iChangeFrame) + 0;
		} else if (iModifierA[iCurRoom][iLoc - 1][1] < 128) {
			iModifierA[iCurRoom][iLoc - 1][1] = FrameAsMod (iChangeFrame) + 32;
		} else if (iModifierA[iCurRoom][iLoc - 1][1] < 160) {
			iModifierA[iCurRoom][iLoc - 1][1] = FrameAsMod (iChangeFrame) + 128;
		} else {
			iModifierA[iCurRoom][iLoc - 1][1] = FrameAsMod (iChangeFrame) + 160;
		}
		iChanged++;
	}
}
/*****************************************************************************/
void ApplyPercentIfNecessary (int iLoc)
/*****************************************************************************/
{
	if (iThingA[iCurRoom][iLoc - 1] == 4)
	{
		iModifierA[iCurRoom][iLoc - 1][1] = PercentAsMod (iChangePercent, 4);
		iChanged++;
	}
	if (iThingA[iCurRoom][iLoc - 1] == 17)
	{
		iModifierA[iCurRoom][iLoc - 1][1] = PercentAsMod (iChangePercent, 17);
		iChanged++;
	}
}
/*****************************************************************************/
void ApplyWallIfNecessary (int iLoc)
/*****************************************************************************/
{
	int iThing, iModifier;
	int iMod;

	iThing = iThingA[iCurRoom][iLoc - 1];
	iModifier = iModifierA[iCurRoom][iLoc - 1][1];

	if (((cCurType == 'c') && (iThing == 20)) ||
		((cCurType == 'r') && (iThing == 20)) ||
		((cCurType == 't') && ((iThing == 20) || (iThing == 25))))
	{
		if (cCurType == 'c')
		{
			if (iModifier >= 128)
				{ iMod = iModifier - 128; }
					else { iMod = iModifier; }
		}
		if (cCurType == 'r')
		{
			if ((iModifier >= 0) && (iModifier <= 3)) { iMod = iModifier; }
			if ((iModifier >= 4) && (iModifier <= 7)) { iMod = iModifier - 4; }
			if ((iModifier >= 8) && (iModifier <= 11)) { iMod = iModifier - 8; }
			if ((iModifier >= 12) && (iModifier <= 15)) { iMod = iModifier - 12; }
		}
		if (cCurType == 't')
		{
			iMod = 0; /*** Anything will do. ***/
		}
		iModifierA[iCurRoom][iLoc - 1][1] = WallAsMod (iChangeWall, iMod);
		iChanged++;
	}
}
/*****************************************************************************/
void ExtractFromMod2 (int iMod2, char cCrackBot, char cSymbolStal)
/*****************************************************************************/
{
	int iMod;

	iMod = iMod2;

	if (cCurType == 'c')
	{
		if (cSymbolStal == 's')
		{
			if ((iMod >= 0) && (iMod < 8)) { iChangeStal = 0; }
			if ((iMod >= 8) && (iMod < 16)) { iChangeStal = 1; }
			if ((iMod >= 16) && (iMod < 24)) { iChangeStal = 2; }
			if ((iMod >= 24) && (iMod < 32)) { iChangeStal = 3; }
			if ((iMod >= 32) && (iMod < 40)) { iChangeStal = 4; }
			if ((iMod >= 40) && (iMod < 48)) { iChangeStal = 5; }
			if ((iMod >= 48) && (iMod < 56)) { iChangeStal = 6; }
			if ((iMod >= 56) && (iMod < 64)) { iChangeStal = 7; }
		} /*** Do not add: else { iChangeStal = 0; } ***/
		if (cCrackBot == 'b')
		{
			while (iMod >= 8) { iMod-=8; }
			iChangeBot = iMod;
		} /*** Do not add: else { iChangeBot = 0; } ***/
	}
	if (cCurType == 'r')
	{
		if (cSymbolStal == 's')
		{
			if ((iMod >= 0) && (iMod < 16)) { iChangeSym = 0; }
			if ((iMod >= 16) && (iMod < 32)) { iChangeSym = 1; }
			if ((iMod >= 32) && (iMod < 48)) { iChangeSym = 2; }
			if ((iMod >= 48) && (iMod < 64)) { iChangeSym = 3; }
		} /*** Do not add: else { iChangeSym = 0; } ***/
		if (cCrackBot == 'c')
		{
			while (iMod >= 16) { iMod-=16; }
			iChangeCrk = iMod;
			if ((iChangeCrk == 2) || (iChangeCrk == 13) ||
				(iChangeCrk == 14) || (iChangeCrk == 15))
			{
				printf ("[ WARN ] Strange crack value: %i\n", iChangeCrk);
				iChangeCrk = 0;
			}
		} /*** Do not add: else { iChangeCrk = 0; } ***/
	}
}
/*****************************************************************************/
int ExtractedAsMod2 (char cCrackBot, char cSymbolStal)
/*****************************************************************************/
{
	int iMod;

	iMod = 0;

	if (cCurType == 'c')
	{
		if (cSymbolStal == 's')
		{
			switch (iChangeStal)
			{
				case 0: iMod+=0; break;
				case 1: iMod+=8; break;
				case 2: iMod+=16; break;
				case 3: iMod+=24; break;
				case 4: iMod+=32; break;
				case 5: iMod+=40; break;
				case 6: iMod+=48; break;
				case 7: iMod+=56; break;
			}
		}
		if (cCrackBot == 'b') { iMod+=iChangeBot; }
	}
	if (cCurType == 'r')
	{
		if (cSymbolStal == 's')
		{
			switch (iChangeSym)
			{
				case 0: iMod+=0; break;
				case 1: iMod+=16; break;
				case 2: iMod+=32; break;
				case 3: iMod+=48; break;
			}
		}
		if (cCrackBot == 'c') { iMod+=iChangeCrk; }
	}

	return (iMod);
}
/*****************************************************************************/
void UpdateMod2IfNecessary (int iLoc)
/*****************************************************************************/
{
	int iNMod;

	iNMod = -1;

	if (cCurType == 'c')
	{
		switch (iThingA[iCurRoom][iLoc - 1])
		{
			case 0: iNMod = ExtractedAsMod2 (' ', 's'); break;
			case 1: iNMod = ExtractedAsMod2 ('b', 's'); break;
			case 2: iNMod = ExtractedAsMod2 ('b', ' '); break;
			case 3: iNMod = ExtractedAsMod2 ('b', 's'); break;
			case 4: iNMod = ExtractedAsMod2 ('b', 's'); break;
			case 8: iNMod = ExtractedAsMod2 ('b', 's'); break;
			case 9: iNMod = ExtractedAsMod2 (' ', 's'); break;
			case 10: iNMod = ExtractedAsMod2 ('b', 's'); break;
			case 11: iNMod = ExtractedAsMod2 ('b', ' '); break;
			case 14: iNMod = ExtractedAsMod2 ('b', ' '); break;
			case 16: iNMod = ExtractedAsMod2 ('b', 's'); break;
			case 17: iNMod = ExtractedAsMod2 ('b', 's'); break;
			case 18: iNMod = ExtractedAsMod2 (' ', ' '); break;
			case 19: iNMod = ExtractedAsMod2 ('b', 's'); break;
			case 20: iNMod = ExtractedAsMod2 ('b', ' '); break;
			case 23: iNMod = ExtractedAsMod2 ('b', 's'); break;
			case 24: iNMod = ExtractedAsMod2 ('b', 's'); break;
			case 28: iNMod = ExtractedAsMod2 (' ', ' '); break;
			case 32: iNMod = ExtractedAsMod2 ('b', 's'); break;
			case 33: iNMod = ExtractedAsMod2 (' ', 's'); break;
			case 34:
				if ((iModifierA[iCurRoom][iLoc - 1][2] >= 0) &&
					(iModifierA[iCurRoom][iLoc - 1][2] <= 7))
				{
					iNMod = ExtractedAsMod2 ('b', ' ') + 0;
				}
				if ((iModifierA[iCurRoom][iLoc - 1][2] >= 8) &&
					(iModifierA[iCurRoom][iLoc - 1][2] <= 15))
				{
					iNMod = ExtractedAsMod2 ('b', ' ') + 8;
				}
				break;
			case 35: iNMod = ExtractedAsMod2 (' ', 's'); break;
			case 36: iNMod = ExtractedAsMod2 ('b', 's'); break;
			case 44: iNMod = ExtractedAsMod2 (' ', ' '); break;
		}
	}

	if (cCurType == 'r')
	{
		switch (iThingA[iCurRoom][iLoc - 1])
		{
			case 0: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 1: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 2: iNMod = ExtractedAsMod2 (' ', ' '); break;
			case 3: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 4: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 5:
				if ((iModifierA[iCurRoom][iLoc - 1][2] >= 0) &&
					(iModifierA[iCurRoom][iLoc - 1][2] <= 7))
				{
					iNMod = ExtractedAsMod2 (' ', ' ') + 0;
				}
				if ((iModifierA[iCurRoom][iLoc - 1][2] >= 8) &&
					(iModifierA[iCurRoom][iLoc - 1][2] <= 15))
				{
					iNMod = ExtractedAsMod2 (' ', ' ') + 8;
				}
				break;
			case 6:
				if ((iModifierA[iCurRoom][iLoc - 1][2] >= 0) &&
					(iModifierA[iCurRoom][iLoc - 1][2] <= 7))
				{
					iNMod = ExtractedAsMod2 (' ', ' ') + 0;
				}
				if ((iModifierA[iCurRoom][iLoc - 1][2] >= 8) &&
					(iModifierA[iCurRoom][iLoc - 1][2] <= 15))
				{
					iNMod = ExtractedAsMod2 (' ', ' ') + 8;
				}
				break;
			case 7: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 8: iNMod = ExtractedAsMod2 ('c', ' '); break;
			case 9: iNMod = ExtractedAsMod2 ('c', ' '); break;
			case 10: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 11: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 12: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 13: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 14: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 15: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 16: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 17: iNMod = ExtractedAsMod2 (' ', 's'); break;
			case 18: iNMod = ExtractedAsMod2 (' ', ' '); break;
			case 20: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 21: iNMod = ExtractedAsMod2 ('c', 's'); break;
			case 22: iNMod = ExtractedAsMod2 ('c', 's'); break;
		}
	}

	if (iNMod != -1)
	{
		iModifierA[iCurRoom][iLoc - 1][2] = iNMod;
		iChanged++;
	}
}
/*****************************************************************************/
