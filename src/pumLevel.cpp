#include "pumLevel.h"

CPumLevel::~CPumLevel()
{
    if (m_szMask != NULL)
        delete[] m_szMask;
    m_srfGfx.Release();
    m_srfBackground.Release();
    m_srfGfxRegen.Release();
}

CPumLevel::CPumLevel()
{
    m_szMask = NULL;
}

int CPumLevel::Load(const char *f)
{
    SDE_SURFACEDESC tmp;
    DE_RETVAL       res = DE_OK;
    DWORD           *ptr;
    FILE            *fp;

    tmp.dwFlags = DE_SYSTEMMEMORY;
    tmp.eFormat = eDE_COMPATIBLE;
    tmp.iWidth = 320;
    tmp.iHeight = 200;

    res = m_srfGfx.Create(tmp);
    if (res != DE_OK) return(res);

    res = m_srfGfxRegen.Create(tmp);
    if (res != DE_OK) return(res);

    res = m_srfBackground.Create(tmp);
    if (res != DE_OK) return(res);

    m_szMask = new CHAR[tmp.iHeight * tmp.iWidth];
    if (m_szMask == NULL) return(DE_OUTOFMEMORY);

    fp = fopen(f, "rb");
    if (!fp)
        return(DE_OK + 1);

    ptr = (DWORD *) m_srfGfx.Lock(NULL);
    fread((DWORD *)ptr, 64000*sizeof(DWORD), 1, fp);
    m_srfGfx.Unlock();
    m_srfGfx.SetColorKey(0x00000000);

    fseek(fp, 0, SEEK_SET);

    ptr = (DWORD *) m_srfGfxRegen.Lock(NULL);
    fread((DWORD *)ptr, 64000*sizeof(DWORD), 1, fp);
    m_srfGfxRegen.Unlock();

    ptr = (DWORD *) m_srfBackground.Lock(NULL);
    fread((DWORD *)ptr, 64000*sizeof(DWORD), 1, fp);
    m_srfBackground.Unlock();

    fread((CHAR *)m_szMask, 64000, 1, fp);

    fclose(fp);

    return(DE_OK);
}

int CPumLevel::Load(const char *fgfx, const char *fbg)
{
    SDE_SURFACEDESC tmp;
    DE_RETVAL       res = DE_OK;
    int             x,
    y;


    tmp.dwFlags = DE_SYSTEMMEMORY;
    tmp.eFormat = eDE_COMPATIBLE;
    tmp.iWidth = 320;
    tmp.iHeight = 200;

    res = m_srfGfx.Load(fgfx, &tmp);
    if (res != DE_OK) return(res);
    m_srfGfx.SetColorKey(0x00000000);

    res = m_srfGfxRegen.Load(fgfx, &tmp);
    if (res != DE_OK) return(res);

    res = m_srfBackground.Load(fbg, &tmp);
    if (res != DE_OK) return(res);

    m_szMask = new CHAR[tmp.iHeight * tmp.iWidth];
    if (m_szMask == NULL) return(DE_OUTOFMEMORY);

    m_srfGfx.Lock(NULL);
    for (y = 0; y < tmp.iHeight; y++)
        for (x = 0; x < tmp.iWidth; x++) {
            if (m_srfGfx.GetPixel(x, y) > 0) m_szMask[ y * tmp.iWidth + x ] = (CHAR)0xff;
            else m_szMask[ y * tmp.iWidth + x ] = (CHAR)0x00;
        }
    m_srfGfx.Unlock();

    return(DE_OK);
}

int CPumLevel::Save(const char *f)
{
    DWORD   *ptr;
    FILE    *fp;
    fp = fopen(f, "wb");
    if (!fp)
        return(DE_OK + 1);

    ptr = (DWORD *) m_srfGfx.Lock(NULL);
    fwrite((DWORD *)ptr, 64000*sizeof(DWORD), 1, fp);
    m_srfGfx.Unlock();

    ptr = (DWORD *) m_srfBackground.Lock(NULL);
    fwrite((DWORD *)ptr, 64000*sizeof(DWORD), 1, fp);
    m_srfBackground.Unlock();

    fwrite((CHAR *)m_szMask, 64000*sizeof(CHAR), 1, fp);

    fclose(fp);

    return(DE_OK);
}

void CPumLevel::Regenerate(DWORD *ptr, int rspd, int vspd)
{
    int r, g, b, rt, gt, bt;
    int p, i, y;
    DWORD *regen;
    regen = (DWORD *)m_srfGfxRegen.Lock(NULL);

    for (i = 0; i < rspd; i++) {
        p = 63680 + RandInt(0, 319);

        for (y = 0; y < 200; y++) {
            if (regen[ p ] > 0 && ptr[ p ] == 0) {
                ptr[ p ] = regen[ p ];

                m_szMask[ p ] = (char)0xff;
                y = 200;
            }

            p -= 320;
        }
    }

    for (i = 0; i < vspd; i++) {
        p = (RandInt(0, 199)) * ScreenWidth + (RandInt(0, 319));

        if (ptr[ p ] > 0)
            if (regen[ p ] != ptr[ p ]) {
                rt = (regen[ p ] & 0x00ff0000) >> 16;
                gt = (regen[ p ] & 0x0000ff00) >> 8;
                bt = regen[ p ] & 0x000000ff;
                r = (ptr[ p ] & 0x00ff0000) >> 16;
                g = (ptr[ p ] & 0x0000ff00) >> 8;
                b = ptr[ p ] & 0x000000ff;

                if (r < rt - 15) r += 8;
                else if (r > rt + 15) r -= 16;
                else r = rt;

                if (g < gt - 15) g += 8;
                else if (g > gt + 15) g -= 16;
                else g = gt;

                if (b < bt - 15) b += 16;
                else if (b > bt + 15) b -= 16;
                else b = bt;

                ptr[ p ] = (r << 16) | (g << 8) | b;

                m_szMask[ p ] = (char)0xff;
                y = 200;
            }
    }

    m_srfGfxRegen.Unlock();
}