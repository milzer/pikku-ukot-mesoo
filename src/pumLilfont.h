#ifndef __PUMLILFONT_H__
#define __PUMLILFONT_H__

#include "pumGlobals.h"
#include <DieselSurface.h>

#define _OnScreen(x,y) ( (x)>=0 && (x)<ScreenWidth && (y)>=0 && (y)<ScreenHeight )

extern unsigned char szLilFontData[1280];

void LilChar(int x, int y, char c, DWORD col, DWORD *pSrf);
void LilString(int x, int y, char *s, DWORD col, DWORD *pSrf);
void LilStringR(int x, int y, char *s, DWORD col, DWORD *pSrf);
void LilStringCenterX(int y, char *s, DWORD col, DWORD *pSrf);
void LilStringCenterXo(int y, char *s, DWORD col, DWORD *pSrf);
void LilStringCentered(int x, int y, char *s, DWORD col, DWORD *pSrf);

#endif