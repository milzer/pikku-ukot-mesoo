#ifndef __PUMLEVEL_H__
#define __PUMLEVEL_H__

#include <DieselSurface.h>
#include "pumGlobals.h"

class   CPumLevel
{
public:
    CDieselSurface  m_srfGfx,
    m_srfBackground,
    m_srfGfxRegen;

    CHAR            *m_szMask;

    void            Regenerate(DWORD *ptr, int rspd, int vspd);
    int             Load(const char *fgfx, const char *fbg);
    int             Load(const char *f);
    int             Save(const char *f);

    CPumLevel();
    ~CPumLevel();
};

#endif