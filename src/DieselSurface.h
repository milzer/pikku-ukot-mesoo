#ifndef DIESELSURFACE_H
#define DIESELSURFACE_H

#include <windows.h>

class CDieselSurface
{
public:
    CDieselSurface();
    ~CDieselSurface();

    DE_RETVAL AlphaBlend(LONG iX, LONG iY, CDieselSurface *psrfSource, RECT *prcSource, float fMul);
    DE_RETVAL Blt(RECT *prcDest, CDieselSurface *psrfSource, RECT *prcSrc, DWORD dwFlags);
    DE_RETVAL BltFast(LONG iX, LONG iY, CDieselSurface *pSrc, RECT *prcSrc, DWORD dwFlags);
    DE_RETVAL Create(SDE_SURFACEDESC &sDesc);
    DE_RETVAL SetPixel(int iX, int iY, DWORD dwColor);
    DWORD GetPixel(LONG iX, LONG iY);
    DE_RETVAL Load(LPCTSTR pcszFilename, SDE_SURFACEDESC *psDesc);
    DE_RETVAL LoadFromResource(DWORD dwResource, LPCTSTR pcszResType, SDE_SURFACEDESC *psDesc);
    void SetColorKey(DWORD dwKey);
    // Lock, Unlock, Release not needed

private:
    // Private member variables and functions
    LPDIRECTDRAWSURFACE m_pSurface;
    RECT m_rcClip;
    LONG m_iSubFrameWidth;
    LONG m_iSubFrameHeight;
    LONG m_iSubFrameXCount;
    LONG m_iSubFrameYCount;
};

#endif // DIESELSURFACE_H