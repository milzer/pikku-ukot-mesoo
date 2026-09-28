#include "DieselSurface.h"
#include "pathcompat.h"

#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_BMP
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_TGA
#include "stb_image.h"

// Clips a copy of rcSrc (whole source when NULL) placed at (iX, iY) against both
// surfaces. Returns false when nothing is left to copy.
static bool ClipCopy(LONG& iX, LONG& iY, RECT& rc, const RECT* prcSrc,
                     LONG iSrcW, LONG iSrcH, LONG iDstW, LONG iDstH)
{
    rc = prcSrc ? *prcSrc : RECT{0, 0, iSrcW, iSrcH};

    if (rc.left < 0)
    {
        iX -= rc.left;
        rc.left = 0;
    }
    if (rc.top < 0)
    {
        iY -= rc.top;
        rc.top = 0;
    }
    if (iX < 0)
    {
        rc.left -= iX;
        iX = 0;
    }
    if (iY < 0)
    {
        rc.top -= iY;
        iY = 0;
    }

    rc.right = std::min<LONG>(rc.right, iSrcW);
    rc.bottom = std::min<LONG>(rc.bottom, iSrcH);
    rc.right = std::min<LONG>(rc.right, rc.left + iDstW - iX);
    rc.bottom = std::min<LONG>(rc.bottom, rc.top + iDstH - iY);

    return rc.right > rc.left && rc.bottom > rc.top;
}

CDieselSurface::CDieselSurface()
    : m_iWidth(0), m_iHeight(0), m_dwColorKey(0)
{
}

CDieselSurface::~CDieselSurface()
{
}

DE_RETVAL CDieselSurface::Create(SDE_SURFACEDESC& sDesc)
{
    m_iWidth = sDesc.iWidth;
    m_iHeight = sDesc.iHeight;
    m_Pixels.assign((size_t)m_iWidth * m_iHeight, 0);
    return DE_OK;
}

DE_RETVAL CDieselSurface::Load(LPCTSTR pcszFilename, SDE_SURFACEDESC* psDesc)
{
    int w, h, n;
    stbi_uc* rgb = stbi_load(ResolvePath(pcszFilename).c_str(), &w, &h, &n, 3);
    if (!rgb)
    {
        return DE_FILENOTFOUND;
    }

    CDieselSurface image;
    SDE_SURFACEDESC desc = {DE_SYSTEMMEMORY, eDE_COMPATIBLE, w, h};
    image.Create(desc);

    for (int i = 0; i < w * h; i++)
    {
        image.m_Pixels[i] = (rgb[i * 3] << 16) | (rgb[i * 3 + 1] << 8) | rgb[i * 3 + 2];
    }
    stbi_image_free(rgb);

    // A requested size stretches the image, otherwise it is used as is
    if (psDesc && psDesc->iWidth > 0 && psDesc->iHeight > 0 && (psDesc->iWidth != w || psDesc->iHeight != h))
    {
        desc.iWidth = psDesc->iWidth;
        desc.iHeight = psDesc->iHeight;
        Create(desc);
        return Blt(NULL, &image, NULL, 0);
    }

    *this = std::move(image);
    return DE_OK;
}

DE_RETVAL CDieselSurface::LoadFromResource(DWORD dwResource, LPCTSTR pcszResType, SDE_SURFACEDESC* psDesc)
{
    for (const SDE_RESOURCE* r = g_DieselResources; r->pcszFilename; r++)
    {
        if (r->dwId == dwResource && strcmp(r->pcszType, pcszResType) == 0)
        {
            return Load(r->pcszFilename, psDesc);
        }
    }
    return DE_FILENOTFOUND;
}

void CDieselSurface::Release()
{
    m_Pixels.clear();
    m_Pixels.shrink_to_fit();
    m_iWidth = m_iHeight = 0;
}

void* CDieselSurface::Lock(RECT* prcLock)
{
    if (prcLock)
    {
        return &m_Pixels[(size_t)prcLock->top * m_iWidth + prcLock->left];
    }
    return m_Pixels.data();
}

void CDieselSurface::Unlock()
{
}

DE_RETVAL CDieselSurface::Blt(RECT* prcDest, CDieselSurface* psrfSource, RECT* prcSrc, DWORD dwFlags)
{
    RECT rcDst = prcDest ? *prcDest : RECT{0, 0, m_iWidth, m_iHeight};
    RECT rcSrc = prcSrc ? *prcSrc : RECT{0, 0, psrfSource->m_iWidth, psrfSource->m_iHeight};
    LONG iDstW = rcDst.right - rcDst.left;
    LONG iDstH = rcDst.bottom - rcDst.top;
    LONG iSrcW = rcSrc.right - rcSrc.left;
    LONG iSrcH = rcSrc.bottom - rcSrc.top;
    DWORD key = psrfSource->m_dwColorKey;

    for (LONG y = std::max<LONG>(rcDst.top, 0); y < std::min<LONG>(rcDst.bottom, m_iHeight); y++)
    {
        LONG sy = rcSrc.top + (y - rcDst.top) * iSrcH / iDstH;
        const DWORD* src = &psrfSource->m_Pixels[(size_t)sy * psrfSource->m_iWidth];
        DWORD* dst = &m_Pixels[(size_t)y * m_iWidth];

        for (LONG x = std::max<LONG>(rcDst.left, 0); x < std::min<LONG>(rcDst.right, m_iWidth); x++)
        {
            DWORD c = src[rcSrc.left + (x - rcDst.left) * iSrcW / iDstW];
            if (!(dwFlags & DE_BLTSRCCOLORKEY) || c != key)
            {
                dst[x] = c;
            }
        }
    }
    return DE_OK;
}

DE_RETVAL CDieselSurface::BltFast(LONG iX, LONG iY, CDieselSurface* psrfSource, RECT* prcSrc, DWORD dwFlags)
{
    RECT rc;
    if (!ClipCopy(iX, iY, rc, prcSrc, psrfSource->m_iWidth, psrfSource->m_iHeight, m_iWidth, m_iHeight))
    {
        return DE_OK;
    }

    DWORD key = psrfSource->m_dwColorKey;

    for (LONG y = rc.top; y < rc.bottom; y++)
    {
        const DWORD* src = &psrfSource->m_Pixels[(size_t)y * psrfSource->m_iWidth + rc.left];
        DWORD* dst = &m_Pixels[(size_t)(iY + y - rc.top) * m_iWidth + iX];

        for (LONG x = 0; x < rc.right - rc.left; x++)
        {
            if (!(dwFlags & DE_BLTSRCCOLORKEY) || src[x] != key)
            {
                dst[x] = src[x];
            }
        }
    }
    return DE_OK;
}

// Blends the source over this surface: dst = src * fMul + dst * (1 - fMul)
DE_RETVAL CDieselSurface::AlphaBlend(LONG iX, LONG iY, CDieselSurface* psrfSource, RECT* prcSource, float fMul)
{
    RECT rc;
    if (!ClipCopy(iX, iY, rc, prcSource, psrfSource->m_iWidth, psrfSource->m_iHeight, m_iWidth, m_iHeight))
    {
        return DE_OK;
    }

    int a = (int)(std::clamp(fMul, 0.0f, 1.0f) * 256.0f);

    for (LONG y = rc.top; y < rc.bottom; y++)
    {
        const DWORD* src = &psrfSource->m_Pixels[(size_t)y * psrfSource->m_iWidth + rc.left];
        DWORD* dst = &m_Pixels[(size_t)(iY + y - rc.top) * m_iWidth + iX];

        for (LONG x = 0; x < rc.right - rc.left; x++)
        {
            int sr = (src[x] >> 16) & 0xff, sg = (src[x] >> 8) & 0xff, sb = src[x] & 0xff;
            int dr = (dst[x] >> 16) & 0xff, dg = (dst[x] >> 8) & 0xff, db = dst[x] & 0xff;

            dr += ((sr - dr) * a) >> 8;
            dg += ((sg - dg) * a) >> 8;
            db += ((sb - db) * a) >> 8;

            dst[x] = (dr << 16) | (dg << 8) | db;
        }
    }
    return DE_OK;
}

DE_RETVAL CDieselSurface::SetPixel(int iX, int iY, DWORD dwColor)
{
    if (iX < 0 || iY < 0 || iX >= m_iWidth || iY >= m_iHeight)
    {
        return DE_INVALIDPARAMS;
    }
    m_Pixels[(size_t)iY * m_iWidth + iX] = dwColor;
    return DE_OK;
}

DWORD CDieselSurface::GetPixel(LONG iX, LONG iY)
{
    if (iX < 0 || iY < 0 || iX >= m_iWidth || iY >= m_iHeight)
    {
        return 0;
    }
    return m_Pixels[(size_t)iY * m_iWidth + iX];
}

void CDieselSurface::SetColorKey(DWORD dwKey)
{
    m_dwColorKey = dwKey;
}
