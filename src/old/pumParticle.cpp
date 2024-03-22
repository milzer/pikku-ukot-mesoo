#include <stdlib.h>
#include "pumParticle.h"
#include "pumPlayer.h"

extern float globalMatrixTime;

DWORD ParticlePalette[256];

CPumParticle::CPumParticle()
{
    m_fXpos = 0;
    m_fYpos = 0;
    m_Acceleration = 0;
    m_Misc = 0;
    m_Vec.Set(0.0f, 0.0f);
    m_iXpos = 0;
    m_iYpos = 0;
    m_Flags = 0;
    m_TTL = 0;
    m_Counter = 0;
    m_Countfreq = 0;
    m_Col = 0;
    m_Onscreen = 0;
}

inline void CPumParticle::Kill()
{
    m_fXpos = 0;
    m_fYpos = 0;
    m_Acceleration = 0;
    m_Misc = 0;
    m_Vec.Set(0.0f, 0.0f);
    m_iXpos = 0;
    m_iYpos = 0;
    m_Flags = 0;
    m_TTL = 0;
    m_Counter = 0;
    m_Countfreq = 0;
    m_Col = 0;
    m_Onscreen = 0;
}

inline void CPumParticle::Create(float x, float y, float i, float j, float accel, int lt, const BYTE col, int fl,
                                 CPumSysPointers* sp)
{
    if (y < 1.0f)
    {
        return;
    }
    if (y > 198.0f)
    {
        return;
    }
    if (x < 1.0f)
    {
        return;
    }
    if (x > 318.0f)
    {
        return;
    }

    m_SP = sp;
    m_fXpos = x;
    m_fYpos = y;
    m_Vec.Set(i, j);
    m_Acceleration = accel;
    m_TTL = lt;
    m_Col = col;
    m_Flags = fl;

    if (m_Flags & PRT_GIB)
    {
        m_Countfreq = (rand() & 15) - 4;
        m_Misc = (float)(rand() & 127) / 630.0f; // 653;
    }
}


inline int CPumParticle::Update(float frameTime)
{
    CHAR pix;

    float fXOpos = m_fXpos,
            fYOpos = m_fYpos;

    m_TTL--;

    if (m_Flags & PRT_GRAVITY)
    {
        m_Vec.y += m_SP->Options->Gravity * frameTime;
    }

    if (m_Flags & PRT_XFRICTION)
    {
        m_Vec.x *= m_Acceleration;
    }
    if (m_Flags & PRT_YFRICTION)
    {
        m_Vec.y *= m_Acceleration;
    }

    m_fXpos += m_Vec.x * frameTime;
    m_fYpos += m_Vec.y * frameTime;

    RoundToInt(&m_iXpos, m_fXpos);
    RoundToInt(&m_iYpos, m_fYpos);

    if (m_iXpos < ScreenWidth - 1 && m_iXpos > 0 && m_iYpos < ScreenHeight - 1 && m_iYpos > 0)
    {
        m_Onscreen = 1;
        pix = m_SP->Level->m_szMask[m_iYpos * ScreenWidth + m_iXpos];
    }
    else
    {
        m_Onscreen = 0;
    }

    m_TTL *= m_Onscreen;

    if (m_TTL < 1)
    {
        return (0);
    }

    if (m_Onscreen)
    {
        if (pix)
        {
            if (m_Flags & PRT_DIEONHIT)
            {
                m_TTL = 0;
            }

            if (m_Flags & PRT_DRAWBACK)
            {
                m_SP->Level->m_srfGfx.SetPixel(m_iXpos, m_iYpos, ParticlePalette[m_Col]);
            }

            if (m_Flags & PRT_FIRE)
            {
                if (rand() & 1)
                {
                    if (m_SP->Level->m_szMask[(m_iYpos + 1) * ScreenWidth + (m_iXpos - 1)] == 0)
                    {
                        if ((rand() & 1))
                        {
                            m_iYpos += 1;
                        }
                        else
                        {
                            m_iXpos -= 1;
                        }

                        m_iYpos += 1;
                        m_fYpos = (float)m_iYpos;
                        m_fXpos = (float)m_iXpos;
                    }
                    else
                    {
                        m_TTL = 0;
                    }

                }
                else
                {
                    if (m_SP->Level->m_szMask[(m_iYpos + 1) * ScreenWidth + (m_iXpos + 1)] == 0)
                    {
                        m_iYpos += 1;
                        m_iXpos += 1;
                        m_fYpos = (float)m_iYpos;
                        m_fXpos = (float)m_iXpos;
                    }
                    else
                    {
                        m_TTL = 0;
                    }
                }
            }

            if (m_Flags & PRT_BOUNCE)
            {
                if (m_Flags & PRT_BLOOD)
                {
                    if (!(rand() & 15))
                    {
                        m_Vec.Set(0.0f, 0.0f);
                    }
                    m_Flags |= PRT_DIEONHIT;
                }

                if (m_Flags & PRT_GIB)
                {
                    if (m_Vec.Length() > 0.9f * TimeFactor)
                        if (!(rand() & 3))
                        {
                            m_SP->Sound->PlaySample(8, 10, m_iXpos);
                        }

                    WallBounce(&m_Vec, m_iXpos, m_iYpos, m_SP->Level->m_szMask, frameTime);
                    m_Vec.Mul(0.3f);
                    if (m_Vec.Length() < 0.1f * TimeFactor)
                    {
                        m_Flags |= (PRT_DIEONHIT | PRT_DRAWBACK);
                    }
                    m_Countfreq += rand() & 7;
                    m_Misc = (float)(rand() & 127) / 630.0f; // 653;
                }

                if (m_Flags & PRT_SPARK)
                {
                    WallBounce(&m_Vec, m_iXpos, m_iYpos, m_SP->Level->m_szMask, frameTime);
                    m_Vec.Mul(RandFloat(0.00f, 0.07f)); // 0.05f
                    m_TTL -= 20;
                }

                if (m_Flags & PRT_FIREEMIT)
                {
                    m_Vec.Set(0.0f, 0.0f);
                }

                if (m_Flags & PRT_FASTSPARK)
                {
                    WallBounce(&m_Vec, m_iXpos, m_iYpos, m_SP->Level->m_szMask, frameTime);
                }

                if (m_Flags & PRT_DEBRIS)
                {
                    WallBounce(&m_Vec, m_iXpos, m_iYpos, m_SP->Level->m_szMask, frameTime);
                }

                m_fXpos = fXOpos;
                m_fYpos = fYOpos;
                RoundToInt(&m_iXpos, m_fXpos);
                RoundToInt(&m_iYpos, m_fYpos);
            }

            if (m_Flags & PRT_DIGGER)
            {
                if (m_iYpos > 1 && m_iYpos < ScreenHeight - 2 && m_iXpos > 1 && m_iXpos < ScreenWidth - 2)
                {
                    m_SP->Level->m_srfGfx.SetPixel(m_iXpos, m_iYpos, 0);
                    m_SP->Level->m_szMask[m_iYpos * ScreenWidth + m_iXpos] = 0;
                }
            }
        }
    }

    if (m_Flags & PRT_FIREEMIT)
        if (!(rand() & 3))
        {
            SpawnFlame(m_fXpos, m_fYpos, m_SP);
        }

    if (m_Flags & PRT_FIRE)
    {
        m_Vec.x = cosinus[rand() & 255] * TimeFactor / 4 * frameTime;
        m_Vec.y -= 0.04f * TimeFactor;
        m_Col = 217 - (m_TTL >> 1);
    }

    if (m_Flags & PRT_GIB)
    {
        m_Counter += m_Countfreq;
        if (!(rand() & 3))
        {
            SpawnParticles(_PRT_BLOOD, m_fXpos, m_fYpos, (float)m_Counter, 2.0f, (float)m_Misc * TimeFactor,
                           (float)m_Misc * TimeFactor, 1.0f, 1, 0, 0, m_SP);
        }
    }

    if (m_Flags & PRT_DEBRIS)
    {
        if (!(rand() & 3))
        {
            SpawnParticles(_PRT_SPARK, m_fXpos, m_fYpos, (float)(rand() & 255), 0.0f, 0.0f, 0.5f * TimeFactor, 1.0f, 1, 0, 0,
                           m_SP);
        }
    }


    if (m_Flags & PRT_SPARK)
    {
        m_Col = 217 - (m_TTL >> 4);
    }

    if (m_Flags & PRT_FASTSPARK)
    {
        m_Col = 217 - (m_TTL << 1);
    }

    return (m_TTL);
}


inline void CPumParticle::Draw(DWORD* pSurface)
{
    if (m_iYpos < 1)
    {
        return;
    }
    if (m_iYpos > ScreenHeight - 2)
    {
        return;
    }
    if (m_iXpos < 1)
    {
        return;
    }
    if (m_iXpos > ScreenWidth - 2)
    {
        return;
    }

    pSurface[m_iYpos * ScreenWidth + m_iXpos] = ParticlePalette[m_Col];
}

inline void CPumParticle::DrawBlended(DWORD* pSurface)
{
    int r, g, b, p;

    if (m_iYpos < 1)
    {
        return;
    }
    if (m_iYpos > ScreenHeight - 2)
    {
        return;
    }
    if (m_iXpos < 1)
    {
        return;
    }
    if (m_iXpos > ScreenWidth - 2)
    {
        return;
    }

    if (m_Onscreen)
    {
        p = m_iYpos * ScreenWidth + m_iXpos;

        r = g = b = pSurface[p];
        r = (r & 0x00ff0000) >> 16;
        g = (g & 0x0000ff00) >> 8;
        b = (b & 0x000000ff);

        r += ((ParticlePalette[m_Col] & 0x00ff0000) >> 16) >> 1;
        g += ((ParticlePalette[m_Col] & 0x0000ff00) >> 8) >> 1;
        b += (ParticlePalette[m_Col] & 0x000000ff) >> 1;

        if (r > 255)
        {
            r = 255;
        }
        if (g > 255)
        {
            g = 255;
        }
        if (b > 255)
        {
            b = 255;
        }

        pSurface[p] = (r << 16) | (g << 8) | b;

// --------------------------------------------------------

        r = g = b = pSurface[p - 1];
        r = (r & 0x00ff0000) >> 16;
        g = (g & 0x0000ff00) >> 8;
        b = (b & 0x000000ff);

        r += ((ParticlePalette[m_Col] & 0x00ff0000) >> 16) >> 2;
        g += ((ParticlePalette[m_Col] & 0x0000ff00) >> 8) >> 2;
        b += (ParticlePalette[m_Col] & 0x000000ff) >> 2;

        if (r > 255)
        {
            r = 255;
        }
        if (g > 255)
        {
            g = 255;
        }
        if (b > 255)
        {
            b = 255;
        }

        pSurface[p - 1] = (r << 16) | (g << 8) | b;

// --------------------------------------------------------

        r = g = b = pSurface[p + 1];
        r = (r & 0x00ff0000) >> 16;
        g = (g & 0x0000ff00) >> 8;
        b = (b & 0x000000ff);

        r += ((ParticlePalette[m_Col] & 0x00ff0000) >> 16) >> 2;
        g += ((ParticlePalette[m_Col] & 0x0000ff00) >> 8) >> 2;
        b += (ParticlePalette[m_Col] & 0x000000ff) >> 2;

        if (r > 255)
        {
            r = 255;
        }
        if (g > 255)
        {
            g = 255;
        }
        if (b > 255)
        {
            b = 255;
        }

        pSurface[p + 1] = (r << 16) | (g << 8) | b;
// --------------------------------------------------------

        r = g = b = pSurface[p - ScreenWidth];
        r = (r & 0x00ff0000) >> 16;
        g = (g & 0x0000ff00) >> 8;
        b = (b & 0x000000ff);

        r += ((ParticlePalette[m_Col] & 0x00ff0000) >> 16) >> 2;
        g += ((ParticlePalette[m_Col] & 0x0000ff00) >> 8) >> 2;
        b += (ParticlePalette[m_Col] & 0x000000ff) >> 2;

        if (r > 255)
        {
            r = 255;
        }
        if (g > 255)
        {
            g = 255;
        }
        if (b > 255)
        {
            b = 255;
        }

        pSurface[p - ScreenWidth] = (r << 16) | (g << 8) | b;
// --------------------------------------------------------

        r = g = b = pSurface[p + ScreenWidth];
        r = (r & 0x00ff0000) >> 16;
        g = (g & 0x0000ff00) >> 8;
        b = (b & 0x000000ff);

        r += ((ParticlePalette[m_Col] & 0x00ff0000) >> 16) >> 2;
        g += ((ParticlePalette[m_Col] & 0x0000ff00) >> 8) >> 2;
        b += (ParticlePalette[m_Col] & 0x000000ff) >> 2;

        if (r > 255)
        {
            r = 255;
        }
        if (g > 255)
        {
            g = 255;
        }
        if (b > 255)
        {
            b = 255;
        }

        pSurface[p + ScreenWidth] = (r << 16) | (g << 8) | b;

    }

}


// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

CPumParticleSystem::CPumParticleSystem(const int i)
{
    m_Particles = NULL;
    m_Particles = new CPumParticle[i];
    if (m_Particles != NULL)
    {
        m_Max = i;
        m_Current = 0;
    }
}

CPumParticleSystem::~CPumParticleSystem()
{
    if (m_Particles != NULL)
    {
        delete[] m_Particles;
    }
}

void CPumParticleSystem::Add(float x, float y, float i, float j, float accel, float lt, BYTE col, int fl)
{
    if (y < 1.0f)
    {
        return;
    }
    if (y > 198.0f)
    {
        return;
    }
    if (x < 1.0f)
    {
        return;
    }
    if (x > 318.0f)
    {
        return;
    }

    m_Particles[m_Current++].Create(x, y, i, j, accel, (int)lt, col, fl, m_SP);

    if (m_Current >= m_Max)
    {
        m_Current = 0;
    }
}

void CPumParticleSystem::Update(float frameTime)
{
    int i, j;

    for (i = 0; i < m_Max; i++)
    {
        if (m_Particles[i].m_TTL > 0)
        {
            if (m_Particles[i].Update(frameTime))
            {
                if (m_Particles[i].m_Flags & PRT_LETHAL)
                {
                    for (j = 0; j < m_SP->Options->Players; j++)
                    {
                        m_SP->Player[j].HitP(&m_Particles[i]);
                    }
                }
            }
        }
        else
        {
            m_Particles[i].Kill();
        }
    }
}

void CPumParticleSystem::Reset()
{
    int i;

    for (i = 0; i < m_Max; i++)
    {
        m_Particles[i].Kill();
    }
}


void CPumParticleSystem::Draw(DWORD* pSurface)
{
    int i;

    for (i = 0; i < m_Max; i++)
    {
        if (m_Particles[i].m_TTL > 0 && !(m_Particles[i].m_Flags & PRT_FIREEMIT))
        {
            m_Particles[i].Draw(pSurface);
        }
    }
}

void CPumParticleSystem::DrawBlended(DWORD* pSurface)
{
    int i;

    for (i = 0; i < m_Max; i++)
    {
        if (m_Particles[i].m_TTL > 0 && !(m_Particles[i].m_Flags & PRT_FIREEMIT))
        {
            m_Particles[i].DrawBlended(pSurface);
        }
    }
}


// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------


void SpawnParticles(int type, float x, float y, float angle, float spread,
                    float minpower, float maxpower, float friction,
                    int num, int lifetime, BYTE col, CPumSysPointers* sp)
{
    int i, _angle;
    float _power;

    RoundToInt(&_angle, angle);
    _angle &= 255;

    num = (int)((float)num * sp->Options->ParticleFactor);

    if (type == _PRT_BLOOD || type == _PRT_GIB)
    {
        num = (int)(sp->Options->BloodFactor * (float)num);
    }

    for (i = 0; i < num; i++)
    {
        if (spread != 0)
        {
            RoundToInt(&_angle, angle - spread / 2 + spread * RandFloat(0.0f, 1.0f));
            _angle &= 255;
        }
        _power = minpower + (maxpower - minpower) * RandFloat(0.0f, 1.0f);

        switch (type)
        {
            case _PRT_BLOOD:
                sp->Particle->Add(x, y, cosinus[_angle] * _power, sinus[_angle] * _power, 0.992f, 1000.0f, 213 + (rand() & 3),
                                  _PRT_BLOOD);
                break;
            case _PRT_GIB:
                sp->Particle->Add(x, y, cosinus[_angle] * _power, sinus[_angle] * _power, 0.999f, 4000.0f, col, _PRT_GIB);
                break;
            case _PRT_SPARK:
                sp->Particle->Add(x, y, cosinus[_angle] * _power, sinus[_angle] * _power, 0.9f, (float)(145 + (rand() & 63)),
                                  (char)205, _PRT_SPARK);
                break;
            case _PRT_FIREEMIT:
                sp->Particle->Add(x, y, cosinus[_angle] * _power, sinus[_angle] * _power, friction, (float)lifetime, 0, _PRT_FIREEMIT);
                break;
            case _PRT_EXPLOSION:
                sp->Particle->Add(x, y, cosinus[_angle] * _power, sinus[_angle] * _power, 0.9f, 6.0f, (char)205, _PRT_EXPLOSION);
                break;
            case _PRT_FASTSPARK:
                sp->Particle->Add(x, y, cosinus[_angle] * _power, sinus[_angle] * _power, 0.9f, 6.0f, (char)205, _PRT_FASTSPARK);
                break;
            case _PRT_DEBRIS:
                sp->Particle->Add(x, y, cosinus[_angle] * _power, sinus[_angle] * _power, 0.995f, 4000.0f, col, _PRT_DEBRIS);
                break;
            case _PRT_COLOR:
                sp->Particle->Add(x, y, cosinus[_angle] * _power, sinus[_angle] * _power, 0.99f, 128.0f, col, _PRT_COLOR);
                break;
            default:
                sp->Particle->Add(x, y, cosinus[_angle] * _power, sinus[_angle] * _power, friction, (float)lifetime, col, type);
                break;
        }
    }
}

void SpawnFlame(float x, float y, CPumSysPointers* sp)
{
    float f;

    f = RandFloat(-4.0f, 4.0f);

    sp->Particle->Add(x, y - 1.0f, f, 0.0f, 0.87f * TimeFactor, (float)(11 + (rand() & 15)), (char)205, _PRT_FIRE);
}


#define SetPal(i,r,g,b) ParticlePalette[i]=((r*4)<<16)|((g*4)<<8)|(b*4)
void SetParticlePalette()
{
    int i;
    for (i = 0; i < 192; i++)
    {
        SetPal(i, 0, 0, 0);
    }
    SetPal(192, 63, 0, 63);
    SetPal(193, 0, 63, 0);
    SetPal(194, 0, 0, 63);
    SetPal(195, 63, 53, 47);
    SetPal(196, 63, 63, 63);
    SetPal(197, 55, 55, 55);
    SetPal(198, 47, 47, 47);
    SetPal(199, 39, 39, 39);
    SetPal(200, 31, 31, 31);
    SetPal(201, 23, 23, 23);
    SetPal(202, 15, 15, 15);
    SetPal(203, 7, 7, 7);
    SetPal(204, 0, 0, 0);
    SetPal(205, 63, 63, 63);
    SetPal(206, 63, 63, 46);
    SetPal(207, 63, 63, 31);
    SetPal(208, 63, 63, 15);
    SetPal(209, 63, 63, 0);
    SetPal(210, 63, 47, 0);
    SetPal(211, 63, 31, 0);
    SetPal(212, 63, 15, 0);
    SetPal(213, 63, 0, 0);
    SetPal(214, 47, 0, 0);
    SetPal(215, 31, 0, 0);
    SetPal(216, 15, 0, 0);
    SetPal(217, 0, 0, 0);
    SetPal(218, 0, 63, 0);
    SetPal(219, 0, 47, 0);
    SetPal(220, 0, 31, 0);
    SetPal(221, 0, 15, 0);
    SetPal(222, 0, 0, 0);
    SetPal(223, 0, 0, 63);
    SetPal(224, 0, 0, 47);
    SetPal(225, 0, 0, 31);
    SetPal(226, 0, 0, 15);
    SetPal(227, 0, 0, 0);
    SetPal(228, 63, 0, 0);
    SetPal(229, 63, 15, 0);
    SetPal(230, 63, 31, 0);
    SetPal(231, 63, 47, 0);
    SetPal(232, 63, 63, 0);
    SetPal(233, 47, 63, 0);
    SetPal(234, 31, 63, 0);
    SetPal(235, 15, 63, 0);
    SetPal(236, 0, 63, 0);
    SetPal(237, 0, 63, 15);
    SetPal(238, 0, 63, 31);
    SetPal(239, 0, 63, 47);
    SetPal(240, 0, 63, 63);
    SetPal(241, 0, 47, 63);
    SetPal(242, 0, 31, 63);
    SetPal(243, 0, 15, 63);
    SetPal(244, 0, 0, 63);
    SetPal(245, 15, 0, 63);
    SetPal(246, 31, 0, 63);
    SetPal(247, 47, 0, 63);
    SetPal(248, 63, 0, 63);
    SetPal(249, 63, 0, 47);
    SetPal(250, 63, 0, 31);
    SetPal(251, 63, 0, 15);
    SetPal(252, 63, 0, 0);
    SetPal(253, 0, 0, 0);
    SetPal(254, 0, 0, 0);
    SetPal(255, 63, 63, 63);
}