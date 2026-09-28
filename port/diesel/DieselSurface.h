// Software surface with 32-bit X8R8G8B8 pixels (high byte zero), matching what
// DirectDraw gave the game in a 32bpp display mode. Lock() returns a tightly
// packed buffer (pitch == width), which the game code relies on.

#ifndef __DIESELSURFACE_H__
#define __DIESELSURFACE_H__

#include <vector>
#include "DieselTypes.h"

enum EDE_FORMAT
{
    eDE_COMPATIBLE,
};

// SDE_SURFACEDESC::dwFlags
#define DE_SYSTEMMEMORY     0x0001
#define DE_VIDEOMEMORY      0x0002

// Blt flags
#define DE_BLTSRCCOLORKEY   0x0001

struct SDE_SURFACEDESC
{
    DWORD dwFlags;
    EDE_FORMAT eFormat;
    LONG iWidth;
    LONG iHeight;
};

class CDieselSurface
{
public:
    CDieselSurface();
    ~CDieselSurface();

    DE_RETVAL Create(SDE_SURFACEDESC& sDesc);
    DE_RETVAL Load(LPCTSTR pcszFilename, SDE_SURFACEDESC* psDesc);
    DE_RETVAL LoadFromResource(DWORD dwResource, LPCTSTR pcszResType, SDE_SURFACEDESC* psDesc);
    void Release();

    void* Lock(RECT* prcLock);
    void Unlock();

    DE_RETVAL Blt(RECT* prcDest, CDieselSurface* psrfSource, RECT* prcSrc, DWORD dwFlags);
    DE_RETVAL BltFast(LONG iX, LONG iY, CDieselSurface* psrfSource, RECT* prcSrc, DWORD dwFlags);
    DE_RETVAL AlphaBlend(LONG iX, LONG iY, CDieselSurface* psrfSource, RECT* prcSource, float fMul);

    DE_RETVAL SetPixel(int iX, int iY, DWORD dwColor);
    DWORD GetPixel(LONG iX, LONG iY);
    void SetColorKey(DWORD dwKey);

    LONG GetWidth() const
    {
        return m_iWidth;
    }
    LONG GetHeight() const
    {
        return m_iHeight;
    }

private:
    std::vector<DWORD> m_Pixels;
    LONG m_iWidth;
    LONG m_iHeight;
    DWORD m_dwColorKey;
};

// Resource table, provided by the application in place of a Win32 .rc script
struct SDE_RESOURCE
{
    DWORD dwId;
    LPCTSTR pcszType;
    LPCTSTR pcszFilename;
};

extern const SDE_RESOURCE g_DieselResources[];

#endif
