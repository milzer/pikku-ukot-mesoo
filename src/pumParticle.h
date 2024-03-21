#ifndef __PUMPARTICLE_H__
#define __PUMPARTICLE_H__

#include <DieselVector2.h>
#include "pumGlobals.h"
#include "pumSound.h"
#include "pumLevel.h"
//#include "list.h"

extern DWORD ParticlePalette[256];

#define PRT_SPARK       0x0001
#define PRT_FASTSPARK   0x0002
#define PRT_GIB         0x0004
#define PRT_GRAVITY     0x0008
#define PRT_DIEONHIT    0x0010
#define PRT_BOUNCE      0x0020
#define PRT_DEBRIS      0x0040
#define PRT_LETHAL      0x0080
#define PRT_DIGGER      0x0100
#define PRT_DRAWBACK    0x0200
#define PRT_XFRICTION   0x0400
#define PRT_YFRICTION   0x0800
#define PRT_FIRE        0x1000
#define PRT_BLOOD       0x2000
#define PRT_FIREEMIT    0x4000
#define PRT_COLOR       0x8000

#define _PRT_FRICTION   ( PRT_XFRICTION | PRT_YFRICTION )
#define _PRT_FIRE       ( PRT_FIRE | PRT_LETHAL | PRT_XFRICTION )

#define _PRT_BLOOD      ( PRT_BLOOD | PRT_GRAVITY | PRT_XFRICTION | PRT_DRAWBACK | PRT_BOUNCE )
#define _PRT_GIB        ( PRT_GIB | _PRT_FRICTION | PRT_BOUNCE | PRT_GRAVITY )
#define _PRT_SPARK      ( PRT_SPARK | PRT_BOUNCE | PRT_GRAVITY )
#define _PRT_FASTSPARK  ( PRT_FASTSPARK | PRT_BOUNCE )
#define _PRT_FIREEMIT   ( PRT_FIREEMIT | PRT_BOUNCE | PRT_LETHAL | PRT_GRAVITY )
#define _PRT_EXPLOSION  ( PRT_FASTSPARK | PRT_LETHAL | PRT_DIGGER | PRT_DIEONHIT )
#define _PRT_DEBRIS     ( PRT_DEBRIS | PRT_GRAVITY | PRT_LETHAL | _PRT_FRICTION | PRT_DIEONHIT )
#define _PRT_COLOR      ( PRT_GRAVITY | PRT_DIEONHIT | PRT_COLOR )

class CPumParticle
{
public:
    CPumSysPointers* m_SP;

    float m_fXpos;
    float m_fYpos;
    float m_Acceleration;
    float m_Misc;

    CDieselVector2 m_Vec;

    int m_iXpos;
    int m_iYpos;
    int m_Flags;
    int m_TTL;
    int m_Counter;
    int m_Countfreq;

    char m_Onscreen;
    BYTE m_Col;

    inline void Kill();
    inline int Update(float frameTime);
    inline void Draw(DWORD* pSurface);
    inline void DrawBlended(DWORD* pSurface);
    inline void Create(float x, float y, float i, float j, float accel, int lt, BYTE col, int fl, CPumSysPointers* sp);

    CPumParticle();
    ~CPumParticle() {}
};


class CPumParticleSystem
{
protected:
    int m_Max;
    int m_Current;

    CPumParticle* m_Particles;

public:
    CPumSysPointers* m_SP;

    void Add(float x, float y, float i, float j, float accel, float lt, BYTE col, int fl);
    void Update(float frameTime);
    void Draw(DWORD* pSurface);
    void DrawBlended(DWORD* pSurface);
    void Reset();

    CPumParticleSystem(const int i);
    ~CPumParticleSystem();
};


void SetParticlePalette();
void SpawnFlame(float x, float y, CPumSysPointers* sp);
void SpawnParticles(int type, float x, float y, float angle, float spread,
                    float minpower, float maxpower, float friction,
                    int num, int lifetime, BYTE col, CPumSysPointers* sp);

#endif